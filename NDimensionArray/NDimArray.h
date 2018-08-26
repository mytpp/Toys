/**********************************************************************
Developer: Peipei Tan (mytpp)
Update: 2018-8

A implementation for n-dimension-array
**********************************************************************/


#ifndef NDIMARRAY_H_
#define NDIMARRAY_H_


#include <cassert> 
#include <new>      //for placement new

namespace mytpp {

	template <typename T, size_t dim_size, size_t... args>
	class NDimArray
	{
	public:
		//default constructor
		NDimArray() : m_offset(0), value(dim_size) { }

		//Don't use this constructor in user code. This is only for implementation
		NDimArray(size_t offset) : m_offset(offset), value(dim_size*offset) { }

		//copy constructor and copy assignment operator
		NDimArray(const NDimArray&) = default;
		NDimArray& operator=(const NDimArray&) = default;

		//move constructor and move assignment operator
		NDimArray(NDimArray&&) = default;
		NDimArray& operator=(NDimArray&&) = default;


		size_t dim() {
			return sizeof...(args) + 1;
		}

		size_t size_of_dim(size_t dim) {
			assert(dim <= sizeof...(args) + 1);
			size_t tmp[] = { dim_size, args... };
			return tmp[dim];
		}

		size_t size() {
			return value.size();
		}


		//overload operator[] to allow for this kind of syntax:
		//NDimArray<int, 50, 100> a;  a[17][18] = 2333;
		NDimArray<T, args...>&  operator[] (size_t index) {
			assert(index < dim_size);
			value.setOffset(m_offset*dim_size + index);
			return value;
		}

		//this is only for implementation
		void setOffset(size_t offset) {
			m_offset = offset;
		}


	private:
		size_t m_offset;
		NDimArray<T, args...> value;
	};

	/*-------------------------------------------------------------------*/

	//partial specialization to end recursion and allocate memory
	template <typename T, size_t dim_size>
	class NDimArray<T, dim_size>
	{
	public:
		NDimArray()
			: m_offset(0)
			, m_size(dim_size)
			, value(new T[dim_size])
		{ }

		NDimArray(size_t offset)
			: m_offset(offset)
			, m_size(dim_size*offset)
			, value(new T[dim_size*offset])
		{ }

		//copy constructor and copy assignment operator
		NDimArray(const NDimArray& rhs)
			:m_offset(rhs.m_offset)
			,m_size(rhs.m_size)
			,value(static_cast<T*>(
				::operator new(sizeof(T)*rhs.m_size)
				))
		{
			do_copy(value, rhs.value, rhs.m_size);
		}

		NDimArray& operator=(const NDimArray& rhs) {
			T* old_memery = value;
			value = static_cast<T*>(
				::operator new(sizeof(T)*rhs.m_size));
			delete old_memery;
			do_copy(value, rhs.value, rhs.m_size);

			m_offset = rhs.m_offset;
			m_size = rhs.m_size;

			return *this;
		}

		//move constructor
		NDimArray(NDimArray&& rhs) noexcept 
			:m_offset(rhs.m_offset)
			,m_size(rhs.m_size)
			,value(rhs.value) 
		{ 
			rhs.value = nullptr; 
		}

		//move assignment operator
		NDimArray& operator=(NDimArray&& rhs) noexcept {
			if (this != &rhs) {
				m_offset = rhs.m_offset;
				m_size = rhs.m_size;
				value = rhs.value;
				rhs.value = nullptr;
			}
			return *this;
		}


		size_t dim() {
			return 1;
		}

		size_t size_of_dim(size_t dim) {
			assert(dim == 0);
			return dim_size;
		}

		size_t size() {
			return m_size;
		}

		~NDimArray() {
			delete[] value;
		}


		T& operator[] (size_t index) {
			assert(index < dim_size);
			return value[m_offset*dim_size + index];
		}

		//this is only for implementation
		void setOffset(size_t offset) {
			m_offset = offset;
		}

	private:
		void do_copy(T* dest, T* src, size_t length) {
			for (size_t i = 0; i < length; i++, dest++, src++)
				::new (static_cast<void*>(dest)) T(*src);
		}

	private:
		size_t m_offset;
		size_t m_size;

		T* value;
	};
}

#endif // NDIMARRAY_H_
