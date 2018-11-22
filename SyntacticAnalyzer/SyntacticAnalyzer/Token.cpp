#include "Token.h"


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