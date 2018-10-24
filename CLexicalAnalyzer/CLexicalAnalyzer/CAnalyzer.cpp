#include "CAnalyzer.h"
#include <cctype>
#include <iostream>


CAnalyzer::CAnalyzer(const std::string& srcPath) try
	:buffer(srcPath)
	, state(0)
	//,curLetter()
	, letterAvailable(false)
	, item()
	, itemAvailable(false)
	, end(false)
	, error(false)
	//,errorMessage()
	, statistics(new Statistics{ 1, -1, 0 })
{ }
catch (std::runtime_error& e) {
	std::cout << e.what() << std::endl;
	end = true;
}

CAnalyzer::~CAnalyzer() = default;

std::pair<CAnalyzer::Item, bool> CAnalyzer::getItem()
{
	while (!itemAvailable)
	{
		if (!letterAvailable)
			curLetter = getLetter();
		letterAvailable = !eat(curLetter);
	}
	itemAvailable = false;

	if (error)
	{
		error = false;
		return { item, false };
	}
	return { item, true };
}

char CAnalyzer::getLetter()
{
	char temp = buffer.get();

	//record
	statistics->charCount++;
	if (temp == '\n')
		statistics->lineCount++;

	return temp;
}

bool CAnalyzer::isKey(const std::string & lexeme)
{
	for (auto key : keyWords)
		if (lexeme == key)
			return true;
	return false;
}


bool CAnalyzer::eat(const char letter)
{
	switch (state)
	{
	case 0:
		resetItem();
		if (isspace(letter))
			break;
		else if (isalpha(letter) || letter == '_')
		{
			state = 1;
			item.token += letter;
		}
		else if (isdigit(letter))
		{
			state = 2;
			item.token += letter;
		}
		else switch (letter)
		{
		case '[': throwUp({ "[","" }); break;
		case ']': throwUp({ "]","" }); break;
		case '.': throwUp({ ".","" }); break;  //'...'?
		case '~': throwUp({ "~","" }); break;
		case '(': throwUp({ "(","" }); break;
		case ')': throwUp({ ")","" }); break;
		case ',': throwUp({ ",","" }); break;
		case '?': throwUp({ "?","" }); break;
		case ':': throwUp({ ":","" }); break;
		case '{': throwUp({ "{","" }); break;
		case '}': throwUp({ "}","" }); break;
		case ';': throwUp({ ";","" }); break;

		case '<': state = 8; break;
		case '>': state = 9; break;
		case '=': state = 10; break;
		case '!': state = 11; break;
		case '+': state = 12; break;
		case '-': state = 13; break;
		case '*': state = 14; break;
		case '%': state = 15; break;
		case '|': state = 16; break;
		case '&': state = 17; break;
		case '^': state = 18; break;
		case '\'': state = 21; item.token += letter; break;
		case '\"': state = 24; item.token += letter; break;
		case '/': state = 26; break;

		case DoubleBuffer::eof: 
			end = true; 
			error = true;
			itemAvailable = true; 
			errorMessage = "eof"; 
			break;

		default: 
			handleIllegalIdentifier();
			return false;
		}
		break;

	case 1: 
		if (isalnum(letter) || letter == '_')
			item.token += letter;
		else
		{
			if (isKey(item.token))
				throwUp({ item.token, "" });
			else
				throwUp({ "id",item.token });
			return false;
		}
		break;

	case 2:
		if (isdigit(letter))
			item.token += letter;
		else switch (letter)
		{
		case '.': state = 3; item.token += letter; break;
		case 'e':
		case 'E': state = 5; item.token += letter; break;
		default:
			throwUp({ "integerNum",item.token });
			return false;
		}
		break;

	case 3:
		if (isdigit(letter))
		{
			state = 4;
			item.token += letter;
		}
		else
		{
			handleIllegalNumber();
			return false;
		}
		break;

	case 4:
		if (isdigit(letter))
			item.token += letter;
		else switch (letter)
		{
		case 'e':
		case 'E': state = 5; item.token += letter; break;
		default:
			throwUp({ "floatNum",item.token });
			return false;
		}
		break;

	case 5:
		if (isdigit(letter))
		{
			state = 7;
			item.token += letter;
		}
		else switch (letter)
		{
		case '+':
		case '-': state = 6; item.token += letter; break;
		default:
			handleIllegalNumber();
			return false;
		}
		break;

	case 6:
		if (isdigit(letter))
		{
			state = 7;
			item.token += letter;
		}
		else
		{
			handleIllegalNumber();
			return false;
		}
		break;

	case 7:
		if (isdigit(letter))
			item.token += letter;
		else
		{
			throwUp({ "float",item.token });
			return false;
		}
		break;

	case 8://<
		switch (letter)
		{
		case '=': throwUp({ "<=","" }); break;
		case '<': state = 19; break;
		default:
			throwUp({ "<","" });
			return false;
		}
		break;

	case 9://>
		switch (letter)
		{
		case '=': throwUp({ ">=","" }); break;
		case '>': state = 20; break;
		default:
			throwUp({ ">","" });
			return false;
		}
		break;

	case 10://=
		if (letter == '=')
			throwUp({ "==","" });
		else
		{
			throwUp({ "=","" });
			return false;
		}
		break;

	case 11://!
		if (letter == '=')
			throwUp({ "!=","" });
		else
		{
			throwUp({ "!","" });
			return false;
		}
		break;

	case 12://+
		switch (letter)
		{
		case '=': throwUp({ "+=","" }); break;
		case '+': throwUp({ "++","" }); break;
		default:
			throwUp({ "+","" });
			return false;
		}
		break;

	case 13://-
		switch (letter)
		{
		case '=': throwUp({ "-=","" }); break;
		case '-': throwUp({ "--","" }); break;
		case '>': throwUp({ "->","" }); break;
		default:
			throwUp({ "-","" });
			return false;
		}
		break;

	case 14://*
		if (letter == '=')
			throwUp({ "*=","" });
		else
		{
			throwUp({ "*","" });
			return false;
		}
		break;

	case 15://%
		if (letter == '=')
			throwUp({ "%=","" });
		else
		{
			throwUp({ "%","" });
			return false;
		}
		break;

	case 16://|
		switch (letter)
		{
		case '=': throwUp({ "|=","" }); break;
		case '|': throwUp({ "||","" }); break;
		default:
			throwUp({ "|","" });
			return false;
		}
		break;

	case 17://&
		switch (letter)
		{
		case '=': throwUp({ "&=","" }); break;
		case '&': throwUp({ "&&","" }); break;
		default:
			throwUp({ "&","" });
			return false;
		}
		break;

	case 18://^
		if (letter == '=')
			throwUp({ "^=","" });
		else
		{
			throwUp({ "^","" });
			return false;
		}
		break;

	case 19://<<
		if (letter == '=')
			throwUp({ "<<=","" });
		else
		{
			throwUp({ "<<","" });
			return false;
		}
		break;

	case 20://>>
		if (letter == '=')
			throwUp({ ">>=","" });
		else
		{
			throwUp({ ">>","" });
			return false;
		}
		break;

	case 21:// \'
		item.token += letter;
		if (letter == '\\')
			state = 22;
		else
			state = 23;
		break;

	case 22: // \'+\\ 
		item.token += letter;
		state = 23;
		break;

	case 23:
		if (letter == '\'')
			throwUp({ "charLiteral","" });
		else
		{
			handleIllegalCharLiteral();
			return false;
		}
		break;

	case 24:// \"
		item.token += letter;
		if (letter == '\\')
			state = 25;
		else if (letter == '\"')
			throwUp({ "strLiteral",item.token });
		break;

	case 25: // \"+\\ 
		item.token += letter;
		state = 24;
		break;


	case 26: // / 
		switch (letter)
		{
		case '/': state = 27; break;
		case '*': state = 28; break;
		default:
			throwUp({ "/","" });
			return false;
		}
		break;

	case 27:
		if (letter == '\n')
			state = 0;
		break;

	case 28:
		if (letter == '*')
			state = 29;
		break;

	case 29:
		if (letter == '/')
			state = 0;
		else
			state = 28;
		break;

	default:
		handleIllegalState();
		return false;
	}

	return true;
}

void CAnalyzer::throwUp(const Item & item)
{
	this->item = item;
	if (this->item.property == "")
		this->item.property = "null";
	itemAvailable = true;
	state = 0;
	logToken(item.token);
}

void CAnalyzer::logToken(const std::string & token)
{
	++statistics->tokens[token];
}


void CAnalyzer::continueRunning()
{
	while (true)
	{
		if (isspace(curLetter))
			break;
		if (curLetter == ';' || curLetter == ')' || curLetter == '}')
		{
			logToken(std::string(1, curLetter));
			break;
		}
		if (curLetter == DoubleBuffer::eof)
		{
			end = true;
			errorMessage = "eof";
			break;
		}
		curLetter = getLetter();
	}
	
	state = 0;
	itemAvailable = true;
	error = true;
	++statistics->errorCount;
}



const std::array<const char*, 34> CAnalyzer::keyWords =
{ "auto","break","case","char","const","continue",
"default","do","double","else","enum","extern",
"float","for","goto","if","inline","int","long",
"register","restrict","return","short","signed",
"sizeof","static","struct","switch","typedef",
"union","unsigned","void","volatile","while"
};//34
