#include <locale>
#include <iostream>
#include "LL1.h"
#include "LR1.h"


void PrintSeparator(std::wstring info, int length) {
	std::wcout << std::wstring(length, L'*') << info 
		<< std::wstring(length, L'*') << std::endl;
}

int main() {
	setlocale(LC_ALL, "chs");

	PrintSeparator(L"LL1", 40);
	PrintSeparator(L" LL1 table ", 15);
	LL1::GenerateLL1Table();
	LL1::PrintLL1Table();
	PrintSeparator(L"n+n*n", 10);
	LL1::Parse(L"n+n*n");
	PrintSeparator(L"(n-n)/n", 10);
	LL1::Parse(L"(n-n)/n");

	PrintSeparator(L"SLR1", 40);
	PrintSeparator(L"n+n*n", 10);
	SLR1::Parse(L"n+n*n");
	PrintSeparator(L"(n-n)/n", 10);
	SLR1::Parse(L"(n-n)/n");

	system("pause");
	return 0;
}


