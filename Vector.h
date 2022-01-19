#ifndef _TokaVector_H_
#define _TokaVector_H_

#include "Allocator.h"
#include "UninitializedFunctions.h"

#include <algorithm>
#include <type_traits>
#include <utility>

namespace TokameinE {

	template<class T,class Alloc=allocator<T>>
	class vector {
	private:
		T* start;
		T* finish;
		T* endOfStorage;
		typedef Alloc dataAllocator;

	public:
		typedef T value_type;
		typedef T* iterator;
		typedef const T* const_iterator;
		typedef iterator pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t	difference_type;
	
	public:
		vector() :start(0), finish(0), endOfStorage(0) {};
		explicit vector(const size_type);
		vector(const size_type n, const value_type& value);
		template<class InputIterator>
		vector(InputIterator first, InputIterator last);
		vector(vector&& v);
		vector& operator=(const vector& v);
		vector& operator=(vector&& v);
		~vector();


		bool operator==(const vector& v)const;
		//const函数不能修改成员/const对象不能访问非const函数
		bool operator!=(const vector&)const;

		iterator begin() { return (start); }
		const_iterator begin() const { return (start); }
		iterator end() { return (finish); }
		const_iterator end() const { return (finish); }

		difference_type size()const { return(finish - start); }
		difference_type capacity() { return (endOfStorage - start); }
		bool empty()const { return (start == finish); }
		void resize(size_type n, value_type val = value_type());
		void reserve(size_type n);
		void shrink_to_fit();


		void clear();
		void swap(vector& v);
		void push_back(const value_type& value);
		void pop_back();

		iterator insert(iterator position, const value_type& val);
		void insert(iterator position, const size_type& n, const value_type& val);
		template <class InputIterator>
		void insert(iterator position, InputIterator first, InputIterator last);

		iterator erase(iterator position);
		iterator erase(iterator first, iterator last);

		Alloc get_allocator(){return dataAllocator;}

	private:
		void destoryVec();
		template<class InputIterator>
		void insert_aux(iterator position, InputIterator first, InputIterator last, std::false_type);
		template<class Integer>
		void insert_aux(iterator position, Integer n, const value_type& value, std::true_type);

		template<class InputIterator>
		void reallocateAndCopy(iterator position, InputIterator first, InputIterator last);
		void reallocateAndFillN(iterator position, const size_type& n, const value_type& val);
		size_type getNewCapacity(size_type len)const;


	};
	template<class T,class Alloc>
	vector<T, Alloc>::~vector() {
		destoryVec();
	}
	template<class T, class Alloc>
	void vector<T, Alloc>::destoryVec() {
		if (capacity() != 0) {
			dataAllocator::destroy(start, finish);
			dataAllocator::deallocate(start, capacity());
		}
	}






	template<class T,class Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator position) {
		return erase(position, position + 1);
	}
	template<class T, class Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator first,iterator last) {
		difference_type lenOfTail = end() - last;
		difference_type lenOfRemoved = last - first;
		finish = finish - lenOfRemoved;
		for (; lenOfRemoved != 0; --lenOfRemoved)
		{
			auto temp = last - lenOfRemoved;
			*temp = *(last++);
		}
		return first;
	}


	template<class T ,class Alloc>
	bool vector<T, Alloc>::operator!=(const vector& v)const {
		return !(*this == v);
	}
	template<class T, class Alloc>
	bool vector<T, Alloc>::operator==(const vector& v)const {
		if (size() != v.size())
		{
			return false;
		}
		else
		{
			auto ptr1 = start;
			auto ptr2 = v.start;
			for (; ptr1 != finish && ptr2 != v.finish; ++ptr1, ++ptr2)
			{
				if (*ptr1 != *ptr2)
					return false;
			}
			return true;
		}
	}

	template<class T, class Alloc>
	bool operator!=(const vector<T, Alloc>& v1, const vector<T, Alloc>& v2) {
		return !(v1 == v2);
	}
	template<class T, class Alloc>
	bool operator==(const vector<T, Alloc>& v1, const vector<T, Alloc>& v2) {
		return v1.operator==(v2);
	}

	template<class T, class Alloc>
	void vector<T, Alloc>::clear()
	{
		dataAllocator::destroy(start, finish);
		finish = start;
	}

	template<class T, class Alloc>
	void vector<T, Alloc>::swap(vector& v) {
		if (this != &v) {
			//TokameinE::swap(start, v.start);
			//do something
		}
	}
	template<class T, class Alloc>
	void vector<T, Alloc>::pop_back() {
		--finish;
		dataAllocator::destroy(finish);
	}


	template<class T, class Alloc>
	void vector<T, Alloc>::push_back(const value_type&value)
	{
		insert(end(), value);
	}

	template<class T, class Alloc>
	void vector<T, Alloc>::insert(iterator position, const size_type& n, const value_type& val) {
		insert_aux(position, n, val, typename std::is_integral<size_type>::type());
	}

	template<class T, class Alloc>
	template<class InputIterator>
	void vector<T, Alloc>::insert(iterator position, InputIterator first, InputIterator last) {
		insert_aux(position, first, last, typename std::is_integral<InputIterator>::type());
	}
	template<class T, class Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(iterator position, const value_type& val) {
		const auto index = position - begin();
		insert(position, 1, val);
		return begin() + index;
	}


	template<class T, class Alloc>
	template<class InputIterator>
	void vector<T, Alloc>::insert_aux(iterator position, InputIterator first, InputIterator last, std::false_type) {

		difference_type locationLeft = endOfStorage - finish;
		difference_type locationNeed = distance(first, last);
		if (locationLeft >= locationNeed)
		{
			if (finish - position > locationNeed) {
				TokameinE::uninitialized_copy(finish - locationNeed, finish,finish);
				std::copy_backward(position, finish - locationNeed, finish);
				std::copy(first, last, position);
			}
			else
			{
				iterator temp = TokameinE:: uninitialized_copy(first + (finish - position),last,finish);
				TokameinE::uninitialized_copy(position, finish, temp);
				std::copy(first, first + (finish - position), position);

			}
			finish += locationNeed;
		}
		else
		{
			reallocateAndCopy(position, first, last);
		}
	}
	template<class T, class Alloc>
	template<class Integer>
	void vector<T, Alloc>::insert_aux(iterator position, Integer n, const value_type& value, std::true_type) {
		//assert(n != 0);
		difference_type locationLeft = endOfStorage - finish; // the size of left storage
		difference_type locationNeed = n;

		if (locationLeft >= locationNeed) {
			auto tempPtr = end() - 1;
			for (; tempPtr - position >= 0; --tempPtr) {//move the [position, finish) back
				//*(tempPtr + locationNeed) = *tempPtr;//bug
				construct(tempPtr + locationNeed, *tempPtr);
			}
			TokameinE::uninitialized_fill_n(position, n, value);
			finish += locationNeed;
		}
		else {
			reallocateAndFillN(position, n, value);
		}
	}


	template<class T, class Alloc>
	void vector<T, Alloc>::reallocateAndFillN(iterator position, const size_type& n, const value_type& val) {
		difference_type newCapacity = getNewCapacity(n);

		T* newStart = dataAllocator::allocate(newCapacity);
		T* newEndOfStorage = newStart + newCapacity;
		T* newFinish = TokameinE::uninitialized_copy(begin(), position, newStart);
		newFinish = TokameinE::uninitialized_fill_n(newFinish, n, val);
		newFinish = TokameinE::uninitialized_copy(position, end(), newFinish);

		destoryVec();
		start = newStart;
		finish = newFinish;
		endOfStorage = newEndOfStorage;
	}

	template<class T, class Alloc>
	typename vector<T, Alloc>::size_type vector<T, Alloc>::getNewCapacity(size_type len)const {
		size_type oldCapacity = endOfStorage - start;
		auto res = (oldCapacity < len ? len : oldCapacity);
		size_type newCapacity = (oldCapacity != 0 ? (oldCapacity + res) : len);
		return newCapacity;
	}


	template<class T, class Alloc>
	template<class InputIterator>
	void vector<T, Alloc>::reallocateAndCopy(iterator position, InputIterator first, InputIterator last) {
		difference_type newCapacity = getNewCapacity(last - first);

		T* newStart = dataAllocator::allocate(newCapacity);
		T* newEndOfStorage = newStart + newCapacity;
		T* newFinish = TokameinE::uninitialized_copy(begin(), position, newStart);
		newFinish = TokameinE::uninitialized_copy(first, last, newFinish);
		newFinish = TokameinE::uninitialized_copy(position, end(), newFinish);

		destoryVec();
		start = newStart;
		finish = newFinish;
		endOfStorage = newEndOfStorage;
	}



}




#endif // !_TokaVector_H_
