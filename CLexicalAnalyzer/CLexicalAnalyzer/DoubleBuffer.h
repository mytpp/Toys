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

	char get();

private:
	bool readFileWithEOF(char (&buffer)[bufferSize]);

private:
	std::ifstream src;
	char left[bufferSize];
	char right[bufferSize];
	char* forward;
};

