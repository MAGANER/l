#include"Printer.h"

std::string PrinterInnerFn::cut_quotas(const std::string& str)
{
    return str.substr(0, str.size());
}
void Printer::print_as_list(const std::string& dir,
                            const Options const* options)
{
    std::list<std::string> dirs, files;

    namespace in = PrinterInnerFn;
    for (const fs::directory_entry& dir_entry :
        fs::directory_iterator(dir))
    {
        //if shouldn't sort just print
        if (options->sort)
        {
            if (dir_entry.is_directory())
                dirs.push_back(in::cut_quotas(dir_entry.path().filename().string())+"/");
            else
                files.push_back(in::cut_quotas(dir_entry.path().filename().string()));
        }
        else
        {
            auto entry_val = dir_entry.path().filename().string();
            if (options->show_only_dirs and dir_entry.is_directory())
                std::cout << entry_val+"/" << std::endl;
            else if (options->show_only_files and !dir_entry.is_directory())
                std::cout << entry_val << std::endl;
        }
    }


    if (options->sort)
    {
        auto print = [](const auto& arg) {std::cout << arg << std::endl; };
        for (auto& ch : options->sorting_order)
        {
            if (ch == 'd')
                std::for_each(dirs.begin(), dirs.end(),print);
            if (ch == 'f')
                std::for_each(files.begin(), files.end(), print);
        }
    }
}