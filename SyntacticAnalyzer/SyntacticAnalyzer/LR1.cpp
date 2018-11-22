#include "LR1.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <array>
#include <stack>

namespace SLR1 {

	enum NonTerminator : uint8_t
	{
		E = 0,
		T = 1,
		F = 2,
		invalidNonTerminator = 255
	};

	enum Terminator : uint8_t
	{
		num = 0,
		add = 1,    // +
		sub = 2,    // -
		mul = 3,    // *
		divi = 4,   // /
		lpar = 5,   // (
		rpar = 6,   // )
		dollar = 7, // $ (not terminator actually)
		epsilon = 8,// ¦Å (not terminator actually)
		invalidTerminator = 255
	};

	//Empty string means the production is null
	//As '¦Å' is not included in ASCII, we use wstring to represent the symbol string
	using Production = std::pair<NonTerminator, std::wstring>;

	//A syntactic token maybe NonTerminator or Terminator
	struct Token
	{
		Token(NonTerminator nt)
			:isTernaminator(false)
			, nt(nt)
		{}

		Token(Terminator t)
			:isTernaminator(true)
			, t(t)
		{}

		bool isTernaminator;
		union {
			NonTerminator nt;
			Terminator    t;
		};
	};

	bool operator==(Token token, Terminator t) {
		return token.isTernaminator && token.t == t;
	}

	bool operator==(Token token, NonTerminator nt) {
		return !token.isTernaminator && token.nt == nt;
	}

	bool  operator!=(Token token, Terminator t) {
		return !(token == t);
	}

	bool  operator!=(Token token, NonTerminator nt) {
		return !(token == nt);
	}


	enum ActionCategory
	{
		Error,
		Shift,
		Reduce,
		Accept
	};

	using Action = std::pair<ActionCategory, int>;

	/**************************************VARIABLE***************************************/
	const std::array<Production, 8> productions = { {
		{E, L"E+T"},
		{E, L"E-T"},
		{E, L"T"  },
		{T, L"T*F"},
		{T, L"T/F"},
		{T, L"F"  },
		{F, L"(E)"},
		{F, L"n"  }
	} };

	std::array<std::array<Action, 8>, 16> action{ {
			/*0 */	{{ {Shift, 5}, {}, {}, {}, {}, {Shift, 4}, {}, {} }},
			/*1 */	{{ {}, {Shift, 6}, {Shift, 7}, {}, {}, {}, {}, {Accept, 0} }},
			/*2 */	{{ {}, {Reduce, 2}, {Reduce, 2}, {Shift, 8}, {Shift, 9}, {}, {Reduce, 2}, {Reduce, 2} }},
			/*3 */	{{ {}, {Reduce, 5}, {Reduce, 5}, {Reduce, 5}, {Reduce, 5}, {}, {Reduce, 5}, {Reduce, 5} }},
			/*4 */	{{ {Shift, 5}, {}, {}, {}, {}, {Shift, 4}, {}, {} }},
			/*5 */	{{ {}, {Reduce, 7}, {Reduce, 7}, {Reduce, 7}, {Reduce, 7}, {}, {Reduce, 7}, {Reduce, 7} }},
			/*6 */	{{ {Shift, 5}, {}, {}, {}, {}, {Shift, 4}, {}, {} }},
			/*7 */	{{ {Shift, 5}, {}, {}, {}, {}, {Shift, 4}, {}, {} }},
			/*8 */	{{ {Shift, 5}, {}, {}, {}, {}, {Shift, 4}, {}, {} }},
			/*9 */	{{ {Shift, 5}, {}, {}, {}, {}, {Shift, 4}, {}, {} }},
			/*10*/	{{ {}, {Shift, 6}, {Shift, 7}, {}, {}, {}, {Shift, 15}, {} }},
			/*11*/	{{ {}, {Reduce, 0}, {Reduce, 0}, {Shift, 8}, {Shift, 9}, {}, {Reduce, 0}, {Reduce, 0} }},
			/*12*/	{{ {}, {Reduce, 1}, {Reduce, 1}, {Shift, 8}, {Shift, 9}, {}, {Reduce, 1}, {Reduce, 1} }},
			/*13*/	{{ {}, {Reduce, 3}, {Reduce, 3}, {Reduce, 3}, {Reduce, 3}, {}, {Reduce, 3}, {Reduce, 3} }},
			/*14*/	{{ {}, {Reduce, 4}, {Reduce, 4}, {Reduce, 4}, {Reduce, 4}, {}, {Reduce, 4}, {Reduce, 4} }},
			/*15*/	{{ {}, {Reduce, 6}, {Reduce, 6}, {Reduce, 6}, {Reduce, 6}, {}, {Reduce, 6}, {Reduce, 6} }}
			} };

	std::array<std::array<int, 3>, 16> go{ {
			/*0*/	{{1, 2, 3}},
			/*1*/	{{}},
			/*2*/	{{}},
			/*3*/	{{}},
			/*4*/	{{10, 2, 3}},
			/*5*/	{{}},
			/*6*/	{{0, 11, 3}},
			/*7*/	{{0, 12, 3}},
			/*8*/	{{0, 0, 13}},
			/*9*/	{{0, 0, 14}},
			/*10*/	{{}},
			/*11*/	{{}},
			/*12*/	{{}},
			/*13*/	{{}},
			/*14*/	{{}},
			/*15*/	{{}}
			} };

	std::stack<Token> symbols;
	std::stack<int> states;


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
		case L'T': return T;
		case L'F': return F;
		default: return invalidNonTerminator;
		}
	}

	//Next four functions are used for formatted output 

	void PrintTerminator(Terminator t, std::streamsize w = 12) {
		switch (t)
		{
		case SLR1::num:   std::wcout << std::setw(w) << L"num";
			break;
		case SLR1::add:   std::wcout << std::setw(w) << L'+';
			break;
		case SLR1::sub:   std::wcout << std::setw(w) << L'-';
			break;
		case SLR1::mul:   std::wcout << std::setw(w) << L'*';
			break;
		case SLR1::divi:  std::wcout << std::setw(w) << L'/';
			break;
		case SLR1::lpar:  std::wcout << std::setw(w) << L'(';
			break;
		case SLR1::rpar:  std::wcout << std::setw(w) << L')';
			break;
		case SLR1::dollar:std::wcout << std::setw(w) << L'$';
			break;
		case SLR1::epsilon:std::wcout << std::setw(w) << L'¦Å';
			break;
		default:
			break;
		}
	}

	void PrintNonTerminator(NonTerminator nt, std::streamsize w = 12) {
		switch (nt)
		{
		case SLR1::E: std::wcout << std::setw(w) << L'E';
			break;
		case SLR1::T: std::wcout << std::setw(w) << L'T';
			break;
		case SLR1::F: std::wcout << std::setw(w) << L'F';
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


	void Parse(std::wstring tokens) {
		std::wcout << std::left;

		symbols.push(dollar);
		states.push(0);

		tokens += L'$';//add end token to the symbol string
		auto it = tokens.cbegin();
		do {
			auto top = symbols.top();
			auto ter = MapWchar2Terminator(*it);
			auto state = states.top();

			if (action[state][ter].first == Shift) {
				states.push(action[state][ter].second);
				symbols.push(ter);
				it++;
			}
			else if (action[state][ter].first == Reduce) {
				int index = action[state][ter].second;
				auto production = productions[index];
				int length = production.second.size();

				for (int i = 0; i < length; i++) {
					symbols.pop();
					states.pop();
				}

				state = states.top();
				int newState = go[state][production.first];
				states.push(newState);
				symbols.push(production.first);

				PrintProduction(production);
				std::wcout << std::endl;
			}
			else if (action[state][ter].first == Accept) {
				return;
			}
			else {
				//handle error
			}

		} while (true);
	}

}
