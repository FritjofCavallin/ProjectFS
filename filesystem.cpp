#include "filesystem.h"
#include <string>

//Default constructor
FileSystem::FileSystem()
{
	_memBlockDevice = MemBlockDevice();
	_root = new Directory("root", nullptr);
	_currentDir = _root;

	for (unsigned int i = 0; i < 250; i++)
	{
		_freeBlocks.push_back(i);
	}
}

//Destructor
FileSystem::~FileSystem()
{
	delete _root;
}

//Resets the whole system
void FileSystem::format()
{
	delete _root;
	_memBlockDevice = MemBlockDevice();
	_root = new Directory("root", nullptr);
	_currentDir = _root;

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
		p = p.substr(0, p.length() - 1);

	if (p[0] == '.')  //Relative path
		return _currentDir->processPath(p);
	else  //Absolute path
		return _root->processPath(p);
}

//Initializes storing virtual disk on real storage
std::string FileSystem::createImage(const std::string & path) //real path
{
	std::string output;
	std::ofstream saveFile;
	saveFile.open(path);
	if (saveFile.is_open())
	{
		output = saveToFile(*_root, saveFile);
		saveFile.close();
	}
	else
	{
		output = "Real file active or invalid path name.\n";
	}

	return output;
}

//Stores virtual disk on hard drive
std::string FileSystem::saveToFile(Directory & directory, std::ofstream & saveFile)
{
	std::string output;
	int children[2] = { 0, 0 };
	directory.getChildren(children);
	saveFile << directory.getName() << "\n" << std::to_string(children[0]) << "\n" << std::to_string(children[1]) << "\n";
	for (int i = 0; i < children[0]; i++)
	{
		Directory* dir = directory.getDirectory(i);
		if (dir != nullptr) //Checks whether the path exists
			saveToFile(*directory.getDirectory(i), saveFile);
		else
		
			output = "Directory " + std::to_string(i) + " in " + directory.getName() + " is non-existant.\n";
	}
	for (int i = 0; i < children[1]; i++)
	{
		File* file = directory.getFile(i);
		if (file != nullptr) //Checks whether the path exists
			saveFile << file->getName() << "\n" << file->getSize() << "\n" << file->getAccessRights() << "\n" << file->getData() << "\n";
		else
			output = "File " + std::to_string(i) + " in " + directory.getName() + " is non-existant.\n";


	}
	output = "Save successful.\n";
	return output;
}

//Initializes restoring virtual disk from real storage
std::string FileSystem::restoreImage(const std::string & path)
{
	std::string output;
	char read = 'a';
	FILE* loadFile = NULL;
	errno_t error;
	if (error = fopen_s(&loadFile, path.c_str(), "r") == 0) //Checks whether the (real) file exists
	{
		while (read != '\n')
		{
			fscanf(loadFile, "%c", &read);
		}
		output = loadFromFile(*_root, loadFile);
		fclose(loadFile);
	}
	else
	{
		output = "Invalid path name.\n";
	}

	return output;
}

//Loads virtual disk from real storage
std::string FileSystem::loadFromFile(Directory & directory, FILE* loadFile)
{
	std::string name, data, children[2], size, accessRights;
	char read;

	//reads directory's number of children, 0: directories, 1: files
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
		while (1)
		{
			fscanf(loadFile, "%c", &read);
			if (read != '\n')
			{
				accessRights += read;
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
		writeToFile(&directory, name, data, std::stoi(accessRights));
		name = "";
	}

	return "Load successful.\n";
}

//Create a new file
std::string FileSystem::createFile(const std::string & path, const std::string & data, const std::string & accessRights)
{
	std::string p = path;
	std::string name = extractNameFromPath(p);
	Directory* dir = startPathProcessing(p);
	if (dir != nullptr) //Checks whether the path exists
	{
		writeToFile(dir, name, data, std::stoi(accessRights));
		return "Creation successful.\n";
	}
	else
		return "Invalid path.\n";
}

//Returns string containing all data in file
std::string FileSystem::getFileData(const std::string & path)
{
	std::string p = path;
	std::string name = extractNameFromPath(p);
	Directory* dir = startPathProcessing(p);
	if (dir != nullptr) //Checks whether the path exists
	{
		File* file = dir->getFile(name);
		if (file != nullptr) //Checks whether the file exists
		{
			if (file->getAccessRights() < 2) //Checks access rights for reading file
			{
				std::string data = "";
				if (dir->getFileData(name, data)) //Checks whether the file exists while getting data if it does
					return "Data in file \"" + name + "\":\n" + data + "\n";
				else
					return "Invalid file name.\n";
			}
			else
				return "Access violation reading file \'" + name + "\'.\n";
		}
		else
			return "Invalid file name.\n";
	}
	else
		return "Invalid path.\n";
}

//Lists all directories and files in the current directory
std::string FileSystem::ls(const std::string & path)
{
	if (path != "") //Checks whether the path refers to current directory
	{
		Directory* dir = startPathProcessing(path);
		if (dir != nullptr) //Checks whether the path exists
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
std::string FileSystem::writeToFile(Directory* dir, const std::string & name, const std::string & data
	, const unsigned int accessRights)
{
	if ((data.length() + 511) / 512 < _freeBlocks.size())  //Check if there is enough space
	{
		int index = dir->newFileIndex(name);
		if (index == -1)
			return "Name already used";
		else
		{
			std::vector<Block*> blocks;
			std::vector<int> usedIndexes;
			unsigned int i = 0;
			//Using "full" blocks
			while (i + 512 < data.length())
			{
				_memBlockDevice.writeBlock(_freeBlocks.front(), data.substr(i, 512));
				blocks.push_back(&_memBlockDevice[_freeBlocks.front()]);
				usedIndexes.push_back(_freeBlocks.front());
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
			usedIndexes.push_back(_freeBlocks.front());
			_freeBlocks.pop_front();
			dir->addFile(index, name, accessRights, i + left, blocks, usedIndexes);
			return "File successfully written.\n";
		}
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
	if (dir != nullptr) //Checks whether the path exists
		return dir->addDirectory(name);
	else
		return "Invalid path.\n";
}

//Sets new working directory
std::string FileSystem::goToFolder(const std::string & path, std::string & fullPath)
{
	Directory* dir = startPathProcessing(path);
	if (dir != nullptr) //Checks whether the path exists
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

//Rename and/or move a file. mv command in shell
std::string FileSystem::renameFile(const std::string & prevName, const std::string & newName)
{
	std::string output, _prevName, _newName, p;
	p = prevName;
	_prevName = extractNameFromPath(p);
	Directory* prevDir = startPathProcessing(p), *newDir;
	File* file = prevDir->getFile(_prevName);
	if (file->getAccessRights() == 0 || file->getAccessRights() == 2) //Checks access rights for reading in file
	{
		p = newName;
		_newName = extractNameFromPath(p);
		newDir = startPathProcessing(p);
		if (prevDir == newDir) //Checks whether the file is to be renamed only
			output = prevDir->renameFile(_prevName, _newName);
		else
		{
			copyFile(prevName, newName); 
			std::vector<int> usedIndexes;
			prevDir->removeFile(_prevName, usedIndexes);
			_freeBlocks.insert(std::end(_freeBlocks), std::begin(usedIndexes), std::end(usedIndexes));
			output = "File successfully moved.\n";
		}
	}
	return output;
}

//Copies a file
std::string FileSystem::copyFile(const std::string & path1, const std::string & path2)
{
	std::string output, data;
	File* file;
	std::string p = path1;
	std::string fileName = extractNameFromPath(p);
	Directory* dir = startPathProcessing(p);
	if ((file = dir->getFile(fileName)) != nullptr) //Checks whether the file exists
	{
		dir->getFileData(fileName, data);
		p = path2;
		fileName = extractNameFromPath(p);
		if (p != "") //Checks whether the file is to be copied to the same path
		{
			dir = startPathProcessing(p);
			if (dir != nullptr) //Checks whether the path to the file exists
			{
				writeToFile(dir, fileName, data, file->getAccessRights());
				output = "File copy successful.\n";
			}
			else
				output = "Invalid path name.\n";
		}
		else
		{
			writeToFile(_currentDir, fileName, data, file->getAccessRights());
			output = "File copy successful.\n";
		}
	}
	else
		output = "Invalid file name.\n";

	return output;
}

//Adds the content of one file to another
std::string FileSystem::appendFile(const std::string & path1, const std::string & path2)
{
	std::string output, data, data1, data2, fileName, appendFileName;
	std::string p = path1;
	std::string fileName = extractNameFromPath(p);
	Directory* dir = startPathProcessing(p);
	if (dir != nullptr) //Checks whether the path to file 1 exists
	{
		File* file = _currentDir->getFile(p);
		if (file != nullptr) //Checks whether file 1 exists
		{
			if (file->getAccessRights() < 2) //Checks access rights for reading file 1
			{
				dir->getFileData(p, data1);
				p = path2;
				appendFileName = extractNameFromPath(p);
				dir = startPathProcessing(p);
				if ((file = dir->getFile(appendFileName)) != nullptr) //Checks whether file 2 exists
				{
					if (file->getAccessRights() == 0 || file->getAccessRights() == 2) //Checks access rights for writing in file 2
					{
						dir->getFileData(appendFileName, data2);
						std::vector<int> usedIndexes;
						dir->removeFile(appendFileName, usedIndexes);
						_freeBlocks.insert(std::end(_freeBlocks), std::begin(usedIndexes), std::end(usedIndexes));
						data = data1 + data2;
						writeToFile(dir, appendFileName, data, file->getAccessRights());
						output = "File successfully appended.\n";
					}
					else
						output = "Access violation writing in '" + appendFileName + "'.\n";
				}
				else
					output = "Invalid file 2 name or path.\n";
			}
			else
				output = "Access violation reading '" + fileName + "'.\n";
		}
		else
			output = "Invalid file 1 name.\n";
	}
	else
		output = "Invalid path name.\n";

	return output;
}

//Removes a file from the system
std::string FileSystem::removeFile(const std::string & path)
{
	std::string p = path;
	std::string name = extractNameFromPath(p);
	Directory* dir = startPathProcessing(p);
	if (dir != nullptr) //Checks whether the path to the file exists
	{
		std::vector<int> usedIndexes;
		if (dir->removeFile(name, usedIndexes)) //Checks whether the file exists
		{
			_freeBlocks.insert(std::end(_freeBlocks), std::begin(usedIndexes), std::end(usedIndexes));
			return "Removal successful.\n";
		}
		else
			return "Invalid name.\n";
	}
	else
		return "Invalid path.\n";
}

//Changes acces rights for a file
std::string FileSystem::accessRights(const std::string & accessRights, const std::string & path)
{
	std::string output, name, p;
	p = path;
	name = extractNameFromPath(p);
	Directory* dir = startPathProcessing(p);
	if (dir != nullptr) //Checks whether the path to the file exists
	{
		File* file = dir->getFile(name);
		if (file != nullptr) //Checks whether the file exists
		{
			file->setAccessRights(std::stoi(accessRights));
		}
		else
			output = "Invalid file name.\n";
	}
	else
		output = "Invalid path name.\n";
	return output;
}
