#ifndef CONFIG_READER_H
#define CONFIG_READER_H
#include<fstream>
#include <stdlib.h>
#include"OptionParser.h"

#ifdef __linux__
#include<unistd.h>
#include<sys/types.h>
#include<pwd.h>
#endif

static inline std::string get_default_config()
{
	return "table_output_width=5\n" 
			"dir_color=94\n"
			"dir_bg_color=40\n" 
			"file_color=34\n"
			"file_bg_color=40\n";
}


static std::string get_user_home_dir()
{

#ifdef _WIN32
	return std::string(getenv("USERPROFILE"));
#endif

#ifdef __linux__
	char* dir = getenv("HOME");
	if(dir == NULL)
	{
		dir = getpwuid(getuid())->pw_dir;
	}
	return std::string(dir);
#endif
}
static void read_config_file(Options* options)
{
	//if there is config file, than create it
	//otherwise read and save options


	auto home_dir = get_user_home_dir();
	auto config = std::ifstream(home_dir + "/.config/.lconfig");
	if (!config)
	{
		std::cout << ".lconfig file will be created at " + home_dir+"/.config" << std::endl;
		auto write_config = std::ofstream(home_dir + "/.config/.lconfig");
		write_config << get_default_config();
	}
	else
	{
		//if config doesn't exist, then don't read
		//because default values are written in the config file already
		std::string option_line;
		while (std::getline(config, option_line))
		{
			auto sep = option_line.find("=");
			if (sep != std::string::npos)
			{
				auto left = option_line.substr(0, sep);
				auto right = option_line.substr(sep + 1);
				auto val = (short)atoi(right.c_str());

				if (val > 0)
				{
					if (left == "table_output_width")
					{
						options->table_output_width = val;
					}
					else if (left == "dir_color")
					{
						options->dir_color = val;
					}
					else if (left == "dir_bg_color")
					{
						options->dir_bg_color = val;
					}
					else if (left == "file_color")
					{
						options->file_color = val;
					}
					else if (left == "file_bg_color=")
					{
						options->file_bg_color = val;
					}
				}
			}
		}
	}
}



#endif
