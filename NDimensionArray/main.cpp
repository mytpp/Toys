/*****************************************************************************************
Developer: Peipei Tan (mytpp)
Update: 2018-8

A simple test and example uses for template NDimArray
*****************************************************************************************/

#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include "NDimArray.h"
#include <utility>

using namespace std;
using namespace mytpp;

//////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
	//start a scope for testing memery leak
	{
		NDimArray<int, 3, 4> a;
		//NDimArray<int, 1.5> b;//compile error: argument of type "double" is incompatible 
								//with template parameter of type "size_t"
		//NDimArray<int> c;	    //compile error: too few arguments for class template "NDimArray"
		NDimArray<double, 2, 3, 4> d;
		NDimArray<bool, 2, 2, 2, 2, 2, 2> e;
		NDimArray<long long, 100, 100> bigArray;
		NDimArray<int, 3, 4> copyImage(a);                  //test copy constructor
		NDimArray<double, 2, 3, 4> moveImage(std::move(d)); //test move constructor

		//test for member function NDimArray<..>::dim()
		assert(a.dim() == 2);
		assert(d.dim() == 3);
		assert(e.dim() == 6);

		//test for member function NDimArray<..>::size_of_dim(size_t)
		assert(d.size_of_dim(0) == 2);
		assert(d.size_of_dim(1) == 3);
		assert(d.size_of_dim(2) == 4);

		//test for member function NDimArray<..>::size()
		assert(a.size() == 12);
		assert(d.size() == 24);
		assert(e.size() == 64);
		assert(bigArray.size() == 10000);


		//test for NDimArray<..>::operator[]
		//assgin some initial value to the array
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 4; j++) {
				a[i][j] = i + j;
				moveImage[0][i][j] = static_cast<double>(i + j) / 2;
				moveImage[1][i][j] = static_cast<double>(i + j) / 3;
			}

		//test copy assignment operator
		copyImage = a;

		//test move assignment operator
		d = std::move(moveImage);

		//test for correctness
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 4; j++) {
				assert(copyImage[i][j] == i + j);
				assert(d[0][i][j] == static_cast<double>(i + j) / 2);
				assert(d[1][i][j] == static_cast<double>(i + j) / 3);
			}
	}//end the scope

#ifdef _MSC_VER
	_CrtDumpMemoryLeaks();
#endif

	//system("pause");
	return 0;
}
