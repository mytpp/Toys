#pragma once


#include "Token.h"

namespace LL1 {
	
	void GenerateLL1Table();

	void PrintLL1Table();

	//'n' stands for "num" here
	void Parse(std::wstring tokens);
}


