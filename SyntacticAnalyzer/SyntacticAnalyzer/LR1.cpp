#include "LR1.h"
#include <vector>
#include <array>
#include <stack>

namespace SLR1 {
	enum ActionCategory
	{
		Shift,
		Reduce,
		Accept
	};

	using Action = std::pair<ActionCategory, int>;

	/**************************************VARIABLE***************************************/
	const std::array<Production, 8> productions={{
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
	std::array<std::array<Production, 3>, 16> go;

	std::stack<Token> symbols;
	std::stack<int> states;


	/**********************************fUNCTION*************************************/
	 
}
