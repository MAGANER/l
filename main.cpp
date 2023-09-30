#include"OptionParser.h"
#include"Printer.h"
#include"ConfigReader.h"
#include<clocale>

int main(int argc, char** argv)
{
	std::setlocale(LC_ALL, ""); //use default system locale

	auto options = parse_args(argc, argv);
	read_config_file(options);

	//fixes problem i found on arch linux
	//(on windows it is ignored and nothing is printed)
	if (!fs::exists(options->dir))
	{
		fmt::print("Error: {} doesn't exist!", options->dir);
		exit(-1);
	}

	//check there are no exluding flags
	//because you can show only as list, table or tree
	if (!options->is_regime_showing_ok() && !options->is_default_output_mode())
	{
		fmt::print("Error: you can use only one flag: -l, -m || -t, because  they exclude each other!");
		exit(-1);
	}


	//it's common practice to print help page and then quit
	if (SHOW_HELP)
	{
		Printer::print_help();
		delete options;
		return 0;
	}

	//default mode is equal to list showing mode
	if (options->is_default_output_mode() || SHOW_AS_LIST)
		Printer::print_as_list(options);
	else if (SHOW_AS_TABLE)
		Printer::print_as_table(options);
	else if (SHOW_AS_TREE)
		Printer::print_as_tree(options);

	delete options;
	return 0;
}
