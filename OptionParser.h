#ifndef OPTION_PARSER_H
#define OPTION_PARSER_H
#include<string>
#include<filesystem>
#include<iostream>

namespace fs = std::filesystem;
struct Options
{
	bool sort;
	std::string sorting_order; //fd or df

	bool show_only_dirs, show_only_files;
	bool show_as_list, show_as_tree, show_as_table; //only one variable must be true
	std::string dir; //it's current working directory by default

	inline bool is_regime_showing_ok()
	{
		//check there is only one regime(function simply check only one variable is true)
		return (show_as_list and !show_as_tree and !show_as_table) or
			   (!show_as_list and show_as_tree and !show_as_table) or
			   (!show_as_list and !show_as_tree and show_as_table);
	}
	inline bool is_default_output_mode()
	{
		//user did not provided any flag, so output will be default
		return !(show_as_list or show_as_tree or show_as_table);
	}

	Options() 
	{
		dir = fs::current_path().string();
		//l -d -f <=> l 
		/*
			so if you want to show directories and files, then just don't type any
			flag related, but if you want only files, type -f and if you need only
			directories type -d
		*/
		show_only_dirs  = true;
		show_only_files = true;

		show_as_list	= false;
		show_as_tree    = false;
		show_as_table   = false;

		sort = false;
	}
	~Options() {}
};

static inline bool is_option(const std::string& arg)
{
	return std::string("-d-f-l-m-t-s").find(arg) != std::string::npos;
}
static Options* parse_args(int argc, char** argv)
{
	Options* options = new Options();
	std::string args;

	for (size_t i = 1; i < argc; i++)
	{
		//first, check argument wasn't added already
		auto arg = std::string(argv[i]);
		if (args.find(arg) == std::string::npos)
			args += arg;
		else
		{
			std::cout <<"Error:"<< arg << " is already added as flag!" << std::endl;
		}

		if (fs::is_directory(arg))
		{
			options->dir = arg;
		}
		else if (not is_option(arg))
		{
			std::cout << "Error:" << arg << " is expected to be a directory!" << std::endl;
			exit(-1);
		}
		else if (arg == "-d")
		{
			options->show_only_files = false;
			options->sorting_order += "d";
		}
		else if (arg == "-f")
		{
			options->show_only_dirs = false;
			options->sorting_order += "f";
		}
		else if (arg == "-l") options->show_as_list = true;
		else if (arg == "-m") options->show_as_table = true;//m means matrix
		else if (arg == "-t") options->show_as_tree = true;
		else if (arg == "-s")options->sort = true;
		else
		{
			std::cout << "Error:" << arg << " is unknown argument! Try using -h to see help!" << std::endl;
			exit(-1);

		}
	}

	//show all
	if (args.find("-d") != std::string::npos and
		args.find("-f") != std::string::npos)
	{
		options->show_only_dirs = true;
		options->show_only_files = true;
	}

	//if only sort flag is passed, then set default sorting order
	if (options->sort and options->sorting_order.empty())
		options->sorting_order = "df";

	return options;
}

#endif 