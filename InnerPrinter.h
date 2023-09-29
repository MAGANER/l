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
#define TC (fmt::terminal_color)
#define FG(x) fmt::fg(TC x)
#define BG(x) fmt::bg(TC x)

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


#define unmutable const Options* const
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
	extern std::string cut_quotas(const std::string& str);
	extern void erase_sub(std::string& str, const std::string& sub);
	extern std::string mult_str(const std::string& str, size_t n);
	extern std::string prepare_entry_val(const fs::directory_entry& dir_entry, unmutable options);
	///


	//common functors
	extern void iterate_over_dir(unmutable options,
		const fn& iterate,
		const fn1& iterate_sorted);

	extern void iterate_over_dir_recursively(unmutable options,
		const fn& iterate,
		const fn1& iterate_sorted);
	///

	//print colored or pure
	extern void print_d(const std::string& str, unmutable options); //print directory
	extern void print_f(const std::string& str, unmutable options);//print file

	extern bool does_matches(const std::string& str, const std::string& regex);

	extern size_t compute_dir_elements_number(const std::string& path, bool rec);

	extern size_t get_max_dir_str_size(const std::string& dir, unmutable options);
	extern size_t get_max_dir_str_size_recursivly(const std::string& dir, unmutable options);
	extern size_t get_max_dir_file_size_str_size(const std::string& dir, unmutable options);
	extern size_t get_max_dir_file_size_str_size_recursivly(const std::string& dir, unmutable options);
	extern void show_permissions(const std::string& entry);
	extern void print_time(const std::string& time, unmutable options, const std::string& space);

	extern void printDirectoryTree(unmutable options, const fs::path& path, size_t level = 0);
};
#endif