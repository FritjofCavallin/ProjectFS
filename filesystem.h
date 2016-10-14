#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <list>

#include "memblockdevice.h"
#include "Directory.h"

class FileSystem
{
private:
    MemBlockDevice			_memBlockDevice;
	Directory				_root;
	Directory*				_currentDir;
	std::list<int>			_freeBlocks;

	Directory* startPathProcessing(const std::string & path);

public:
    FileSystem();
    ~FileSystem();
	void format();
	std::string createImage(const std::string & path);
	std::string saveToFile(Directory & directory, std::ofstream & saveFile);
	std::string restoreImage(const std::string & path);
	std::string readFromFile(Directory & directory, std::ifstream & readFile);
	std::string ls() const;
	std::string writeToFile(const std::string & path, const std::string & data);
	std::string makeDir(const std::string & path);
	std::string goToFolder(const std::string & path);

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
