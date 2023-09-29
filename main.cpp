#include"OptionParser.h"
#include"Printer.h"
#include"ConfigReader.h"
#include<clocale>

int main(int argc, char** argv)
{
	std::setlocale(LC_ALL, ""); //use default system locale

	auto options = parse_args(argc, argv);
	read_config_file(options);

	//check there are no exluding flags
	//because you can show only as list, table or tree
	if (!options->is_regime_showing_ok() && !options->is_default_output_mode())
	{
		fmt::print("Error: you can use only one flag: -l, -m || -t, because  they exclude each other!");
		exit(-1);
	}


	//it's common practice to print help page and then quit
	if (options->print_help)
	{
		Printer::print_help();
		delete options;
		return 0;
	}

	//default mode is equal to list showing mode
	if (options->is_default_output_mode() || options->show_as_list)
		Printer::print_as_list(options);
	else if (options->show_as_table)
		Printer::print_as_table(options);
	else if (options->show_as_tree)
		Printer::print_as_tree(options);

	delete options;
	return 0;
}
