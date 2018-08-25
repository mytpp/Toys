/******************************************************************
Developer: Peipei Tan (mytpp)
Update: 2018-8

A implementation for n-dimension-array
*******************************************************************/


#ifndef NDIMARRAY_H_
#define NDIMARRAY_H_


#include <cassert> 

namespace mytpp {

	template <typename T, size_t size, size_t... args>
	class NDimArray
	{
	public:
		NDimArray() : offset(0), value(size) { }

		//Don't use this constructor in user code
		//This is only for implementation
		NDimArray(size_t offset) : offset(offset), value(size*offset) {
			this->offset = 0;
		}

		//overload operator[] to allow for this kind of syntax:
		//NDimArray<int, 50, 100> a;
		//a[17][18] = 2333;
		NDimArray<T, args...>&  operator[] (size_t index) {
			assert(index < size);
			value.setOffset(offset*size + index);
			return value;
		}

		//this is only for implementation
		void setOffset(size_t offset) {
			this->offset = offset;
		}


	private:
		size_t offset;
		NDimArray<T, args...> value;
	};


	//partial specialization to end recursion
	template <typename T, size_t size>
	class NDimArray<T, size>
	{
	public:
		NDimArray()
			: offset(0)
			, value(new T[size])
		{ }

		NDimArray(size_t offset)
			: offset(offset)
			, value(new T[size*offset])
		{ }

		~NDimArray() {
			delete[] value;
		}

		T& operator[] (size_t index) {
			assert(index < size);
			return value[offset*size + index];
		}

		void setOffset(size_t offset) {
			this->offset = offset;
		}

	private:
		size_t offset;
		T* value;
	};
}

#endif // NDIMARRAY_H_
