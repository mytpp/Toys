#pragma once

#include <array>
#include <string>
#include <fstream>

class DoubleBuffer final
{
public:
	static constexpr char eof = -1;
private:
	static constexpr size_t bufferSize = 512;

public:
	DoubleBuffer(const std::string& path);
	~DoubleBuffer();

	//get a character from the buffer
	char get();

private:
	//read 'bufferSize' charaters to 'buffer'.
	//when meeting the end of file, add a eof flag to the buffer.
	//if we have reach the end of file, return false. otherwise return true
	bool readFileWithEOF(char (&buffer)[bufferSize]);

private:
	std::ifstream src;      //the input  file
	char left[bufferSize];  //the left buffer
	char right[bufferSize]; //the right buffer
	char* forward;          //pointer to next charater being read
};

