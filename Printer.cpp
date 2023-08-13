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



void Printer::print_as_list(const Options const* options)
{
    

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
                std::cout << colorize(entry_val,options->file_color,options->file_bg_color) << std::endl;
            }
        }
    };
    
    auto iterate_sorted = [&](std::list<std::string>& dirs,
                             std::list<std::string>& files)
    {
        bool dir = false;
        auto print = [&](const auto& arg) 
        {
            auto val = dir ? colorize(arg, options->dir_color, options->dir_bg_color) :
                colorize(arg, options->file_color, options->file_bg_color);
            std::cout <<val << std::endl; 
        };
        for (auto& ch : options->sorting_order)
        {
            if (ch == 'd')
            {
                dir = true;
                std::for_each(dirs.begin(), dirs.end(), print);
            }
            if (ch == 'f')
            {
                dir = false;
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