#include <locale>
#include "LL1.h"
#include "LR1.h"


int main() {
	setlocale(LC_ALL, "chs");

	//LL1::GenerateLL1Table();
	//LL1::PrintLL1Table();
	//LL1::Parse(L"n+n*n");

	SLR1::Parse(L"n+n*n");

	system("pause");
	return 0;
}






