/*****************************************************************************
Developer: Peipei Tan (mytpp)
Update: 2018-8

A implementation for n-dimension-array
*****************************************************************************/


#ifndef NDIMARRAY_H_
#define NDIMARRAY_H_


#include <cassert> 
#include <new>      //for placement new

namespace mytpp {

	//T: type of elements in the NDimArray
	//dim_size: size of current dimension
	//args: sizes of following dimensions
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


		//Return the dimension of this array
		size_t dim() const {
			return sizeof...(args) + 1;
		}

		//Return # of elements in dimension 'dim'.
		//Note that 'dim' starts from 0.
		size_t size_of_dim(size_t dim) const {
			assert(dim <= sizeof...(args) + 1);
			size_t tmp[] = { dim_size, args... };
			return tmp[dim];
		}

		//Return the total # of elements in this array
		size_t size() const {
			return value.size();
		}


		//Overload operator[] to allow for this kind of syntax:
		//NDimArray<int, 50, 100> a;  a[17][18] = 2333;
		//Return a proxy class which also overloads operator[]
		//The true element is returned at the partial specialization version
		const NDimArray<T, args...>&  operator[] (size_t index) const{
			assert(index < dim_size);
			value.setOffset(m_offset*dim_size + index);
			return value;
		}

		NDimArray<T, args...>&  operator[] (size_t index) {
			return const_cast< NDimArray<T, args...>& >(
				static_cast<const NDimArray&>(*this)[index]
				);
		}

		//This is only for implementation
		void setOffset(size_t offset) const {
			m_offset = offset;
		}


	private:
		mutable size_t m_offset;     //current offset
		NDimArray<T, args...> value; //proxy class
	};

	/*-----------------------------------------------------------------------------*/

	//Partial specialization of one dimension array to end recursion.
	//And this is the only place where we use 'new expression'.
	//Hence, we can ensure allocating memory only once when creating a NDimArray.
	template <typename T, size_t dim_size>
	class NDimArray<T, dim_size>
	{
	public:
		//default constructor
		NDimArray()
			: m_offset(0)
			, m_size(dim_size)
			, value(new T[dim_size])
		{ }

		//This is only for implementation
		NDimArray(size_t offset)
			: m_offset(offset)
			, m_size(dim_size*offset)
			, value(new T[dim_size*offset])
		{ }

		//copy constructor
		NDimArray(const NDimArray& rhs)
			:m_offset(rhs.m_offset)
			,m_size(rhs.m_size)
			,value(static_cast<T*>(
				::operator new(sizeof(T)*rhs.m_size)
				))
		{
			do_copy(value, rhs.value, rhs.m_size);
		}

		//copy assignment operator
		NDimArray& operator=(const NDimArray& rhs) {
			//Offer strong exception safety guarantee
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
			//make 'rhs' destructible
			rhs.value = nullptr;     
		}

		//move assignment operator
		NDimArray& operator=(NDimArray&& rhs) noexcept {
			if (this != &rhs) {
				m_offset = rhs.m_offset;
				m_size = rhs.m_size;
				value = rhs.value;

				//make 'rhs' destructible
				rhs.value = nullptr; 
			}
			return *this;
		}

		//destructor
		~NDimArray() {
			delete[] value;
		}


		size_t dim() const {
			return 1;
		}

		size_t size_of_dim(size_t dim) const {
			assert(dim == 0);
			return dim_size;
		}

		size_t size() const {
			return m_size;
		}


		//Return a real element of type 'T' at the calculated position
		const T& operator[] (size_t index) const {
			assert(index < dim_size);
			return value[m_offset*dim_size + index];
		}

		T& operator[] (size_t index) {
			return const_cast<T&>(
				static_cast<const NDimArray&>(*this)[index]
				);
		}

		//This is only for implementation
		void setOffset(size_t offset) const {
			m_offset = offset;
		}

	private:
		//Copy a block of memory from 'src' to 'dest'
		//There are 'length' elements of type 'T' in the memory block
		void do_copy(T* dest, T* src, size_t length) {
			for (size_t i = 0; i < length; i++, dest++, src++)
				::new (static_cast<void*>(dest)) T(*src);
		}

	private:
		mutable size_t m_offset;// the position of element returned by operator[]
		size_t m_size;          // # of elements in the array

		T* value;               // the memery block storing elements of type 'T'
	};
}

#endif // NDIMARRAY_H_
