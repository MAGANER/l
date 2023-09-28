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
void PrinterInnerFn::iterate_over_dir(const Options* const options,
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
void PrinterInnerFn::iterate_over_dir_recursively(const Options* const options,
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
void PrinterInnerFn::print_d(const std::string& str, const Options* const options)
{
    if (options->print_pure)
    {
        fmt::print("{}", str);
    }
    else
    {
        fmt::print(FG(options->dir_color) | BG(options->dir_bg_color),str);
    }
}
void PrinterInnerFn::print_f(const std::string& str, const Options* const options)
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

size_t PrinterInnerFn::get_max_dir_str_size(const std::string& dir,const Options* const options)
{
    std::vector<size_t> sizes;
    for (const fs::directory_entry& dir_entry : fs::directory_iterator(dir))
    {
        sizes.push_back(PrinterInnerFn::prepare_entry_val(dir_entry, options).size());
    }

    return *std::max_element(sizes.begin(), sizes.end());
}
size_t PrinterInnerFn::get_max_dir_str_size_recursivly(const std::string& dir, const Options* const options)
{
    std::vector<size_t> sizes;
    for (const fs::directory_entry& dir_entry : fs::recursive_directory_iterator(dir))
    {
       sizes.push_back(PrinterInnerFn::prepare_entry_val(dir_entry, options).size());
    }

    return *std::max_element(sizes.begin(), sizes.end());
}
size_t PrinterInnerFn::get_max_dir_file_size_str_size(const std::string& dir, const Options* const options)
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
                buffer << PrinterInnerFn::HumanReadable{f};
                sizes.push_back(buffer.str().size());
            }
            catch (const std::exception& e)
            {

                fmt::print("{}",e.what());
                sizes.push_back(0);
            }
        }
    }

    return *std::max_element(sizes.begin(), sizes.end());
}
size_t PrinterInnerFn::get_max_dir_file_size_str_size_recursivly(const std::string& dir, const Options* const options)
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
                buffer << PrinterInnerFn::HumanReadable{f};
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

void PrinterInnerFn::print_time(const std::string& time, const Options* const options, const std::string& space)
{
    if ((options->show_permissions && !options->show_file_size) ||
        (options->show_permissions && options->show_file_size))
        fmt::print("  ");
    else
        fmt::print("{}",space);

    fmt::print("{}", time);
}
void PrinterInnerFn::show_permissions(const std::string& entry)
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
void PrinterInnerFn::printDirectoryTree(const Options* const options,const fs::path& path, size_t level)
{
    for (const auto& entry : fs::directory_iterator(path)) 
    {
        auto entry_val = cut_quotas(entry.path().filename().string());
        if (fs::is_directory(entry)) 
        {
            fmt::print("{}", std::string(level, '-'));
            print_d(entry_val, options);
            fmt::println("");
            printDirectoryTree(options,entry, level + 1);
        }
        else 
        {
            fmt::print("{}",std::string(level, ' '));
            print_f(entry_val, options);
            fmt::println("");
        }
    }
}

#define MULT_VAL2 max_size2 == 1 ? 1 : (max_size2 - file_size_str_size) + 1
void Printer::print_as_list(const Options* const options)
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
        std::list<fs::directory_entry>& dirs,
        std::list<fs::directory_entry>& files)
    {
        namespace in = PrinterInnerFn;
          
        auto entry_val = in::prepare_entry_val(dir_entry, options);
        //if shouldn't sort just print
        if (options->sort)
        {
            if (dir_entry.is_directory())
                dirs.push_back(fs::directory_entry(dir_entry));
            else
                files.push_back(fs::directory_entry(dir_entry));
                
        }
        else
        {
            if (options->show_only_dirs && dir_entry.is_directory())
            {
                auto val = entry_val + "/";
                PrinterInnerFn::print_d(val, options);
                fmt::println("");
            }
            else if (options->show_only_files && !dir_entry.is_directory())
            {
                auto mult_val = max_size == 1 ? 1 : (max_size - entry_val.size()) + 1;
                PrinterInnerFn::print_f(entry_val, options);
                fmt::print("{}",in::mult_str(" ", mult_val));

                size_t file_size_str_size = 0;
                if (options->show_file_size)
                {
                    //fmt::print("{}",in::HumanReadable{fs::file_size(dir_entry)});
                    try
                    {
                        std::cout << in::HumanReadable{fs::file_size(dir_entry)};
                        std::stringstream buffer;
                        buffer << in::HumanReadable{fs::file_size(dir_entry)};
                        file_size_str_size = buffer.str().size();
                    }
                    catch (const std::exception& e)
                    {
                        fmt::print("{}", e.what());
                    }
                }
                if (options->show_permissions)
                {
                    fmt::print("{}",in::mult_str(" ", MULT_VAL2));
                    in::show_permissions(dir_entry.path().string());
                }
                if (options->show_last_write_time)
                {
                    auto time = get_modification_file_time(dir_entry.path().string());
                    in::print_time(" mod time: " + time, options, in::mult_str(" ", MULT_VAL2));
                }
                if (options->show_creation_time)
                {
                    auto time = get_creation_file_time(dir_entry.path().string());
                    in::print_time(" cre time: " + time, options, in::mult_str(" ", MULT_VAL2));
                }
                fmt::println("");
            }
        }
    };
    
    auto iterate_sorted = [&](std::list<fs::directory_entry>& dirs,
                             std::list<fs::directory_entry>& files)
    {   
        bool show_size = false;
        auto print = [&](const auto& arg) 
        { 
            auto entry_val = in::prepare_entry_val(arg, options);
            auto mult_val = max_size == 1 ? 1 : (max_size - entry_val.size()) + 1;
            
            if (show_size)
                in::print_f(entry_val, options);
            else
                in::print_d(entry_val+"/", options);

            fmt::print("{}", PrinterInnerFn::mult_str(" ", mult_val));

            size_t file_size_str_size = 0;
            if (options->show_file_size && show_size)
            {
                auto f = fs::file_size(arg.path());
                //fmt::print("{}", PrinterInnerFn::HumanReadable{f});
                std::cout << PrinterInnerFn::HumanReadable{f};
                std::stringstream buffer;
                buffer << PrinterInnerFn::HumanReadable{f};
                file_size_str_size = buffer.str().size();

            }
            if (options->show_permissions)
            {
                auto mult_val = max_size2 == 1 ? 1 : (max_size2 - file_size_str_size) + 1;
                fmt::print("{}", PrinterInnerFn::mult_str(" ", mult_val));
                PrinterInnerFn::show_permissions(arg.path().string());
            }
            if (options->show_last_write_time)//if show_size is true, then functions is used to iterate over files
            {
                auto time = get_modification_file_time(arg.path().string());
                in::print_time(" mod time: " + time, options, in::mult_str(" ", MULT_VAL2));
            }
            if (options->show_creation_time)
            {
                auto time = get_creation_file_time(arg.path().string());
                in::print_time(" cre time: " + time, options, in::mult_str(" ", MULT_VAL2));
            }
            
            fmt::println("");
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
void Printer::print_as_table(const Options* const options)
{
    size_t counter = 0;
    auto iterate = [&](const fs::directory_entry& dir_entry,
        std::list<fs::directory_entry>& dirs,
        std::list<fs::directory_entry>& files)
    {
        namespace in = PrinterInnerFn;
        auto entry_val = in::prepare_entry_val(dir_entry, options);

        if (options->sort)
        {
            if (dir_entry.is_directory())
                dirs.push_back(fs::directory_entry(dir_entry));
            else
                files.push_back(fs::directory_entry(dir_entry));
        }
        else
        {
            auto separator = counter == options->table_output_width ? '\n' : ' ';
            if (separator == '\n')counter = 0;

            if (options->show_only_dirs && dir_entry.is_directory())
            {
                PrinterInnerFn::print_d(entry_val, options);
                fmt::print("/{}", separator);
            }
            else if (options->show_only_files && !dir_entry.is_directory())
            {
                PrinterInnerFn::print_f(entry_val, options);
                fmt::print("{}",separator);
            }
            counter++;
        }
    };

    auto iterate_sorted = [&](std::list<fs::directory_entry>& dirs,
        std::list<fs::directory_entry>& files)
    {
        counter = 0;


        auto show_dirs = false;

        char separator = ' ';
        auto print = [&](const auto& arg)
        {
            separator = counter == options->table_output_width ? '\n' : ' ';
            if (separator == '\n')counter = 0;

            auto entry_val = PrinterInnerFn::prepare_entry_val(arg, options);
            auto dir_val = entry_val + "/";
            if (show_dirs)
                PrinterInnerFn::print_d(dir_val, options);
            else
                PrinterInnerFn::print_f(entry_val, options);
            fmt::print("{}",separator);
            counter++;
        };
        for (auto& ch : options->sorting_order)
        {
            if (ch == 'd')
            {
                show_dirs = true;
                std::for_each(dirs.begin(), dirs.end(), print);
            }
            if (ch == 'f')
            {
                show_dirs = false;
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
void Printer::print_as_tree(const Options* const options)
{
    PrinterInnerFn::printDirectoryTree(options,options->dir);
}
void Printer::print_help()
{
    std::string help[] = {
        "This is l help page:",
        "Overall, this program is dedicated to show directory's content and its properties.",
        "It can show file/dir permissions, size, creation and modification time.",
        "It has 3 different modes of output: list, table, tree.",
        "All properties are shown in list mode, so if you wanna show size of files in tree mode,",
        "than program will be forced to print all requested data in list mode.",
        "To see all files and dirs just type l without any flags.",
        "If you don't provide any path as argument, l will show content of current dir,",
        "otherwise it will show content of provided directory.",
        "List of flags:",
        "-d show only directories",
        "-f show only files",
        "-s sort output( -d -f and -f -d are two ways to manage sorted output)",
        "-r recursivly walk through directory",
        "-p show permissions of files/dirs",
        "-T show modification time",
        "-C show creation time",
        "-S show size of each file in directory",
        "-l show as list(by default)",
        "-m show as table",
        "-t show as tree",
        "-a show all information(permissions, sizes, creation/modification times",
        "-P print information without colorizing",
        "-h print help page and break program execution"
    };

    for (auto& l : help)
        fmt::println("{}", l);

    exit(0);
}
