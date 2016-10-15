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
		saveFile.close();
	}
	else
	{
		output = "File active or invalid path name.\n";
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
			output = "Directory " + std::to_string(i) + " in " + directory.getName() + " is non-existant.\n";
		else
		{
			saveToFile(*directory.getDirectory(i), saveFile);
		}
	}
	for (int i = 0; i < children[1]; i++)
	{
		File* file = directory.getFile(i);
		if (file == nullptr)
			output = "File " + std::to_string(i) + " in " + directory.getName() + " is non-existant.\n";
		else
		{
			saveFile << file->getName() << "\n" << file->getSize() << "\n" << file->getData() << "\n";
		}

	}
	output = "Save successful.\n";
	return output;
}

std::string FileSystem::restoreImage(const std::string & path)
{
	std::string output;
	char read = 'a';
	FILE* loadFile = NULL;
	errno_t error;
	if (error = fopen_s(&loadFile, path.c_str(), "r") == 0)
	{
		while (read != '\n')
		{
			fscanf(loadFile, "%c", &read);
		}
		output = loadFromFile(_root, loadFile);
		fclose(loadFile);
	}
	else
	{
		output = "Invalid path name.\n";
	}

	return output;
}

std::string FileSystem::loadFromFile(Directory & directory, FILE* loadFile)
{
	std::string name, data, children[2], size;
	char read;

	//reads number of directory children, 0: directories, 1: files
	for (int i = 0; i < 2; i++)
	{
		while (1)
		{
			fscanf(loadFile, "%c", &read);
			if (read != '\n')
			{
				children[i]+= read;
			}
			else
				break;
		}
	}

	//reads and stores the subderectories for the current directory
	for (int i = 0; i < std::stoi(children[0]); i++) //std::stoi converts string to int
	{
		//reads and stores directory name
		while (1)
		{
			fscanf(loadFile, "%c", &read);
			if (read != '\n')
				name += read;
			else
				break;
		}
		directory.addDirectory(name);
		loadFromFile(*directory.getDirectory(i), loadFile);
		name = "";
	}

	//reads and stores files
	for (int i = 0; i < std::stoi(children[1]); i++) //std::stoi converts string to int
	{
		//reads and stores file name
		while (1)
		{
			fscanf(loadFile, "%c", &read);
			if (read != '\n')
				name += read;
			else
				break;
		}

		//reads and stores file size
		while (1)
		{
			fscanf(loadFile, "%c", &read);
			if (read != '\n')
			{
				size += read;
			}
			else
				break;
		}

		//reads and stores file data
		for (int j = 0; j < std::stoi(size); j++)
		{
			fscanf(loadFile, "%c", &read);
			data += read;
		}
		fscanf(loadFile, "%c", &read); //reads the line feed after the data
		writeToFile(&directory, name, data);
		name = "";
	}

	return "Load successful.\n";
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
		return "Creation successful.\n";
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

std::string FileSystem::renameFile(const std::string & prevName, const std::string & newName)
{
	return _currentDir->renameFile(prevName, newName);
}

std::string FileSystem::removeFile(const std::string & name)
{
	return _currentDir->removeFile(name);
}

std::string FileSystem::copyFile(const std::string & name, const std::string & path)
{
	std::string output, data;
	if (_currentDir->getFileData(name, data))
	{
		std::string p = path;
		std::string newFileName = extractNameFromPath(p);
		Directory* dir = startPathProcessing(p);
		if (dir != nullptr)
		{
			writeToFile(dir, newFileName, data);
			output = "File copy successful.\n";
		}
		else
			output = "Invalid path name.\n";
	}
	else
		output = "Invalid file name.\n";

	return output;
}
