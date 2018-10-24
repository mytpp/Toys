#pragma once

#include "DoubleBuffer.h"
#include <utility>
#include <memory>
#include <unordered_map> 


class CAnalyzer final
{
	/******************************  TYPEs  *********************************/
public:
	//the output token with its property
	struct Item final
	{
		std::string token;
		std::string property;
	};

private:
	//some statistic infomation about the source C code
	struct Statistics final
	{
		int lineCount;
		int charCount;
		int errorCount;
		std::unordered_map<std::string, size_t> tokens;
	};


	/***************************  INTERFACEs  ******************************/
public:
	CAnalyzer(const std::string& srcPath = "src.c");

	~CAnalyzer();

	//get a item form the lexical analyzer
	//the 'returnValue'.second indicate whether an error is met when generating the item
	std::pair<Item,bool> getItem();

	//get the statistic infomation of src file, 
	//available when the analyzer finishes its work
	std::unique_ptr<Statistics>& getStatisticsInfo() { return statistics; }

	//when 'getItem()' meets an error, call this func to get info about the err.
	std::string& getErrorMessage() { return errorMessage; }

	
	/************************  PRIVATE METHODs  ***************************/
private:
	//extract a charater from the buffer
	char getLetter();

	//is the 'lexeme' an keyword?
	bool isKey(const std::string& lexeme);

	//prepare for the generation of next token
	void resetItem() { item.token.clear(); item.property.clear(); }

	//eat a charater to push the automachine to go
	//If 'letter' needs to be retracted, return false. Otherwise return true.
	bool eat(const char letter);

	//vomit an item
	void throwUp(const Item& item);

	//log the info about a token
	void logToken(const std::string& token);

	//when meet a illegal pattern, 
	//skip some charaters to continue running the analyzer
	void continueRunning();
	
	//as its name
	void handleIllegalNumber() { 
		errorMessage = "ERROR(line "+ std::to_string(statistics->lineCount) +"): Illegal Number"; 
		continueRunning(); 
	}

	//as its name
	void handleIllegalIdentifier() { 
		errorMessage = "ERROR(line " + std::to_string(statistics->lineCount) + "): Illegal lIdentifier"; 
		continueRunning(); 
	}

	//as its name
	void handleIllegalCharLiteral() { 
		errorMessage = "ERROR(line " + std::to_string(statistics->lineCount) + "): Illegal Charater Literal"; 
		continueRunning(); 
	}

	//as its name
	void handleIllegalState() { 
		errorMessage = "ERROR(line " + std::to_string(statistics->lineCount) + "): Illegal State"; 
		continueRunning(); 
	}



	/****************************  FIELDs  **********************************/
private:
	DoubleBuffer buffer;       //buffer to read from

	int state;                 //state of DFA
	char curLetter;            //the current charater being processing
	bool letterAvailable;      //necessary to extract another character from the buffer?
	Item item;                 //the item to vomit
	bool itemAvailable;        //can we end eating?
	bool end;                  //have we met the end of file?
	bool error;                //error occurs?
	std::string errorMessage;  //error message

	std::unique_ptr<Statistics> statistics; //statistic infomation about the source C code

	static const std::array<const char*, 34> keyWords;
};

