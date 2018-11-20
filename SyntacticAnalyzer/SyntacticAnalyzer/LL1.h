#pragma once

#include <vector>
#include <string>
#include <utility>
#include <array>


namespace LL1 {
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

	//Empty string means the production is null
	//As '¦Å' is not included in ASCII, we use wstring to represent the symbol string
	using Production = std::pair<NonTerminator, std::wstring>;


	/**********************PUBLIC fUNCTION**************************/

	void GenerateLL1Table();

	void PrintLL1Table();

	void Parse(std::wstring tokens);
}


