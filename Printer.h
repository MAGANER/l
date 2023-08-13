#ifndef PRINTER_H
#define PRINTER_H
#include"OptionParser.h"
#include"Colorizer.h"
#include<list>
#include<functional>

//namespace contains functions used by Printer's namespace ones
//they aren't dedicated to be used out of Printer namespace
namespace PrinterInnerFn
{
	extern inline std::string cut_quotas(const std::string& str);
	extern void erase_sub(std::string& str, const std::string& sub);
	static inline std::string mult_str(const std::string& str,size_t n)
	{
		std::string ns;
		for (size_t i = 0; i < n; i++)
			ns += str;
		return ns;
	}
	static inline  std::string prepare_entry_val(const fs::directory_entry& dir_entry, const Options const* options)
	{
		auto entry_val = dir_entry.path().filename().string();
		entry_val = options->recursive ? dir_entry.path().parent_path().string() + "/" + entry_val : entry_val;
		if (options->recursive)
		{
			erase_sub(entry_val, options->dir);
			entry_val = entry_val.substr(1); //erase weird / or \ at the beginning of str
		}

		return entry_val;
	};


	//this function type is dedicated for regular iterating without sorting
	typedef std::function<void(const fs::directory_entry& dir_entry,
							   std::list<std::string>& dirs,
							   std::list<std::string>& files)> fn;

	//this one dedicated for iteration over sorted data
	typedef std::function<void(std::list<std::string>& dirs,
								std::list<std::string>& files)> fn1;

	extern void iterate_over_dir(const Options const* options,
				const fn& iterate,
				const fn1& iterate_sorted);

	extern void iterate_over_dir_recursively(const Options const* options,
		const fn& iterate,
		const fn1& iterate_sorted);


	//https://en.cppreference.com/w/cpp/filesystem/file_size
	struct HumanReadable {
		std::uintmax_t size{};
	private: friend
		std::ostream& operator<<(std::ostream& os, HumanReadable hr) {
		int i{};
		double mantissa = hr.size;
		for (; mantissa >= 1024.; mantissa /= 1024., ++i) {}
		mantissa = std::ceil(mantissa * 10.) / 10.;

		auto size_type = "BKMGTPE"[i];
		auto additional_data = size_type != 'B' ? "b" : "";
		os << mantissa << size_type<<additional_data;
		return os;
	}
	};

	extern size_t get_max_dir_str_size(const std::string& dir,const Options const* options);
	extern size_t get_max_dir_str_size_recursivly(const std::string& dir,const Options const* options);
}
namespace Printer
{
	namespace fs = std::filesystem;

	extern void print_as_table(const Options const* options);
	extern void print_as_list(const Options const* options);
	extern void print_as_tree(const Options const* options);
};

#endif