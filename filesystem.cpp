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
			saveFile << file->getName() << "\n" << file->getData() << "\n";
		}

	}
	output = "Save successful\n";
	return output;
}

std::string FileSystem::restoreImage(const std::string & path)
{
	std::string output;
	std::ifstream loadFile;
	loadFile.open(path);
	if (loadFile.is_open)
	{
		output = loadFromFile(_root, loadFile);
	}
	else
	{
		output = "Invalid path name\n";
	}

	return output;
}

std::string FileSystem::loadFromFile(Directory & directory, std::ifstream & loadFile)
{
	std::string output, name;
	int children[2] = { 0,0 };
	loadFile >> name;
	loadFile >> children[0];
	loadFile >> children[1];
	for (int i = 0; i < children[0]; i++)
	{
		loadFile >> name;
		directory.addDirectory(name);
		loadFromFile(*directory.getDirectory(i), loadFile);
	}
	for (int i = 0; i < children[1]; i++)
	{
		loadFile >> name;
		directory.addFile(name, );
	}

	return output;
}

//Lists all directories and files in the current directory
std::string FileSystem::ls(const std::string & path)
{
	Directory* dir = startPathProcessing(path);
	if (dir != nullptr)
		return dir->getInfoString();
	else
		return "Invalid path.\n";
}

//Write data to file
std::string FileSystem::writeToFile(const std::string & path, const std::string & data)
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
		_currentDir->addFile("NAME", i + left, blocks);
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
	int lastSlash = path.find_last_of("/");
	std::string name = "";
	if (lastSlash == -1)  //Found no '/'
	{
		name = path;
		return _root.addDirectory(name);
	}
	else
		name = path.substr(lastSlash + 1);
	Directory* dir = startPathProcessing(path.substr(0, lastSlash + 1));
	if (dir != nullptr)
		return dir->addDirectory(name);
	else
		return "Invalid path.\n";
}

//Sets new working directory
std::string FileSystem::goToFolder(const std::string & path)
{
	Directory* dir = startPathProcessing(path);
	if (dir != nullptr)
	{
		_currentDir = dir;
		return "";
	}
	else
		return "Invalid path.\n";
}
