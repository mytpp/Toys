#include "LL1.h"
#include <iostream>
#include <iomanip>
#include <type_traits> 
#include <vector>
#include <array>
#include <stack>


namespace LL1 {
	/**************************************VARIABLE***************************************/

	//It should be more efficient to use string rather than vector,
	//given that SSO may be performed by complier
	const std::array<Production, 10> productions = { {
			{ E, L"Te"},
			{ e, L"+Te" },
			{ e, L"-Te" },
			{ e, L"¦Å" },
			{ T, L"Ft" },
			{ t, L"*Ft" },
			{ t, L"/Ft" },
			{ t, L"¦Å" },
			{ F, L"(E)" },
			{ F, L"n" }   //n stands for num
		} };

	//the FIRST set
	const std::array<std::vector<Terminator>, 5> first = { {
			{ lpar, num },          //E
			{ add, sub, epsilon },  //e
			{ lpar, num },          //T
			{ mul, divi, epsilon }, //t
			{ lpar, num }           //F
		} };

	//the FOLLOW set
	const std::array<std::vector<Terminator>, 5> follow = { {
			{ dollar, rpar },                     //E
			{ dollar, rpar },                     //e
			{ dollar, rpar, add, sub },           //T
			{ dollar, rpar, add, sub },           //t
			{ dollar, rpar, add, sub, mul, divi } //F
		} };

	//null Production means error
	std::array<std::array<Production, 8>, 5> table = {};

	//Used by parser
	std::stack<Token> tokenStack;


	/**********************************fUNCTION*************************************/
	Terminator MapWchar2Terminator(wchar_t cha) {
		switch (cha)
		{
		case L'n': return num;
		case L'+': return add;
		case L'-': return sub;
		case L'*': return mul;
		case L'/': return divi;
		case L'(': return lpar;
		case L')': return rpar;
		case L'$': return dollar;
		case L'¦Å': return epsilon;
		default: return invalidTerminator;
		}
	}

	NonTerminator MapWchar2NonTerminator(wchar_t cha) {
		switch (cha)
		{
		case L'E': return E;
		case L'e': return e;
		case L'T': return T;
		case L't': return t;
		case L'F': return F;
		default: return invalidNonTerminator;
		}
	}

	//Return the FIRST set for the given symbol string 'tokens'
	const std::vector<Terminator> FirstOf(const std::wstring& tokens) {
		auto terminator = MapWchar2Terminator(tokens.front());
		if (terminator != invalidTerminator)
			return { terminator };

		auto nonTerminator = MapWchar2NonTerminator(tokens.front());
		if (nonTerminator != invalidNonTerminator)
			return first[nonTerminator];

		return {};
	}

	void GenerateLL1Table() {
		for (auto& production : productions) {
			for (auto terminator : FirstOf(production.second)) {
				if (terminator != epsilon)
					table[production.first][terminator] = production;
				else
					for (auto& terminator : follow[production.first])
						table[production.first][terminator] = production;
			}
		}
	}


	//Next four functions are used for formatted output 

	void PrintTerminator(Terminator t, std::streamsize w = 12) {
		switch (t)
		{
		case num:   std::wcout << std::setw(w) << L"num";
			break;	     
		case add:   std::wcout << std::setw(w) << L'+';
			break;	     
		case sub:   std::wcout << std::setw(w) << L'-';
			break;	     
		case mul:   std::wcout << std::setw(w) << L'*';
			break;	     
		case divi:  std::wcout << std::setw(w) << L'/';
			break;	     
		case lpar:  std::wcout << std::setw(w) << L'(';
			break;
		case rpar:  std::wcout << std::setw(w) << L')';
			break;
		case dollar:std::wcout << std::setw(w) << L'$';
			break;
		case epsilon:std::wcout << std::setw(w) << L'¦Å';
			break;
		default:
			break;
		}
	}

	void PrintNonTerminator(NonTerminator nt, std::streamsize w = 12) {
		switch (nt)
		{
		case E: std::wcout << std::setw(w) << L'E';
			break;
		case e: std::wcout << std::setw(w) << L'e';
			break;
		case T: std::wcout << std::setw(w) << L'T';
			break;
		case t: std::wcout << std::setw(w) << L't';
			break;
		case F: std::wcout << std::setw(w) << L'F';
			break;
		default:
			break;
		}
	}

	void PrintProduction(const Production& production, std::streamsize w = 12) {
		//Every production occupy 12 units
		PrintNonTerminator(production.first, 0);
		std::wcout << L"->";
		std::wcout << std::setw(w - 3) << production.second;
	}

	void PrintLL1Table()
	{	
		std::wcout << std::endl;
		std::wcout << std::left;

		//header
		std::wcout << std::wstring(12, ' ');
		for (std::underlying_type_t<Terminator> i = 0; i != epsilon; i++)
			PrintTerminator(static_cast<Terminator>(i));
		std::wcout << std::endl;

		//content
		std::underlying_type_t<NonTerminator> i = 0;
		for (auto& row : table) {
			PrintNonTerminator(static_cast<NonTerminator>(i++));
			for (auto& item : row) {
				if (item.second != L"")   //if there exists a production
					PrintProduction(item);
				else
					std::wcout << std::setw(12) << L"error";
			}
			std::wcout << std::endl;
		}

		std::wcout << std::endl;
		std::wcout << std::endl;
	}


	void Parse(std::wstring tokens)
	{
		tokenStack.push(dollar);
		tokenStack.push(E);

		tokens += L'$';//add end token to the symbol string
		auto it = tokens.cbegin();
		do {
			auto top = tokenStack.top();
			auto ter = MapWchar2Terminator(*it);

			if (top.isTernaminator) {//top of stack is Ternaminator
				if (top.t == ter) {
					tokenStack.pop();
					it++;
				} else {
					//handle error
				}
			} else {                 //top of stack is NonTernaminator
				auto rexpr = table[top.nt][ter].second;
				if (rexpr != L"") {
					tokenStack.pop();

					for (auto ri = rexpr.rbegin(); ri != rexpr.rend(); ri++) {
						auto terminator = MapWchar2Terminator(*ri);
						if (terminator != invalidTerminator && terminator != epsilon) {
							tokenStack.push(terminator);
						} else {
							auto nonTerminator = MapWchar2NonTerminator(*ri);
							if (nonTerminator != invalidNonTerminator)
								tokenStack.push(nonTerminator);
							else
								;//handle error
						}
					}

					PrintProduction(table[top.nt][ter]);
					std::wcout << std::endl;
				} else {
					//handle error
				}
			}

		} while (tokenStack.top() != dollar);
	}

}
