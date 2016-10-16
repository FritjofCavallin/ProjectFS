#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <list>

#include "memblockdevice.h"
#include "Directory.h"
#include "stdio.h"

class FileSystem
{
private:
    MemBlockDevice			_memBlockDevice;
	Directory*				_root;
	Directory*				_currentDir;
	std::list<int>			_freeBlocks;

	//Extracts the name from a path, aka the last part
	std::string extractNameFromPath(std::string & path);
	//Small function that starts the actual path processing
	Directory* startPathProcessing(const std::string & path);
	//The function that actually writes data to blocks
	std::string writeToFile(Directory* dir, const std::string & name,
		const std::string & data, const unsigned int accessRights);
	//recursive function used by createImage(...)
	std::string saveToFile(Directory & directory, std::ofstream & saveFile);
	//recursive function used by restoreImage(...)
	std::string loadFromFile(Directory & directory, FILE* loadFile);

public:
    FileSystem();
    ~FileSystem();
	//format
	void format();
	//ls
	std::string getDirectoryInfo(const std::string & path);
	//create
	std::string createFile(const std::string & path, const std::string & data,
		const std::string & accessRights);
	//cat
	std::string getFileData(const std::string & path);
	//createImage
	std::string createImage(const std::string & path);
	//restoreImage
	std::string restoreImage(const std::string & path);
	//rm
	std::string removeFile(const std::string & path);
	//cp
	std::string copyFile(const std::string & path1, const std::string & path2);
	//append
	std::string appendFile(const std::string & name, const std::string & path);
	//mv
	std::string renameFile(const std::string & path1, const std::string & path2);
	//mkdir
	std::string makeDir(const std::string & path);
	//cd
	std::string goToFolder(const std::string & path, std::string & fullPath);
	//pwd
	std::string getFullPath();
	//cdmod
	std::string accessRights(const std::string & accessRights, const std::string & path);
};

#endif // FILESYSTEM_H
