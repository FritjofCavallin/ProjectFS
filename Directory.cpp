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
	bool ok = true;
	for (unsigned int i = 0; i < _directories.size() && ok; i++)
	{
		if (name == _directories[i]->getName())
			ok = false;
	}
	if (ok)
	{
		_directories.push_back(new Directory(name, this));
		return "";
	}
	else
		return "Name already used.\n";
}

//Add a child file
std::string Directory::addFile(const std::string & name, int size, const std::vector<Block*>& blocks)
{
	_files.push_back(new File(name, size, blocks));
	return "";
}
