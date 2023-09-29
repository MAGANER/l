#ifndef INNER_PRINTER_H
#define INNER_PRINTER_H

#include"OptionParser.h"
#include"FileTimeChecker.h"
#include<list>
#include<functional>
#include<sstream>
#include<iostream> //for HumanReadable. (should be changed and removed in future)
#include<cmath>
#include<regex>


#include"fmt/core.h"
#include"fmt/color.h"
#define FG(x) fmt::fg((fmt::terminal_color) x)
#define BG(x) fmt::bg((fmt::terminal_color) x)

#define unmutable const Options* const

//https://en.cppreference.com/w/cpp/filesystem/file_size
struct HumanReadable {
	std::uintmax_t size{};
private: friend
	std::ostream& operator<<(std::ostream& os, HumanReadable hr)
{
	int i{};
	double mantissa = hr.size;
	for (; mantissa >= 1024.; mantissa /= 1024., ++i) {}
	mantissa = std::ceil(mantissa * 10.) / 10.;

	auto size_type = "BKMGTPE"[i];
	auto additional_data = size_type != 'B' ? "b" : "";
	os << mantissa << size_type << additional_data;
	return os;
}

};

namespace InnerPrinter
{
	//this function type is dedicated for regular iterating without sorting
	typedef std::function<void(const fs::directory_entry& dir_entry,
		std::list<fs::directory_entry>& dirs,
		std::list<fs::directory_entry>& files)> fn;


	//this one dedicated for iteration over sorted data
	typedef std::function<void(std::list<fs::directory_entry>& dirs,
		std::list<fs::directory_entry>& files)> fn1;


	//block of special helper functions
	extern inline std::string cut_quotas(const std::string& str);
	extern void erase_sub(std::string& str, const std::string& sub);
	extern std::string prepare_entry_val(const fs::directory_entry& dir_entry, unmutable options);
	///


	//common functors
	//these function are base for all 2 functions(except tree output) available in Printer.h
	extern void iterate_over_dir(unmutable options,
		const fn& iterate,
		const fn1& iterate_sorted);

	extern void iterate_over_dir_recursively(unmutable options,
		const fn& iterate,
		const fn1& iterate_sorted);
	///
	extern void printDirectoryTree(unmutable options, const fs::path& path, size_t level = 0);
	///

	//print colored or pure
	extern void print_d(const std::string& str, unmutable options); //print directory
	extern void print_f(const std::string& str, unmutable options);//print file
	///


	//these functions are used to print element's properties
	extern size_t compute_dir_elements_number(const std::string& path, bool rec);
	extern void show_permissions(const std::string& entry);
	extern void print_time(const std::string& time, unmutable options, const std::string& space);
	///


	//check does str matches with regex
	extern bool does_matches(const std::string& str, const std::string& regex);
	///

	//these function computes max size of directory's string view
	extern size_t max_dir_size(const std::string& dir, unmutable options);

	//recursive version of `max_dir_size_rec`
	extern size_t max_dir_size_rec(const std::string& dir, unmutable options);

	//these function finds the size of the longest file name in directory
	extern size_t max_file_in_dir(const std::string& dir, unmutable options);

	//recursive version of `max_file_in_dir`
	extern size_t max_file_in_dir_rec(const std::string& dir, unmutable options);
	///
};
#endif