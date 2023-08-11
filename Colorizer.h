#ifndef COLORIZER_H
#define COLORIZER_H
#include<string>

#define BLACK_FG		   30
#define RED_FG             31  
#define GREEN_FG           32 
#define YELLOW_FG          33  
#define BLUE_FG            34
#define MAGENTA_FG         35
#define CYAN_FG            36
#define WHITE_FG           37
#define BRIGHT_BLACK_FG    90
#define BRIGHT_RED_FG      91
#define BRIGHT_GREEN_FG    92
#define BRIGHT_YELLOW_FG   93
#define BRIGHT_BLUE_FG     94
#define BRIGHT_MAGENTA_FG  95
#define BRIGHT_CYAN_FG     96
#define BRIGHT_WHITE_FG    97

#define BLACK_BG		   40
#define RED_BG             41  
#define GREEN_BG           42 
#define YELLOW_BG          43  
#define BLUE_BG            44
#define MAGENTA_BG         45
#define CYAN_BG            46
#define WHITE_BG           47
#define BRIGHT_BLACK_BG    100
#define BRIGHT_RED_BG      101
#define BRIGHT_GREEN_BG    102
#define BRIGHT_YELLOW_BG   103
#define BRIGHT_BLUE_BG     104
#define BRIGHT_MAGENTA_BG  105
#define BRIGHT_CYAN_BG     106
#define BRIGHT_WHITE_BG    107

static inline std::string colorize(const std::string& str,unsigned short foreground_color)
{
	return "\x1B[" + std::to_string(foreground_color)+"m" + str + "\033[0m";
}
static inline std::string colorize(const std::string& str, unsigned short foreground_color, unsigned short background_color)
{
	return "\033[" + std::to_string(foreground_color) + ";" +
		std::to_string(background_color) + "m" + str + "\033[0m";
}

#endif