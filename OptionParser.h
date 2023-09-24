#ifndef OPTION_PARSER_H
#define OPTION_PARSER_H
#include<string>
#include<filesystem>
#include<iostream>

namespace fs = std::filesystem;
struct Options
{
	bool sort, recursive;
	std::string sorting_order; //fd or df

	bool show_only_dirs, show_only_files;
	bool show_as_list, show_as_tree, show_as_table; //only one variable must be true
	std::string dir; //it's current working directory by default

	size_t table_output_width = 4; //default value

	short dir_color = 94, dir_bg_color = 40, file_color = 34, file_bg_color = 40;

	bool show_file_size,
		show_permissions,
		show_last_write_time,
		show_creation_time;
	bool should_compute_formating_size;

	bool print_help;

	bool print_pure;

	inline bool is_regime_showing_ok()
	{
		//check there is only one regime(function simply check only one variable is true)
		return (show_as_list && !show_as_tree && !show_as_table) ||
			   (!show_as_list && show_as_tree && !show_as_table) ||
			   (!show_as_list && !show_as_tree && show_as_table);
	}
	inline bool is_default_output_mode()
	{
		//user did not provided any flag, so output will be default
		return !(show_as_list || show_as_tree || show_as_table);
	}

	Options():
		show_only_dirs(true),
		show_only_files(true),
		show_as_list(false),
		show_as_tree(false),
		show_as_table(false),
		sort(false),
		recursive(false),
		show_file_size(false),
		show_permissions(false),
		show_last_write_time(false),
		show_creation_time(false),
		should_compute_formating_size(false),
		print_help(false),
		print_pure(false),
		dir(fs::current_path().string())
	{
		//l -d -f <=> l 
		/*
			so if you want to show directories and files, then just don't type any
			flag related, but if you want only files, type -f and if you need only
			directories type -d
		*/
	}
	~Options() {}
};

static inline void disable_options(Options* options)
{
	//disable modes that aren't suitable to show information such as file sizes
	// permissions, e.t.c
	options->show_as_list = true;
	options->show_as_tree = false;
	options->show_as_table = false;
}
static inline bool is_option(const std::string& arg)
{
	return std::string("-d-f-l-m-t-s-r-S-p-T-c-h-a-P").find(arg) != std::string::npos;
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
		else if (arg == "-r")options->recursive = true;
		else if (arg == "-S")
		{
			options->show_file_size = true;
			disable_options(options);
		}
		else if (arg == "-p")
		{
			options->show_permissions = true;
			disable_options(options);
		}
		else if (arg == "-T")
		{
			options->show_last_write_time = true;
			disable_options(options);
		}
		else if (arg == "-C")
		{
			options->show_creation_time = true;
			disable_options(options);
		}
		else if (arg == "-a")
		{
			options->show_file_size       = true;
			options->show_creation_time   = true;
			options->show_last_write_time = true;
			options->show_permissions     = true;
			disable_options(options);
		}
		else if (arg == "-h")
		{
			options->print_help = true;
		}
		else if (arg == "-P")
		{
			options->print_pure = true;
		}
		else
		{
			std::cout << "Error:" << arg << " is unknown argument! Try using -h to see help!" << std::endl;
			exit(-1);

		}
	}

	//show all
	if (args.find("-d") != std::string::npos &&
		args.find("-f") != std::string::npos)
	{
		options->show_only_dirs = true;
		options->show_only_files = true;
	}

	if (options->show_permissions || 
		options->show_file_size   || 
		options->show_last_write_time ||
		options->show_creation_time)
		options->should_compute_formating_size = true;

	//if only sort flag is passed, then set default sorting order
	if (options->sort && options->sorting_order.empty())
		options->sorting_order = "df";

	return options;
}

#endif 