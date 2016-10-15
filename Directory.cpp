#include "Directory.h"

//Default constructor
Directory::Directory()
{
	_name = "ERROR!";
	_parent = nullptr;
}

//Constructor
Directory::Directory(const std::string & name, Directory * parent)
{
	_name = name;
	_parent = parent;
}

//Destructor
Directory::~Directory()
{
	for (unsigned int i = 0; i < _directories.size(); i++)
	{
		delete _directories[i];
	}
	for (unsigned int i = 0; i < _files.size(); i++)
	{
		delete _files[i];
	}
}

//Get directory name
std::string Directory::getName()
{
	return _name;
}

//
Directory * Directory::getParent() const
{
	return _parent;
}

//Get info about only this directory
std::string Directory::toString() const
{
	std::string tempName = _name;
	if (_name.length() > 16)
		tempName = _name.substr(0, 16);
	std::string spaces = "    ";
	for (unsigned int i = 0; i < 16 - tempName.length(); i++)
	{
		spaces += " ";
	}
	return "D:  " + tempName + spaces + std::to_string(_directories.size()) + "\t"
		+ std::to_string(_files.size()) + "\n";
}

//Get info about what is inside this directory
std::string Directory::getInfoString() const
{
	std::string output = _name + "\nDirectories: " + std::to_string(_directories.size());
	output += "  \tFiles: " + std::to_string(_files.size()) + "\n";
	for (unsigned int i = 0; i < _directories.size(); i++)
	{
		output.append(_directories[i]->toString());
	}
	for (unsigned int i = 0; i < _files.size(); i++)
	{
		output.append(_files[i]->getFileInfo());
	}
	return output + "\n";
}

std::string Directory::getPath() const
{
	if (_parent == nullptr)
		return _name;
	else
		return _parent->getPath() + "/" + _name;
}

Directory * Directory::getDirectory(const unsigned int & index)
{
	if (index > _directories.size() - 1)
		return nullptr;
	else
		return _directories[index];
}

File* Directory::getFile(const unsigned int & index)
{
	if (index > _files.size() - 1)
		return nullptr;
	else
		return _files[index];
}

File * Directory::getFile(const std::string & name)
{
	File* output = nullptr;
	for (int i = 0; i < _files.size(); i++)
		if (_files[i]->getName() == name)
			output = _files[i];
	return output;
}

bool Directory::getFileData(const std::string & name, std::string & data) const
{
	int index = -1;
	for (unsigned int i = 0; i < _files.size() && index == -1; i++)
	{
		if (name == _files[i]->getName())
			index = i;
	}
	if (index != -1)
	{
		data = _files[index]->getData();
		return true;
	}
	else
		return false;
}

void Directory::getChildren(int* children)
{
	children[0] = _directories.size();
	children[1] = _files.size();
}

//Calculates the index of a new file with the name 'name'. Returns -1 if name is already used
int Directory::newFileIndex(const std::string & name)
{
	//Special case if it´s the first one created
	if (_files.size() == 0)
	{
		return 0;
	}
	//Normal case
	else
	{
		unsigned int i = 0;
		//Loop while 'name' comes (alphabetically) after the directory-names tested 
		while (i < _files.size() && strcmp(name.c_str(), _files[i]->getName().c_str()) > 0)
		{
			if (name == _files[i]->getName())
				return -1;
			i++;
		}
		return i;
	}
}

//Processes the path-string given
Directory * Directory::processPath(const std::string & path)
{
	if (path != "")
	{
		if (path == "..")
			return _parent;
		else if (path.substr(0, 3) == "../")
			return _parent->processPath(path.substr(3));
		else if (path.substr(0, 2) == "./")
			return this->processPath(path.substr(2));
		unsigned int end = path.find_first_of("/");
		bool lastPart = false;
		if (end >= path.length() - 1)
			lastPart = true;
		std::string testString = path.substr(0, end);
		for (unsigned int i = 0; i < _directories.size(); i++)
		{
			if (testString == _directories[i]->getName())
			{
				if (lastPart)
					return _directories[i];
				else
					return _directories[i]->processPath(path.substr(end + 1));
			}
		}
		return nullptr;
	}
	else
	{
		return this;
	}
}

//Add a child directory
std::string Directory::addDirectory(const std::string & name)
{
	//Special case if it´s the first one created
	if (_directories.size() == 0)
	{
		_directories.push_back(new Directory(name, this));
		return "Directory creation successful!\n";
	}
	//Normal case
	else
	{
		unsigned int i = 0;
		//Loop while 'name' comes (alphabetically) after the directory-names tested 
		while (i < _directories.size() && strcmp(name.c_str(), _directories[i]->getName().c_str()) > 0)
		{
			if (name == _directories[i]->getName())
				return "Name already used.\n";
			i++;
		}
		_directories.insert(_directories.begin() + i, new Directory(name, this));
		return "Directory creation successful!\n";
	}
}

//Add a child file
std::string Directory::addFile(const std::string & name, int accessRights, int size, const std::vector<Block*>& blocks, const std::vector<int> & usedIndexes)
{
	_files.push_back(new File(name, accessRights, size, blocks, usedIndexes));
	_files.insert(_files.begin() + index, new File(name, size, blocks, usedIndexes));
	_files.push_back(new File(name, size, blocks, usedIndexes));
	return "";
}

std::string Directory::renameFile(const std::string & prevName, const std::string & newName)
{
	std::string output = "Invalid file name.\n";
	for (int i = 0; i < _files.size(); i++)
		if (_files[i]->getName() == prevName)
		{
			_files[i]->setName(newName);
			output = "Rename successful.\n";
			break;
		}

	return output;
}

std::string Directory::removeFile(const std::string & name)
{
	std::string output = "Invalid file name.\n";
	for (int i = 0; i < _files.size(); i++)
		if (_files[i]->getName() == name)
		{
			_files.erase(_files.begin() + i);
			output = "Removal successful.\n";
			break;
		}
	return output;
}


bool Directory::removeFile(const std::string & name, std::vector<int>& usedIndexes)
{
	int index = -1;
	for (unsigned int i = 0; i < _files.size() && index == -1; i++)
	{
		if (name == _files[i]->getName())
			index = i;
	}
	if (index != -1)
	{
		usedIndexes = _files[index]->getUsedIndexes();
		delete _files[index];
		_files.erase(_files.begin() + index);
		return true;
	}
	else
		return false;
}
