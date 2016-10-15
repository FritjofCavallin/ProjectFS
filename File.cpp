#include "File.h"

//Default constructor
File::File()
{
	_name = "DEFAULT";
	_realSize = 0;
}

//Constructor
File::File(const std::string & fileName, int accessRights, int realSize, const std::vector<Block*> & blocks, const std::vector<int> & usedIndexes)
{
	_name = fileName;
	_realSize = realSize;
	_accessRights = accessRights;
	_usedIndexes = usedIndexes;
	for (unsigned int i = 0; i < blocks.size(); i++)
	{
		_blocks.push_back(blocks[i]);
	}
}

//Destructor
File::~File()
{
	//Nothing
}

//Returns name of file
std::string File::getName() const
{
	return _name;
}

//Get info about the file
std::string File::getFileInfo() const
{
	std::string tempName = _name;
	if (_name.length() > 16)
		tempName = _name.substr(0, 16);
	std::string spaces = "    ";
	for (unsigned int i = 0; i < 16 - tempName.length(); i++)
	{
		spaces += " ";
	}
	return "F:  " + tempName + spaces + std::to_string(_realSize) + "\t" + 
		std::to_string(_blocks.size() * 512) + "\n";
}

//Returns all actual data the file contains
std::string File::getData() const
{
	std::string data = "";
	data.reserve(_realSize);
	for (unsigned int i = 0; i < _blocks.size(); i++)
	{
		data += _blocks[i]->toString();
	}
	return data.substr(0, _realSize);
}

//Returns acces right value
int File::getAccessRights()
{
	return _accessRights;
}

//Returns size of file
int File::getSize() const
{
	return _realSize;
}

//Sets name of file to param name
void File::setName(const std::string & name)
{
	_name = name;
}

//Sets acces rights to param accesRights
void File::setAccessRights(const unsigned int & accessRights)
{
	_accessRights = accessRights;
}

//Returns vec array of used indexes
std::vector<int> File::getUsedIndexes() const
{
	return _usedIndexes;
}
