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
                dirs.push_back(in::cut_quotas(dir_entry.path().filename().string()));
            else
                files.push_back(in::cut_quotas(dir_entry.path().filename().string()));
        }
        else
        {
            std::cout << in::cut_quotas(dir_entry.path().filename().string()) << std::endl;
        }
    }


    if (options->sort)
    {
        auto print = std::for_each(dirs.begin(), dirs.end(),
            [](const auto& arg) {std::cout << arg << std::endl; });
        for (auto& ch : options->sorting_order)
        {
            if (ch == 'd')
                std::for_each(dirs.begin(), dirs.end(),print);
            if (ch == 'f')
                std::for_each(files.begin(), files.end(), print);
        }
    }
}