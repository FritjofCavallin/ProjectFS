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

	std::string extractNameFromPath(std::string & path);
	Directory* startPathProcessing(const std::string & path);

public:
    FileSystem();
    ~FileSystem();
	void format();
	std::string createImage(const std::string & path);
	std::string saveToFile(Directory & directory, std::ofstream & saveFile);
	std::string restoreImage(const std::string & path);
	std::string loadFromFile(Directory & directory, FILE* loadFile);
	std::string createFile(const std::string & path, const std::string & data, const std::string & accessRights);
	std::string getFileData(const std::string & path);
	std::string ls(const std::string & path);
	std::string writeToFile(Directory* dir, const std::string & name, const std::string & data, const unsigned int accessRights);
	std::string makeDir(const std::string & path);
	std::string goToFolder(const std::string & path, std::string & fullPath);
	std::string getFullPath();
	std::string renameFile(const std::string & prevName, const std::string & newName);
	std::string copyFile(const std::string & name, const std::string & path);
	std::string appendFile(const std::string & name, const std::string & path);
	std::string removeFile(const std::string & path);
	std::string accessRights(const std::string & accessRights, const std::string & path);

    /* These commands needs to implemented
     *
     * However, you are free to change the parameterlist and specify your own returntype for each function below.
     */

    /* This function creates a file in the filesystem */
    // createFile(...)

    /* Creates a folder in the filesystem */
    // createFolder(...);

    /* Removes a file in the filesystem */
    // removeFile(...);

    /* Removes a folder in the filesystem */
    // removeFolder(...);

    /* Function will move the current location to a specified location in the filesystem */
    // goToFolder(...);

    /* This function will get all the files and folders in the specified folder */
    // listDir(...);

    /* Add your own member-functions if needed */
};

#endif // FILESYSTEM_H
