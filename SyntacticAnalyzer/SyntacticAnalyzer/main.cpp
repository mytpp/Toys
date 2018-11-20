#include <locale>
#include "LL1.h"


int main() {
	setlocale(LC_ALL, "chs");

	LL1::GenerateLL1Table();
	LL1::PrintLL1Table();


	system("pause");
	return 0;
}








