#pragma once

#include <cassert> 

template <typename T, size_t size, size_t... args>
class NDimArray
{
public:
	NDimArray() : offset(0), value(size) { }

	//Don't use this constructor 
	//This is only for implementation
	NDimArray(size_t offset): offset(offset), value(size*offset) {
		this->offset = 0;
	}

	NDimArray<T, args...>&  operator[] (size_t index) {
		value.setOffset(offset*size + index);
		return value;
	}

	void setOffset(size_t offset) {
		this->offset = offset;
	}


private:
	size_t offset;
	NDimArray<T, args...> value;
};

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
		return value[offset*size + index];
	}

	void setOffset(size_t offset) {
		this->offset = offset;
	}

private:
	size_t offset;
	T* value;
};

