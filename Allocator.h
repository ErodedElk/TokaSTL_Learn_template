#ifndef _TokaALLOCATOR_
#define _TokaALLOCATOR_
#include<new>
#include<cstddef>
#include<cstdlib>
#include<climits>
#include<iostream>
#include"Alloc.h"
namespace TokameinE{

	//ø’º‰≈‰÷√∆˜
	template<class T>
	class allocator {
	public:
		typedef T value_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef size_t		size_type;
		typedef ptrdiff_t	difference_type;

		template<class U>
		struct rebind {
			typedef allocator<U> other;
		};


	public:
		static T* allocate();
		static T* allocate(size_t n);
		static void deallocate(T* ptr);
		static void deallocate(T* ptr, size_t n);

		static void construct(T* ptr);
		static void construct(T* ptr, const T& value);
		static void destroy(T* ptr);
		static void destroy(T* first, T* last);

	};

	template<class T>
	T* allocator<T>::allocate(size_t size)
	{	
		if (size == 0)return 0;
		T* tmp = (T*)(::operator new((size_t)(size * sizeof(T))));
		if (tmp == 0)
		{
			std::cerr << "out of memory.";
			exit(1);
		}
		return tmp;
	}
	template<class T>
	T* allocator<T>::allocate()
	{
		T* tmp = (T*)(::operator new((size_t)(sizeof(T))));
		return tmp;
	}

	template<class T>
	void allocator<T>::deallocate(T* buffer)
	{
		::operator delete(buffer);
	}
	template<class T>
	void allocator<T>::deallocate(T* ptr, size_t n) {
		if (n == 0) return;
		alloc::deallocate(static_cast<void*>(ptr), sizeof(T) * n);
	}


	template<class T>
	void allocator<T>::construct(T* p)
	{
		new(p) T();
	}
	template<class T>
	void allocator<T>::construct(T* p,const T& value)
	{
		new(p) T(value);
	}
	template<class T>
	void allocator<T>::destroy(T* ptr)
	{
		ptr->~T();
	}
	template<class T>
	void allocator<T>::destroy(T* ptr,T* last)
	{
		for(;ptr!=last;ptr++)
			ptr->~T();
	}
}

#endif

