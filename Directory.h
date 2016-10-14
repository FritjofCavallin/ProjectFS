#pragma once

#include <vector>

#include "File.h"

class Directory
{
private:
	std::vector<Directory*>		_directories;
	std::vector<File*>			_files;
	std::string					_name;
	Directory*					_parent;

public:
	Directory();
	Directory(const std::string & name, Directory* parent);
	virtual ~Directory();
	std::string getName() const;
	Directory* getParent() const;
	std::string toString() const;
	std::string getInfoString() const;
	Directory* processPath(const std::string & path);
	std::string addDirectory(const std::string & name);
	std::string addFile(const std::string & name, int size, const std::vector<Block*> & blocks);
};

