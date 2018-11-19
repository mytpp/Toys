#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <array>


namespace {
	enum NonTerminator : uint8_t
	{
		E = 0,
		e = 1,
		T = 2,
		t = 3,
		F = 4
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
		epsilon = 8 // ¦Å (not terminator actually)
	};

	//empty string means the production is null
	using Production = std::pair<NonTerminator, std::string>;

	const std::array<Production, 10> productions{ {
		{E, "Te"},
		{e, "+Te"},
		{e, "-Te"},
		{e, "¦Å"},
		{T, "Ft"},
		{t, "*Ft"},
		{t, "/Ft"},
		{t, "¦Å"},
		{F, "(E)"},
		{F, "n"}   //n stands for num
	} };

	//the FIRST set
	const std::array<std::vector<Terminator>, 5> first{ {
		{ lpar, num },
		{ add, sub, epsilon },
		{ lpar, num },
		{ mul, divi, epsilon },
		{ lpar, num }
	} };

	//the FOLLOW set
	const std::array<std::vector<Terminator>, 5> follow{ {
		{ dollar, rpar },
		{ dollar, rpar },
		{ dollar, rpar, add, sub },
		{ dollar, rpar, add, sub },
		{ dollar, rpar, add, sub, mul, divi }
	} };

	//null Production means error
	std::array<std::array<Production, 8>, 5> table{};
}

std::vector<Terminator> FirstOf(std::string tokens) {

	return{};
}



void GenerateLL1Table() {
	for (auto& production : productions) {
		for (auto terminator : FirstOf(production.second)) {
			if (terminator != epsilon)
				table[production.first][terminator] = production;
			else
				for(auto& terminator:follow[production.first])
					table[production.first][terminator] = production;
		}
	}
}

