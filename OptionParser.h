#ifndef OPTION_PARSER_H
#define OPTION_PARSER_H
#include<unordered_map>
#include<string>
#include<filesystem>
#include<regex>
#include<array>
#include<functional>
#include"fmt/core.h"


//these macroses are used to check options
#define SORT            options->flags[0]
#define RECURSIVE       options->flags[1]
#define SHOW_DIRS_ONLY  options->flags[2]
#define SHOW_FILES_ONLY options->flags[3]
#define SHOW_AS_LIST    options->flags[4]
#define SHOW_AS_TREE    options->flags[5]
#define SHOW_AS_TABLE   options->flags[6]
#define USE_REGEX       options->flags[7]
#define TOTAL_NUMBER    options->flags[8]
#define SHOW_FILE_SIZE  options->flags[9]
#define SHOW_PERMISSION options->flags[10]
#define SHOW_WRITE_TIME options->flags[11]
#define SHOW_CREAT_TIME options->flags[12]
#define SHOW_HELP       options->flags[13]
#define PRINT_PURE      options->flags[14]
#define FORMAT          options->flags[15]

//bunch of special inner macroses for Options structure
#define _SHOW_AS_LIST    flags[4]
#define _SHOW_AS_TREE    flags[5]
#define _SHOW_AS_TABLE   flags[6]
//

//these macroses are used to set associated values
#define _SET_SORT(x)            options->flags[0]=x;
#define _SET_RECURSIVE(x)       options->flags[1]=x;
#define _SET_SHOW_DIRS_ONLY(x)  options->flags[2]=x;
#define _SET_SHOW_FILES_ONLY(x) options->flags[3]=x;
#define _SET_SHOW_AS_LIST(x)    options->flags[4]=x;
#define _SET_SHOW_AS_TREE(x)    options->flags[5]=x;
#define _SET_SHOW_AS_TABLE(x)   options->flags[6]=x;
#define _SET_USE_REGEX(x)       options->flags[7]=x;
#define _SET_TOTAL_NUMBER(x)    options->flags[8]=x;
#define _SET_SHOW_FILE_SIZE(x)  options->flags[9]=x;
#define _SET_SHOW_PERMISSION(x) options->flags[10]=x;
#define _SET_SHOW_WRITE_TIME(x) options->flags[11]=x;
#define _SET_SHOW_CREAT_TIME(x) options->flags[12]=x;
#define _SET_SHOW_HELP(x)       options->flags[13]=x;
#define _SET_PRINT_PURE(x)      options->flags[14]=x;
#define _SET_FORMAT(x)          options->flags[15]=x;



namespace fs = std::filesystem;
struct Options
{
	/*
		Flags ids:
		0  -s sort              
		1  -r recursive
		2  -d show only dirs
		3  -f show only files
		4  -l show as list
		5  -t show as tree
		6  -m show as table
		7  (no key) use regex
		8  -n show total number
		9  -S show file size
		10 -p show permissions
		11 -T show last write time
		12 -C show creation time
		13 -h print help
		14 -P print pure 
		15 (no key) format
	*/
	std::array<bool, 16> flags{false};


	std::string sorting_order, //fd or df
				dir,           //it's current working directory by default
				regex_val;     //match files with this string if user provided regular expression in path argument


	size_t table_output_width = 4; //strings number in one line.(option of table output mode)

	//color values
	uint8_t dir_color = 32,
		dir_bg_color = 30,
		file_color = 35,
		file_bg_color = 30,
		file_size_color = 33,
		file_size_bg_color = 30,
		perm_color = 34,
		perm_bg_color = 30,
		mod_time_color = 35,
		mod_time_bg_color = 30,
		creat_time_color = 36,
		creat_time_bg_color =30;

	inline bool is_regime_showing_ok()
	{
		//check there is only one regime(function simply check only one variable is true)
		return (_SHOW_AS_LIST && !_SHOW_AS_TREE && !_SHOW_AS_TABLE) ||
			   (!_SHOW_AS_LIST && _SHOW_AS_TREE && !_SHOW_AS_TABLE) ||
			   (!_SHOW_AS_LIST && !_SHOW_AS_TREE && _SHOW_AS_TABLE);
	}
	inline bool is_default_output_mode()
	{
		//user did not provided any flag, so output will be default
		return !(_SHOW_AS_LIST || _SHOW_AS_TREE || _SHOW_AS_TABLE);
	}

	Options():
		dir(fs::current_path().string())
	{
		flags[4] = true; //show as list by default
		flags[2] = true; //show dirs by default
		flags[3] = true; //show files by default
		//l -d -f <=> l 
		/*
			so if you want to show directories and files, then just don't type any
			flag related, but if you want only files, type -f and if you need only
			directories type -d
		*/
	}
	~Options() {}
};

static bool is_valid_regex(const std::string& regex) 
{
	try {
		std::regex re(regex);
	}
	catch (const std::regex_error&) 
	{
		return false;
	}
	return true;
}


//option list contains all possible flags passed to the program
static const auto option_list = std::string("-d-f-l-m-t-s-r-S-p-T-c-h-a-P-n");
static inline bool is_option(const std::string& arg)
{
	return option_list.find(arg) != std::string::npos;
}


//disable modes that aren't suitable to show information such as file sizes
// permissions, e.t.c
#define DISABLE_OPTIONS _SET_SHOW_AS_LIST(true) _SET_SHOW_AS_TREE(false) _SET_SHOW_AS_TABLE(false)

//parsing table is made of key that is flag passed to the program and and lamda that is used to set option val
static std::unordered_map<std::string, std::function<void(Options* options)>> get_parsing_table()
{
	std::unordered_map<std::string, std::function<void(Options* options)>> table;
	table.insert_or_assign("-d", [&](Options* options)
		{
			_SET_SHOW_DIRS_ONLY(true)
			_SET_SHOW_FILES_ONLY(false)
			options->sorting_order += "d";
		});
	table.insert_or_assign("-f", [&](Options* options)
	{
			_SET_SHOW_DIRS_ONLY(false)
			_SET_SHOW_FILES_ONLY(true)
			options->sorting_order += "f";
	});
	table.insert_or_assign("-l", [&](Options* options)
	{
		_SET_SHOW_AS_LIST(true)
	});
	table.insert_or_assign("-t", [&](Options* options)
	{
		_SET_SHOW_AS_TREE(true)
	});
	table.insert_or_assign("-m", [&](Options* options)
	{
		_SET_SHOW_AS_TABLE(true)
	});
	table.insert_or_assign("-s", [&](Options* options)
		{
			_SET_SORT(true)
		});
	table.insert_or_assign("-r", [&](Options* options)
	{
		_SET_RECURSIVE(true)
	});
	table.insert_or_assign("-S", [&](Options* options)
	{
		_SET_SHOW_FILE_SIZE(true)
		DISABLE_OPTIONS
	});
	table.insert_or_assign("-p", [&](Options* options)
	{
		_SET_SHOW_PERMISSION(true)
		DISABLE_OPTIONS
	});
	table.insert_or_assign("-T", [&](Options* options)
	{
		_SET_SHOW_WRITE_TIME(true)
		DISABLE_OPTIONS
	});
	table.insert_or_assign("-C", [&](Options* options)
	{
		_SET_SHOW_CREAT_TIME(true)
		DISABLE_OPTIONS
	});
	table.insert_or_assign("-a", [&](Options* options)
	{
		_SET_SHOW_FILE_SIZE(true)
		_SET_SHOW_CREAT_TIME(true)
		_SET_SHOW_WRITE_TIME(true)
		_SET_SHOW_PERMISSION(true)
		DISABLE_OPTIONS
	});
	table.insert_or_assign("-h", [&](Options* options)
	{
		_SET_SHOW_HELP(true)
	});
	table.insert_or_assign("-P", [&](Options* options)
	{
		_SET_PRINT_PURE(true)
	});
	table.insert_or_assign("-n", [&](Options* options)
	{
		_SET_TOTAL_NUMBER(true)
	});

	return table;
}

static Options* parse_args(int argc, char** argv)
{
	static const auto parsing_table = get_parsing_table();
	Options* options = new Options();

	std::string args;//already added args
	for (size_t i = 1; i < argc; i++)
	{
		//first, check argument wasn't added already
		auto arg = std::string(argv[i]);
		if (args.find(arg) == std::string::npos)
			args += arg;
		else
		{
			fmt::print("Error {} is already added as flag!", arg);
			exit(-1);
		}


		if (parsing_table.contains(arg))
		{
			auto fn = parsing_table.at(arg);
			fn(options);
		}
		else if (fs::is_directory(arg))
		{
			options->dir = arg;
		}
		else if (is_valid_regex(arg))
		{
			auto end = arg.find_last_of('/');
			
			//it's regular expression without path. something like l *.txt
			//so dir will be current path, because it's already set in options constructor
			if (end == std::string::npos)
			{
				options->regex_val = arg;
			}
			else
			{
				options->dir = arg.substr(0, end+1);
				options->regex_val = arg.substr(end + 1);
			}
			_SET_USE_REGEX(true)
		}
		else
		{
			fmt::print("Error:{} is unknown argument! Try using -h to see help!",arg);
			exit(-1);

		}
	}

	//show all
	if (args.find("-d") != std::string::npos &&
		args.find("-f") != std::string::npos)
	{
		_SET_SHOW_DIRS_ONLY(true)
		_SET_SHOW_FILES_ONLY(true)
	}

	//if program should print property info, then it should print it in correct, formatted way
	if (SHOW_PERMISSION ||
		SHOW_FILE_SIZE  ||
		SHOW_WRITE_TIME ||
		SHOW_CREAT_TIME)
	{
		_SET_FORMAT(true)
	}

	//if only sort flag is passed, then set default sorting order
	if (SORT && options->sorting_order.empty())
		options->sorting_order = "df";

	return options;
}
#endif 