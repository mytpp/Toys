#include "LL1.h"
#include <iostream>
#include <iomanip>
#include <type_traits> 

namespace LL1 {

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
	extern std::array<std::array<Production, 8>, 5> table = {};


	//Return the FIRST set for the given symbol string 'tokens'
	const std::vector<Terminator> FirstOf(const std::wstring& tokens) {
		switch (tokens.front())
		{
		case L'n': return { num };
		case L'+': return { add };
		case L'-': return { sub };
		case L'*': return { mul };
		case L'/': return { divi };
		case L'(': return { lpar };
		case L')': return { rpar };
		case L'¦Å': return { epsilon };

		case L'E': return first[E];
		case L'e': return first[e];
		case L'T': return first[T];
		case L't': return first[t];
		case L'F': return first[F];

		default: return{};
		}
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

	void PrintTerminator(Terminator t, std::streamsize w = 12) {
		switch (t)
		{
		case LL1::num:   std::wcout << std::setw(w) << L"num";
			break;	     
		case LL1::add:   std::wcout << std::setw(w) << L'+';
			break;	     
		case LL1::sub:   std::wcout << std::setw(w) << L'-';
			break;	     
		case LL1::mul:   std::wcout << std::setw(w) << L'*';
			break;	     
		case LL1::divi:  std::wcout << std::setw(w) << L'/';
			break;	     
		case LL1::lpar:  std::wcout << std::setw(w) << L'(';
			break;
		case LL1::rpar:  std::wcout << std::setw(w) << L')';
			break;
		case LL1::dollar:std::wcout << std::setw(w) << L'$';
			break;
		case LL1::epsilon:std::wcout << std::setw(w) << L'¦Å';
			break;
		default:
			break;
		}
	}

	void PrintNonTerminator(NonTerminator nt, std::streamsize w = 12) {
		switch (nt)
		{
		case LL1::E: std::wcout << std::setw(w) << L'E';
			break;
		case LL1::e: std::wcout << std::setw(w) << L'e';
			break;
		case LL1::T: std::wcout << std::setw(w) << L'T';
			break;
		case LL1::t: std::wcout << std::setw(w) << L't';
			break;
		case LL1::F: std::wcout << std::setw(w) << L'F';
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
		std::wcout << std::left;
		std::wcout << std::wstring(12, ' ');
		for (std::underlying_type_t<Terminator> i = 0; i != epsilon; i++)
			PrintTerminator(static_cast<Terminator>(i));
		std::wcout << std::endl;

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
	}

	void Parse(std::wstring tokens)
	{

	}

	

}
