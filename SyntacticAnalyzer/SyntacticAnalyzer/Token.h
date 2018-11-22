#pragma once

#include <utility>
#include <cstdint>
#include <string>

enum NonTerminator : uint8_t
{
	E = 0,
	e = 1,
	T = 2,
	t = 3,
	F = 4,
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

bool operator==(Token token, Terminator t);

bool operator==(Token token, NonTerminator nt);

bool  operator!=(Token token, Terminator t);

bool  operator!=(Token token, NonTerminator nt);
