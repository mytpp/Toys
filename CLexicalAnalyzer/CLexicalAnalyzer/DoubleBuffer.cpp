#include "DoubleBuffer.h"
#include <stdexcept> 
#include <iostream>


DoubleBuffer::DoubleBuffer(const std::string & path)
	:src(path)
	//,left(default initialzed)
	//,right(default initialized)
	, forward(&left[0])
{
	if (!src.is_open())
		throw std::runtime_error("Can't open file");
	readFileWithEOF(left);
}

DoubleBuffer::~DoubleBuffer()
{
}

char DoubleBuffer::get()
{
	
	if (forward == &left[bufferSize - 1] + 1)
	{
		readFileWithEOF(right);
		forward = &right[0];
	}
	else if (forward == &right[bufferSize - 1] + 1)
	{
		readFileWithEOF(left);
		forward = &left[0];
	}
		
	return *forward++;
}

bool DoubleBuffer::readFileWithEOF(char(&buffer)[bufferSize])
{
	if (!src.read(&buffer[0], bufferSize))
	{
		buffer[src.gcount()] = eof;
		
		/*for (auto e : buffer) {
		std::cout << e;
		}*/

		return false;
	}

	//for (auto e : buffer) {
	//	std::cout << e;
	//}

	return true;
}
