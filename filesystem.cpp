#include "filesystem.h"
#include <string>

//Default constructor
FileSystem::FileSystem()
{
	format();
}

//Destructor
FileSystem::~FileSystem()
{
	//Nothing
}

//Resets the whole system
void FileSystem::format()
{
	_memBlockDevice = MemBlockDevice();
	_root = Directory("root", nullptr);
	_currentDir = &_root;

	_freeBlocks.clear();
	for (unsigned int i = 0; i < 250; i++)
	{
		_freeBlocks.push_back(i);
	}
}

//Extracts and returns the last part of a path, aka the name, and removes it from the referenced path
std::string FileSystem::extractNameFromPath(std::string & path)
{
	int lastSlash = path.find_last_of("/");
	std::string name = "";
	if (lastSlash == -1)  //Found no '/'
	{
		name = path;
		path = "";
	}
	else
	{
		name = path.substr(lastSlash + 1);
		path = path.substr(0, lastSlash + 1);
	}
	return name;
}

//Basically preprocessing for processing paths
Directory * FileSystem::startPathProcessing(const std::string & path)
{
	std::string p = path;
	//Remove '/' at the end of path for consistency
	if (p.length() > 3 && p[p.length() - 1] == '/')
		p = p.substr(0, p.length() - 2);

	if (p[0] == '.')  //Relative path
		return _currentDir->processPath(p);
	else  //Absolute path
		return _root.processPath(p);
}

std::string FileSystem::createImage(const std::string & path) //real path
{
	std::string output;
	std::ofstream saveFile;
	saveFile.open(path);
	if (saveFile.is_open())
	{
		output = saveToFile(_root, saveFile);
	}
	else
	{
		output = "Invalid path name\n";
	}
	
	return output;
}

std::string FileSystem::saveToFile(Directory & directory, std::ofstream & saveFile)
{
	std::string output;
	int children[2] = { 0, 0 };
	directory.getChildren(children);
	saveFile << directory.getName() << "\n" << std::to_string(children[0]) << "\n" << std::to_string(children[1]) << "\n";
	for (int i = 0; i < children[0]; i++)
	{
		Directory* dir = directory.getDirectory(i);
		if (dir == nullptr)
			output = "Directory " + std::to_string(i) + " in " + directory.getName() + " is non-existant\n";
		else
		{
			saveToFile(*directory.getDirectory(i), saveFile);
		}
	}
	for (int i = 0; i > children[1]; i++)
	{
		File* file = directory.getFile(i);
		if (file == nullptr)
			output = "File " + std::to_string(i) + " in " + directory.getName() + " is non-existant\n";
		else
		{
			saveFile << file->getFileInfo().substr(0, 15) << "\n" << file->getData() << "\n";
		}

	}
	output = "Save successful\n";
	return output;
}

//Create a new file
std::string FileSystem::createFile(const std::string & path, const std::string & data)
{
	std::string p = path;
	std::string name = extractNameFromPath(p);
	Directory* dir = startPathProcessing(p);
	if (dir != nullptr)
	{
		writeToFile(dir, name, data);
		return "";
	}
	else
		return "Invalid path.\n";
}

std::string FileSystem::getFileData(const std::string & path)
{
	std::string p = path;
	std::string name = extractNameFromPath(p);
	Directory* dir = startPathProcessing(p);
	if (dir != nullptr)
	{
		std::string data = "";
		if (dir->getFileData(name, data))
			return "Data in file \"" + name + "\":\n" + data + "\n";
		else
			return "Invalid file name.\n";
	}
	else
		return "Invalid path.\n";
}

//Lists all directories and files in the current directory
std::string FileSystem::ls(const std::string & path)
{
	if (path != "")
	{
		Directory* dir = startPathProcessing(path);
		if (dir != nullptr)
			return dir->getInfoString();
		else
			return "Invalid path.\n";
	}
	else
	{
		return _currentDir->getInfoString();
	}
}

//Write data to file
std::string FileSystem::writeToFile(Directory* dir, const std::string & name, const std::string & data)
{
	if ((data.length() + 511) / 512 < _freeBlocks.size())  //Check if there is enough space
	{
		std::vector<Block*> blocks;
		unsigned int i = 0;
		//Using "full" blocks
		while (i + 512 < data.length())
		{
			_memBlockDevice.writeBlock(_freeBlocks.front(), data.substr(i, 512));
			blocks.push_back(&_memBlockDevice[_freeBlocks.front()]);
			_freeBlocks.pop_front();
			i += 512;
		}
		//The last data
		std::string last = ".";
		last.replace(0, 1, 512, '*');
		int left = data.length() - i;
		last.replace(0, left, data.substr(i, left));
		_memBlockDevice.writeBlock(_freeBlocks.front(), last);
		blocks.push_back(&_memBlockDevice[_freeBlocks.front()]);
		_freeBlocks.pop_front();
		dir->addFile(name, i + left, blocks);
		return "";
	}
	else
	{
		return "Not enough storage left.\n";
	}
}

//Create a directory
std::string FileSystem::makeDir(const std::string & path)
{
	std::string p = path;
	std::string name = extractNameFromPath(p);
	Directory* dir = startPathProcessing(p);
	if (dir != nullptr)
		return dir->addDirectory(name);
	else
		return "Invalid path.\n";
}

//Sets new working directory
std::string FileSystem::goToFolder(const std::string & path, std::string & fullPath)
{
	Directory* dir = startPathProcessing(path);
	if (dir != nullptr)
	{
		_currentDir = dir;
		fullPath = getFullPath();
		return "";
	}
	else
		return "Invalid path.\n";
}

//Get the full file path from root to current working directory
std::string FileSystem::getFullPath()
{
	return "/" + _currentDir->getPath() + "/";
}
