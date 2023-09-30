#include"Printer.h"
#define MULT_VAL2 max_size2 == 1 ? 1 : (max_size2 - file_size_str_size) + 1
#define SPACE_PAD std::string(MULT_VAL2,' ')
#define PAD(x) std::string(x,' ')

void Printer::print_as_list(const Options* const options)
{
    namespace in = InnerPrinter;
    //this variable is required to compute indent between file name and its size
    size_t max_size = FORMAT ?
        RECURSIVE ? in::max_dir_size_rec(options->dir, options) :
        in::max_dir_size(options->dir, options) : 1;

    //this variable is required to compute indent between file size and its permissions string
    size_t max_size2 = FORMAT ?
       RECURSIVE ? in::max_file_in_dir_rec(options->dir, options) :
        in::max_file_in_dir(options->dir, options): 1;

    auto iterate = [&](const fs::directory_entry& dir_entry,
        std::list<fs::directory_entry>& dirs,
        std::list<fs::directory_entry>& files)
    {          
        auto entry_val = in::prepare_entry_val(dir_entry, options);

        
        //if it doesn't match the expression just break the execution
        if (USE_REGEX and !in::does_matches(entry_val, options->regex_val))
            return;

        //if shouldn't sort just print
        if (SORT)
        {
            if (dir_entry.is_directory())
                dirs.push_back(fs::directory_entry(dir_entry));
            else
                files.push_back(fs::directory_entry(dir_entry));
                
        }
        else
        {
            if (SHOW_DIRS_ONLY && dir_entry.is_directory())
            {
                auto val = entry_val + "/";
                InnerPrinter::print_d(val, options);
                fmt::println("");
            }
            else if (SHOW_FILES_ONLY && !dir_entry.is_directory())
            {
                auto mult_val = max_size == 1 ? 1 : (max_size - entry_val.size()) + 1;
                InnerPrinter::print_f(entry_val, options);
                fmt::print("{}",PAD(mult_val));

                
                size_t file_size_str_size = 0;
                if (SHOW_FILE_SIZE)
                {
                    try
                    {
                        file_size_str_size = in::print_size(dir_entry.path().string(), options).size();
                    }
                    catch (const std::exception& e)
                    {
                        fmt::print("{}", e.what());
                    }
                }
                if (SHOW_PERMISSION)
                {
                    fmt::print("{}", SPACE_PAD);
                    in::show_permissions(dir_entry.path().string(), options);
                }
                if (SHOW_WRITE_TIME)
                {
                    auto time = get_modification_file_time(dir_entry.path().string());
                    in::print_time(" mod time: " + time, options, SPACE_PAD);
                }
                if (SHOW_CREAT_TIME)
                {
                    auto time = get_creation_file_time(dir_entry.path().string());
                    in::print_time(" cre time: " + time, options, SPACE_PAD);
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

            fmt::print("{}", PAD(mult_val));

            size_t file_size_str_size = 0;
            if (SHOW_FILE_SIZE && show_size)
            {
                file_size_str_size = in::print_size(arg.path().string(),options).size();
            }
            if (SHOW_PERMISSION)
            {
                auto mult_val = max_size2 == 1 ? 1 : (max_size2 - file_size_str_size) + 1;
                fmt::print("{}", PAD(mult_val));
                InnerPrinter::show_permissions(arg.path().string(),options);
            }
            if (SHOW_WRITE_TIME)//if show_size is true, then functions is used to iterate over files
            {
                auto time = get_modification_file_time(arg.path().string());
                in::print_time(" mod time: " + time, options, SPACE_PAD);
            }
            if (SHOW_CREAT_TIME)
            {
                auto time = get_creation_file_time(arg.path().string());
                in::print_time(" cre time: " + time, options, SPACE_PAD);
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


    if (TOTAL_NUMBER)
    {
        auto n = InnerPrinter::compute_dir_elements_number(options->dir, RECURSIVE);
        fmt::print(fmt::fg(fmt::terminal_color::white)| fmt::bg(fmt::terminal_color::black), "total {}\n", n);
    }
    if (RECURSIVE)
    {
        InnerPrinter::iterate_over_dir_recursively(options, iterate, iterate_sorted);
    }
    else
        InnerPrinter::iterate_over_dir(options, iterate, iterate_sorted);


}
void Printer::print_as_table(const Options* const options)
{
    size_t counter = 0;
    auto iterate = [&](const fs::directory_entry& dir_entry,
        std::list<fs::directory_entry>& dirs,
        std::list<fs::directory_entry>& files)
    {
        namespace in = InnerPrinter;
        auto entry_val = in::prepare_entry_val(dir_entry, options);


        //if you use regex, then break running in case entry val doesn't match regular expression
        if (USE_REGEX and !in::does_matches(entry_val, options->regex_val))
            return;

        if (SORT)
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

            if (SHOW_DIRS_ONLY && dir_entry.is_directory())
            {
                InnerPrinter::print_d(entry_val, options);
                fmt::print("/{}", separator);
            }
            else if (SHOW_FILES_ONLY && !dir_entry.is_directory())
            {
                InnerPrinter::print_f(entry_val, options);
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

            auto entry_val = InnerPrinter::prepare_entry_val(arg, options);
            //if you use regex, then break running in case entry val doesn't match regular expression
            if (USE_REGEX and !InnerPrinter::does_matches(entry_val, options->regex_val))
                return;

            auto dir_val = entry_val + "/";
            if (show_dirs)
                InnerPrinter::print_d(dir_val, options);
            else
                InnerPrinter::print_f(entry_val, options);
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
   

    if (TOTAL_NUMBER)
    {
        auto n = InnerPrinter::compute_dir_elements_number(options->dir, RECURSIVE);
        fmt::println("total {}", n);
    }
    if (RECURSIVE)
    {
        InnerPrinter::iterate_over_dir_recursively(options, iterate, iterate_sorted);
    }
    else
        InnerPrinter::iterate_over_dir(options, iterate, iterate_sorted);
}
void Printer::print_as_tree(const Options* const options)
{
    InnerPrinter::printDirectoryTree(options,options->dir);
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
        "-h print help page and break program execution",
        "-n print total number of elements in directory"
    };

    for (auto& l : help)
        fmt::println("{}", l);

    exit(0);
}