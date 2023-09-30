#ifndef DIR_EXIST_H
#define DIR_EXIST_H
#include <sys/types.h>
#include <sys/stat.h>

static bool does_dir_exist(const char* const dir)
{
	struct stat info;
	if (stat(dir, &info) != 0)
		return false;
	else if (info.st_mode & S_IFDIR)  // S_ISDIR() doesn't exist on my windows 
		return true;
	else
		return false;

}
#define dexist(x) does_dir_exist(x) //check is provided path to directory real

#endif