#include"InnerPrinter.h"

std::string InnerPrinter::cut_quotas(const std::string& str)
{
    return str.substr(0, str.size());
}
void InnerPrinter::erase_sub(std::string& str, const std::string& sub)
{
    auto pos = str.find(sub);
    if (pos != std::string::npos)
    {
        str.erase(pos, sub.length());
    }
}
std::string InnerPrinter::mult_str(const std::string& str, size_t n)
{
    std::string ns;
    for (size_t i = 0; i < n; i++)
        ns += str;
    return ns;
}
std::string InnerPrinter::prepare_entry_val(const fs::directory_entry& dir_entry, unmutable options)
{
    auto entry_val = dir_entry.path().filename().string();
    entry_val = options->recursive ? dir_entry.path().parent_path().string() + "/" + entry_val : entry_val;
    if (options->recursive)
    {
        erase_sub(entry_val, options->dir);
        if (entry_val[0] == '/' || entry_val[0] == '\\')
            entry_val = entry_val.substr(1); //erase weird / or \ at the beginning of str
    }

    return fs::path{entry_val}.string();
};
void InnerPrinter::iterate_over_dir(unmutable options,
    const fn& iterate,
    const fn1& iterate_sorted)
{
    std::list<fs::directory_entry> dirs, files;

    for (const fs::directory_entry& dir_entry : fs::directory_iterator(options->dir))
    {
        iterate(dir_entry, dirs, files);
    }

    if (options->sort)
        iterate_sorted(dirs, files);
}
void InnerPrinter::iterate_over_dir_recursively(unmutable options,
    const fn& iterate,
    const fn1& iterate_sorted)
{
    std::list<fs::directory_entry> dirs, files;

    for (const fs::directory_entry& dir_entry : fs::recursive_directory_iterator(options->dir))
    {
        iterate(dir_entry, dirs, files);
    }

    if (options->sort)
        iterate_sorted(dirs, files);
}
void InnerPrinter::print_d(const std::string& str, unmutable options)
{
    if (options->print_pure)
    {
        fmt::print("{}", str);
    }
    else
    {
        fmt::print(FG(options->dir_color) | BG(options->dir_bg_color), str);
    }
}
void InnerPrinter::print_f(const std::string& str, unmutable options)
{
    if (options->print_pure)
    {
        fmt::print("{}", str);
    }
    else
    {
        fmt::print(FG(options->file_color) | BG(options->file_bg_color), str);
    }
}

size_t InnerPrinter::max_dir_size(const std::string& dir, unmutable options)
{
    std::vector<size_t> sizes;
    for (const fs::directory_entry& dir_entry : fs::directory_iterator(dir))
    {
        sizes.push_back(InnerPrinter::prepare_entry_val(dir_entry, options).size());
    }

    return *std::max_element(sizes.begin(), sizes.end());
}
size_t InnerPrinter::max_dir_size_rec(const std::string& dir, unmutable options)
{
    std::vector<size_t> sizes;
    for (const fs::directory_entry& dir_entry : fs::recursive_directory_iterator(dir))
    {
        sizes.push_back(InnerPrinter::prepare_entry_val(dir_entry, options).size());
    }

    return *std::max_element(sizes.begin(), sizes.end());
}
size_t InnerPrinter::max_file_in_dir(const std::string& dir, unmutable options)
{
    std::vector<size_t> sizes;
    for (const fs::directory_entry& dir_entry : fs::directory_iterator(dir))
    {
        if (!fs::is_directory(dir_entry))
        {
            try
            {
                auto f = fs::file_size(dir_entry);
                std::stringstream buffer;
                buffer << HumanReadable{f};
                sizes.push_back(buffer.str().size());
            }
            catch (const std::exception& e)
            {

                fmt::print("{}", e.what());
                sizes.push_back(0);
            }
        }
    }

    return *std::max_element(sizes.begin(), sizes.end());
}
size_t InnerPrinter::max_file_in_dir_rec(const std::string& dir, unmutable options)
{
    std::vector<size_t> sizes;
    for (const fs::directory_entry& dir_entry : fs::recursive_directory_iterator(dir))
    {
        if (!fs::is_directory(dir_entry))
        {
            try
            {
                auto f = fs::file_size(dir_entry);
                std::stringstream buffer;
                buffer << HumanReadable{f};
                sizes.push_back(buffer.str().size());
            }
            catch (const std::exception& e)
            {

                fmt::print("{}", e.what());
                sizes.push_back(0);
            }
        }
    }

    return *std::max_element(sizes.begin(), sizes.end());
}

void InnerPrinter::print_time(const std::string& time, unmutable options, const std::string& space)
{
    if ((options->show_permissions && !options->show_file_size) ||
        (options->show_permissions && options->show_file_size))
        fmt::print("  ");
    else
        fmt::print("{}", space);

    fmt::print("{}", time);
}
void InnerPrinter::show_permissions(const std::string& entry)
{
    auto p = fs::status(entry).permissions();
    using std::filesystem::perms;
    auto show = [=](char op, perms perm)
    {
        fmt::print("{}", perms::none == (perm & p) ? '-' : op);
    };

    fmt::print(" ");
    show('r', perms::owner_read);
    show('w', perms::owner_write);
    show('x', perms::owner_exec);
    show('r', perms::group_read);
    show('w', perms::group_write);
    show('x', perms::group_exec);
    show('r', perms::others_read);
    show('w', perms::others_write);
    show('x', perms::others_exec);
}
void InnerPrinter::printDirectoryTree(unmutable options, const fs::path& path, size_t level)
{
    for (const auto& entry : fs::directory_iterator(path))
    {
        auto entry_val = cut_quotas(entry.path().filename().string());

        if (fs::is_directory(entry))
        {
            fmt::print("{}", std::string(level, '-'));
            print_d(entry_val, options);
            fmt::println("");
            printDirectoryTree(options, entry, level + 1);
        }
        else
        {
            fmt::print("{}", std::string(level, ' '));
            print_f(entry_val, options);
            fmt::println("");
        }
    }
}
bool InnerPrinter::does_matches(const std::string& str, const std::string& regex)
{
    std::regex _regex(regex);
    return std::regex_search(str, _regex);
}
size_t InnerPrinter::compute_dir_elements_number(const std::string& path, bool rec)
{
    using std::filesystem::directory_iterator;
    using std::filesystem::recursive_directory_iterator;

    using fp = bool (*)(const std::filesystem::path&);

    if (!rec)
        return std::count_if(directory_iterator(path), directory_iterator{}, (fp)std::filesystem::is_regular_file) +
        std::count_if(directory_iterator(path), directory_iterator{}, (fp)std::filesystem::is_directory);
    else
        return std::count_if(recursive_directory_iterator(path), recursive_directory_iterator{}, (fp)std::filesystem::is_regular_file) +
        std::count_if(recursive_directory_iterator(path), recursive_directory_iterator{}, (fp)std::filesystem::is_directory);
}