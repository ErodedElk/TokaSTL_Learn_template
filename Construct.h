#ifndef _TOKACONSTRUCT_H_
#define _TOKACONSTRUCT_H_
#include<new.h>
#include "TypeTraits.h"

namespace TokameinE {
	template<class T1,class T2>
	inline void construct(T1* ptr, const T2& value) {
		new(ptr) T1(value);
	}
	template<class T>
	inline void destroy(T* ptr)
	{
		ptr->~T();
	}
	template<class ForwardIterator>
	inline void destory(ForwardIterator first,ForwardIterator last)
	{
		typedef typename __type_traits<ForwardIterator>::is_POD_type is_POD_type;
		_destory_aux(first,last, is_POD_type());
	}

	template<class ForwardIterator>
	inline void _destory_aux(ForwardIterator first, ForwardIterator last, _true_type) {}

	template<class ForwardIterator>
	inline void _destroy_aux(ForwardIterator first, ForwardIterator last, _false_type) {
		for (; first != last; ++first) {
			destroy(&*first);//https://www.quora.com/What-does-a-*A-mean-in-C++-in-which-A-is-a-pointer
		}
	}
}
#endif