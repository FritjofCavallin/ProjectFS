#ifndef FILE_H
#define FILE_H

#include <vector>

#include "block.h"

class File
{
private:
	std::vector<Block*>		_blocks;
	std::string				_name;
	int						_realSize;
	int						_accessRights;

public:
	File();
	File(const std::string & fileName, int accessRigts, int realSize, const std::vector<Block*> & blocks);
	virtual ~File();
	std::string getName() const;
	std::string getFileInfo() const;
	std::string getData() const;
	int getAccessRights();
	std::vector<Block*> getUsedIndexes() const;
	int getSize() const;
	void setName(const std::string & name);
	void setAccessRights(const unsigned int & accessRights);
};

#endif
