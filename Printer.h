#ifndef PRINTER_H
#define PRINTER_H
#include"OptionParser.h"
#include<list>

//namespace contains functions used by Printer's namespace ones
//they aren't dedicated to be used out of Printer namespace
namespace PrinterInnerFn
{
	extern inline std::string cut_quotas(const std::string& str);
}
namespace Printer
{
	namespace fs = std::filesystem;

	extern void print_as_table(const std::string& dir, const Options const* options);
	extern void print_as_list(const std::string& dir,const Options const* options);
	extern void print_as_tree(const std::string& dir, const Options const* options);
};

#endif