#include"Printer.h"

std::string PrinterInnerFn::cut_quotas(const std::string& str)
{
    return str.substr(0, str.size());
}
void PrinterInnerFn::erase_sub(std::string& str, const std::string& sub)
{
    auto pos = str.find(sub);
    if (pos != std::string::npos)
    {
        str.erase(pos, sub.length());
    }
}
void PrinterInnerFn::iterate_over_dir(const Options const* options,
                  const fn& iterate,
                  const fn1& iterate_sorted)
{
    std::list<std::string> dirs, files;

    for (const fs::directory_entry& dir_entry : fs::directory_iterator(options->dir))
    {
        iterate(dir_entry, dirs, files);
    }

    if (options->sort)
        iterate_sorted(dirs, files);
}
void PrinterInnerFn::iterate_over_dir_recursively(const Options const* options,
                    const fn& iterate,
                    const fn1& iterate_sorted)
{
    std::list<std::string> dirs, files;

    for (const fs::directory_entry& dir_entry : fs::recursive_directory_iterator(options->dir))
    {
        iterate(dir_entry, dirs, files);
    }

    if (options->sort)
        iterate_sorted(dirs, files);
}
size_t PrinterInnerFn::get_max_dir_str_size(const std::string& dir,const Options const* options)
{
    std::vector<size_t> sizes;
    for (const fs::directory_entry& dir_entry : fs::directory_iterator(dir))
    {
        sizes.push_back(PrinterInnerFn::prepare_entry_val(dir_entry, options).size());
    }

    return *std::max_element(sizes.begin(), sizes.end());
}
size_t PrinterInnerFn::get_max_dir_str_size_recursivly(const std::string& dir, const Options const* options)
{
    std::vector<size_t> sizes;
    for (const fs::directory_entry& dir_entry : fs::recursive_directory_iterator(dir))
    {
        sizes.push_back(PrinterInnerFn::prepare_entry_val(dir_entry, options).size());
    }

    return *std::max_element(sizes.begin(), sizes.end());
}
size_t PrinterInnerFn::get_max_dir_file_size_str_size(const std::string& dir, const Options const* options)
{
    std::vector<size_t> sizes;
    for (const fs::directory_entry& dir_entry : fs::directory_iterator(dir))
    {
        std::stringstream buffer;
        buffer << PrinterInnerFn::HumanReadable{fs::file_size(dir_entry)};
        sizes.push_back(buffer.str().size());
    }

    return *std::max_element(sizes.begin(), sizes.end());
}
size_t PrinterInnerFn::get_max_dir_file_size_str_size_recursivly(const std::string& dir, const Options const* options)
{
    std::vector<size_t> sizes;
    for (const fs::directory_entry& dir_entry : fs::recursive_directory_iterator(dir))
    {
        std::stringstream buffer;
        buffer << PrinterInnerFn::HumanReadable{fs::file_size(dir_entry)};
        sizes.push_back(buffer.str().size());
    }

    return *std::max_element(sizes.begin(), sizes.end());
}

void PrinterInnerFn::show_permissions(const std::string& entry)
{
    auto p = fs::status(entry).permissions();

    using std::filesystem::perms;
    auto show = [=](char op, perms perm)
    {
        std::cout << (perms::none == (perm & p) ? '-' : op);
    };

    std::cout << " ";
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
void PrinterInnerFn::printDirectoryTree(const Options const* options,const fs::path& path, size_t level)
{
    for (const auto& entry : fs::directory_iterator(path)) 
    {
        auto entry_val = cut_quotas(entry.path().filename().string());
        if (fs::is_directory(entry)) 
        {
            std::cout<< std::string(level,'-') << colorize(entry_val, options->dir_color, options->dir_bg_color) << "/" << std::endl;
            printDirectoryTree(options,entry, level + 1);
        }
        else 
        {
            std::cout << std::string(level, ' ')<< "|" << colorize(entry_val, options->file_color, options->file_bg_color) << std::endl;
        }
    }
}


void Printer::print_as_list(const Options const* options)
{
    namespace in = PrinterInnerFn;
    //this variable is required to compute indent between file name and its size
    size_t max_size = options->should_compute_formating_size ?
        options->recursive ? in::get_max_dir_str_size_recursivly(options->dir, options) :
        in::get_max_dir_str_size(options->dir, options) : 1;

    //this variable is required to compute indent between file size and its permissions string
    size_t max_size2 = options->should_compute_formating_size ?
        options->recursive ? in::get_max_dir_file_size_str_size_recursivly(options->dir, options) :
        in::get_max_dir_file_size_str_size(options->dir, options): 1;

    auto iterate = [&](const fs::directory_entry& dir_entry,
        std::list<std::string>& dirs,
        std::list<std::string>& files)
    {
        namespace in = PrinterInnerFn;
           
        auto entry_val = in::prepare_entry_val(dir_entry, options);
        //if shouldn't sort just print
        if (options->sort)
        {
            if (dir_entry.is_directory())
                dirs.push_back(in::cut_quotas(entry_val) + "/");
            else
                files.push_back(in::cut_quotas(entry_val));
                
        }
        else
        {
            if (options->show_only_dirs and dir_entry.is_directory())
            {
                std::cout << colorize(entry_val + "/",options->dir_color,options->dir_bg_color) << std::endl;
            }
            else if (options->show_only_files and !dir_entry.is_directory())
            {
                auto mult_val = max_size == 1 ? 1 : (max_size - entry_val.size()) + 1;
                std::cout << colorize(entry_val, 
                                      options->file_color, 
                                      options->file_bg_color) 
                     << in::mult_str(" ",mult_val);


                size_t file_size_str_size = 0;
                if (options->show_file_size)
                {
                    std::cout << in::HumanReadable{fs::file_size(dir_entry)};
                    std::stringstream buffer;
                    buffer << in::HumanReadable{fs::file_size(dir_entry)};
                    file_size_str_size = buffer.str().size();
                }
                if (options->show_permissions)
                {
                    auto mult_val = max_size2 == 1 ? 1 : (max_size2 - file_size_str_size) + 1;
                    std::cout << in::mult_str(" ", mult_val);
                    in::show_permissions(dir_entry.path().string());
                }
                std::cout << std::endl;

            }
        }
    };
    
    auto iterate_sorted = [&](std::list<std::string>& dirs,
                             std::list<std::string>& files)
    {   
        bool show_size = false;
        auto print = [&](const auto& arg) 
        { 
            auto mult_val = max_size == 1 ? 1 : (max_size - arg.size()) + 1;
            
            auto out = show_size ? colorize(arg, options->dir_color, options->dir_bg_color) :
                colorize(arg, options->file_color, options->file_bg_color);
            std::cout << out << PrinterInnerFn::mult_str(" ",mult_val);

            size_t file_size_str_size = 0;
            if (options->show_file_size and show_size)
            {
                std::cout << PrinterInnerFn::HumanReadable{fs::file_size(arg)};
                std::stringstream buffer;
                buffer << PrinterInnerFn::HumanReadable{fs::file_size(arg)};
                file_size_str_size = buffer.str().size();
            }
            if (options->show_permissions)
            {
                auto mult_val = max_size2 == 1 ? 1 : (max_size2 - file_size_str_size) + 1;
                std::cout << PrinterInnerFn::mult_str(" ", mult_val);
                PrinterInnerFn::show_permissions(arg);
            }
            
            std::cout << std::endl;
        };
        for (auto& ch : options->sorting_order)
        {
            if (ch == 'd')
            {
                show_size = false;
                std::for_each(dirs.begin(), dirs.end(), print);
            }
            if (ch == 'f')
            {
                show_size = true;
                std::for_each(files.begin(), files.end(), print);
            }
        }
    };

    if (options->recursive)
    {
        PrinterInnerFn::iterate_over_dir_recursively(options, iterate, iterate_sorted);
    }
    else
        PrinterInnerFn::iterate_over_dir(options, iterate, iterate_sorted);


}
void Printer::print_as_table(const Options const* options)
{
    size_t counter = 0;
    auto iterate = [&](const fs::directory_entry& dir_entry,
        std::list<std::string>& dirs,
        std::list<std::string>& files)
    {
        namespace in = PrinterInnerFn;
        auto entry_val = in::prepare_entry_val(dir_entry, options);

        if (options->sort)
        {
            if (dir_entry.is_directory())
                dirs.push_back(colorize(in::cut_quotas(entry_val) + "/",options->dir_color,options->dir_bg_color));
            else
                files.push_back(colorize(in::cut_quotas(entry_val),options->file_color,options->file_bg_color));
        }
        else
        {
            auto separator = counter == options->table_output_width ? '\n' : ' ';
            if (separator == '\n')counter = 0;

            if (options->show_only_dirs and dir_entry.is_directory())
                std::cout << colorize(entry_val,options->dir_color,options->dir_bg_color) + "/" << separator;
            else if (options->show_only_files and !dir_entry.is_directory())
                std::cout << colorize(entry_val,options->file_color,options->file_bg_color) << separator;

            counter++;
        }
    };

    auto iterate_sorted = [&](std::list<std::string>& dirs,
        std::list<std::string>& files)
    {
        counter = 0;

        char separator = ' ';
        auto print = [&](const auto& arg)
        {
            separator = counter == options->table_output_width ? '\n' : ' ';
            if (separator == '\n')counter = 0;

            std::cout << arg << separator;
            counter++;
        };
        for (auto& ch : options->sorting_order)
        {
            if (ch == 'd')
                std::for_each(dirs.begin(), dirs.end(), print);
            if (ch == 'f')
                std::for_each(files.begin(), files.end(), print);
        }
    };
   
    if (options->recursive)
    {
        PrinterInnerFn::iterate_over_dir_recursively(options, iterate, iterate_sorted);
    }
    else
        PrinterInnerFn::iterate_over_dir(options, iterate, iterate_sorted);
}
void Printer::print_as_tree(const Options const* options)
{
    PrinterInnerFn::printDirectoryTree(options,options->dir);
}