#include "PM/board.h"

namespace pm
{
	Board::Board()
	{
	}
	
	bool Board::loadFromFile(const char* path)
	{
		bool status = false;
		if((status = m_tree.loadFromFile(path)))
			init();
		
		return status;  
	}
	
	bool Board::loadFromFile(const std::string& path)
	{
		bool status = false;
		if(status = m_tree.loadFromFile(path.c_str()))
			init();
		return status;
	}
	
	bool Board::loadFromMemory(const void* memory, SPsize length)
	{
		bool status = false;
		if(status = m_tree.loadFromMemory(memory, length))
			init();
		return status;
	}
	
	bool Board::init()
	{
		return true;
	}
	
}