#ifndef PRINTER_H
#define PRINTER_H

#include"InnerPrinter.h"
namespace Printer
{
	namespace fs = std::filesystem;

	extern void print_as_table(const Options* const options);
	extern void print_as_list(const Options* const options);
	extern void print_as_tree(const Options* const options);

	extern void print_help();
};

#endif
