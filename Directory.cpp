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
	//Nothing
}

//Get directory name
std::string Directory::getName() const
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
		output.append(_directories[i].toString());
	}
	for (unsigned int i = 0; i < _files.size(); i++)
	{
		output.append(_files[i].getFileInfo());
	}
	return output;
}

//Processes the path-string given
Directory * Directory::processPath(const std::string & path)
{
	//if (path.substr(0, 3) == "../")
	//	return _parent->processPath(path.substr(3));
	//else if (path == "./")
	//	return this;
	//else if (path.substr(0, 2) == "./")
	//	return this->processPath(path.substr(2));
	//else
	//{

	if (path != "")
	{
		unsigned int end = path.find("/");
		bool lastPart = false;
		if (end >= path.length() - 1)
			lastPart = true;
		std::string testString = path.substr(0, end);
		for (unsigned int i = 0; i < _directories.size(); i++)
		{
			if (testString == _directories[i].getName())
			{
				if (lastPart)
					return &_directories[i];
				else
					return _directories[i].processPath(path.substr(end + 1));
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
		if (name == _directories[i].getName())
			ok = false;
	}
	if (ok)
	{
		_directories.push_back(Directory(name, this));
		return "";
	}
	else
		return "Name already used.\n";
}

//Add a child file
std::string Directory::addFile(const std::string & name, int size, const std::vector<Block*>& blocks)
{
	_files.push_back(File(name, size, blocks));
	return "";
}
