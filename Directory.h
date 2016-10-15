#pragma once

#include <vector>
#include <iostream>
#include <fstream>
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
	std::string getName();
	Directory* getParent() const;
	std::string toString() const;
	std::string getInfoString() const;
	std::string getPath() const;
	Directory* getDirectory(const unsigned int & index);
	File* getFile(const unsigned int & index);
	File* getFile(const std::string & name);
	bool getFileData(const std::string & name, std::string & data) const;
	void getChildren(int* children);
	Directory* processPath(const std::string & path);
	std::string addDirectory(const std::string & name);
	std::string addFile(const std::string & name, int size, const std::vector<Block*> & blocks);
	std::string renameFile(const std::string & prevName, const std::string & newName);
	std::string removeFile(const std::string & name);
	std::string addFile(const std::string & name, int size
		, const std::vector<Block*> & blocks, const std::vector<int> & usedIndexes);
	bool removeFile(const std::string & name, std::vector<int>& usedIndexes);
};

