#pragma once

#include <vector>

#include "block.h"

class File
{
private:
	std::vector<Block*>		_blocks;
	std::string				_name;
	int						_realSize;
	std::vector<int>		_usedIndexes;

public:
	File();
	File(const std::string & fileName, int realSize, const std::vector<Block*> & blocks
		, const std::vector<int> & usedIndexes);
	virtual ~File();
	std::string getName() const;
	std::string getFileInfo() const;
	std::string getData() const;
	std::vector<int> getUsedIndexes() const;
};

