#include "CAnalyzer.h"

#include<iostream>

int main()
{
	CAnalyzer analyzer("src.c");
	
	bool goOn = true;

	while (goOn) {
		auto [item,success] = analyzer.getItem();
		
		if (!success)
		{
			if (analyzer.getErrorMessage() == "eof")
				goOn = false;
			else
				std::cout << '\n' + analyzer.getErrorMessage() + '\n' << std::endl;
		}
		else
			std::cout << "token:" << item.token << '\t' << "property:" << item.property << std::endl;
	}

	std::cout << "\n\n";
	std::cout << std::string(30, '*') << std::endl;

	auto& stat = analyzer.getStatisticsInfo();
	std::cout << "lineCount:    " << stat->lineCount << std::endl;
	std::cout << "charCount:    " << stat->charCount << std::endl;
	std::cout << "errorCount:   " << stat->errorCount << std::endl;

	for (const auto& token : stat->tokens)
	{
		std::cout << token.first+':' << token.second << std::endl;
	}

	system("pause");
	return 0;
}

