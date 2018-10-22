#pragma once

#include "DoubleBuffer.h"
#include <utility>
#include <memory>
#include <unordered_map> 


class CAnalyzer final
{
	/***************************  TYPEs  ******************************/
public:
	struct Item final
	{
		std::string token;
		std::string property;
	};

private:
	struct Statistics final
	{
		int lineCount;
		int charCount;
		int errorCount;
		std::unordered_map<std::string, size_t> tokens;
	};


	/************************  INTERFACEs  ***************************/
public:
	CAnalyzer(const std::string& srcPath = "src.c");

	~CAnalyzer();

	std::pair<Item,bool> getItem();
	std::unique_ptr<Statistics>& getStatisticsInfo() { return statistics; }
	std::string& getErrorMessage() { return errorMessage; }

	
private:
	char getLetter();
	bool isKey(const std::string& lexeme);
	void resetItem() { item.token.clear(); item.property.clear(); }

	//If 'letter' needs to be retracted, return false.
	//Otherwise return true.
	bool eat(const char letter);

	void throwUp(const Item& item);

	void logToken(const std::string& token);

	void continueRunning();
	
	void handleIllegalNumber() { 
		errorMessage = "ERROR(line "+ std::to_string(statistics->lineCount) +"): Illegal Number"; 
		continueRunning(); 
	}

	void handleIllegalIdentifier() { 
		errorMessage = "ERROR(line " + std::to_string(statistics->lineCount) + "): Illegal lIdentifier"; 
		continueRunning(); 
	}

	void handleIllegalCharLiteral() { 
		errorMessage = "ERROR(line " + std::to_string(statistics->lineCount) + "): Illegal Charater Literal"; 
		continueRunning(); 
	}

	void handleIllegalState() { 
		errorMessage = "ERROR(line " + std::to_string(statistics->lineCount) + "): Illegal State"; 
		continueRunning(); 
	}



	/****************************  FIELDs  **********************************/
private:
	DoubleBuffer buffer;

	int state;
	char curLetter;
	bool letterAvailable;
	Item item;
	bool itemAvailable;
	bool end;
	bool error;
	std::string errorMessage;

	std::unique_ptr<Statistics> statistics;

	static const std::array<const char*, 34> keyWords;
};

