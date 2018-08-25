/******************************************************************
Developer: Peipei Tan (mytpp)
Update: 2018-8

A simple test and example uses for template NDimArray
*******************************************************************/

#include"NDimArray.h"

//////////////////////////////////////////////////////////////////


using namespace std;
using namespace mytpp;

int main()
{
	NDimArray<int, 3, 4> a;
	//NDimArray<int, 1.5> b;//compile error: argument of type "double" is incompatible 
	                        //with template parameter of type "size_t"
	//NDimArray<int> c;	    //compile error: too few arguments for class template "NDimArray"
	NDimArray<double, 2, 3, 4> d;
	NDimArray<long long, 100, 100> bigArray;

	//assgin some initial value to the array
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 4; j++) {
			a[i][j] = i + j;
			d[0][i][j] = static_cast<double>(i + j) / 2;
			d[1][i][j] = static_cast<double>(i + j) / 3;
		}

	//test for correctness
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 4; j++) {
			assert(a[i][j] == i + j);
			assert(d[0][i][j] == static_cast<double>(i + j) / 2);
			assert(d[1][i][j] == static_cast<double>(i + j) / 3);
		}

	return 0;
}
