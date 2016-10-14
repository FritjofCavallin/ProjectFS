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

Directory * FileSystem::startPathProcessing(const std::string & path)
{
	if (path.substr(0, 3) == "../")
	{
		std::string p = path.substr(3);
		Directory* dir = nullptr;
		while (p.substr(0, 3) == "../")
		{
			dir = _currentDir->getParent();
			p = p.substr(3);
			if (dir == nullptr)
				break;
		}
		if (dir != nullptr)
			return dir->processPath(p);
		else
			return nullptr;
	}
	else if (path.substr(0, 2) == "./")
		return _currentDir->processPath(path.substr(2));
	else
		return _root.processPath(path);
}

//Resets the whole system
void FileSystem::format()
{
	_memBlockDevice = MemBlockDevice();
	_root = Directory("root", nullptr);
	_currentDir = &_root;

	for (unsigned int i = 0; i < 250; i++)
	{
		_freeBlocks.push_back(i);
	}
}

//Lists all directories and files in the current directory
std::string FileSystem::ls() const
{
	return _currentDir->getInfoString();
}

//Write data to file
std::string FileSystem::writeToFile(const std::string & path, const std::string & data)
{
	if ((data.length() + 511) / 512 < _freeBlocks.size())  //Check if there is enough space
	{
		std::vector<Block*> blocks;
		int i = 0;
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
	if (path == ".." && _currentDir != &_root)
	{
		_currentDir = _currentDir->getParent();
		return "";
	}
	Directory* dir = startPathProcessing(path);
	if (dir != nullptr)
	{
		_currentDir = dir;
		return "";
	}
	else
		return "Invalid path.\n";
}
