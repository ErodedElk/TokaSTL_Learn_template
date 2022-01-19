#ifndef _UNINITIALIZED_FUNCTIONS_H_
#define _UNINITIALIZED_FUNCTIONS_H_

//#include "Algorithm.h"
#include "Construct.h"
//#include "Iterator.h"
#include "TypeTraits.h"
#include<string.h>
#include"Iterator.h"

namespace TokameinE {

	/***************************************************************************/
	template<class InputIterator, class ForwardIterator>
	ForwardIterator _uninitialized_copy_aux(InputIterator first, InputIterator last,
		ForwardIterator result, _true_type);
	template<class InputIterator, class ForwardIterator>
	ForwardIterator _uninitialized_copy_aux(InputIterator first, InputIterator last,
		ForwardIterator result, _false_type);

	template<class InputIterator, class ForwardIterator>
	ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
		typedef typename __type_traits<iterator_traits<InputIterator>::value_type>::is_POD_type isPODType;
		return _uninitialized_copy_aux(first, last, result, isPODType());
	}
	template<class InputIterator, class ForwardIterator>
	ForwardIterator _uninitialized_copy_aux(InputIterator first, InputIterator last,
		ForwardIterator result, _true_type) {
		//memcpy(result, first, (last - first) * sizeof(*first));
		//.return result + (last - first);
		return std::copy(first, last, result);//使用 STL 版本的 copy 函数
	}
	template<class InputIterator, class ForwardIterator>
	ForwardIterator _uninitialized_copy_aux(InputIterator first, InputIterator last,
		ForwardIterator result, _false_type) {
		/*
		int i = 0;
		for (; first != last; ++first, ++i) {
			construct((result + i), *first);
		}
		return (result + i);*/
		ForwardIterator cur = result;
		for (; first != last; ++first, ++cur)
			construct(&*cur, *first);
		return cur;
	}
	//wchar_t 与 char 较为特殊,直接复制内存是最效率的做法
	inline char* uninitialized_copy(const char* first, const char* last, char* result) {
		//Toka: memcpy 也能起效,但如果出现空间重叠,memcpy会出错
		memmove(result,first,last-first);
		return result + (last - first);//Toka: 这个括号竟然是必须的......
	}
	inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
		memmove(result, first, last - first);
		return result + (last - first);
	}


	/***************************************************************************/
	template<class ForwardIterator, class T>
	void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
		const T& value, _true_type);
	template<class ForwardIterator, class T>
	void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
		const T& value, _false_type);

	template<class ForwardIterator, class T>
	void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& value) {
		typedef typename __type_traits<T>::is_POD_type isPODType;
		_uninitialized_fill_aux(first, last, value, isPODType());
	}
	template<class ForwardIterator, class T>
	void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
		const T& value, _true_type) {
		fill(first, last, value);//调用 STL 算法
	}
	template<class ForwardIterator, class T>
	void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
		const T& value, _false_type) {
		for (; first != last; ++first) {
			construct(&*first, value);
		}
	}




	/***************************************************************************/
	template<class ForwardIterator, class Size, class T>
	ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first,
		Size n, const T& x, _true_type);
	template<class ForwardIterator, class Size, class T>
	ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first,
		Size n, const T& x, _false_type);

	template<class ForwardIterator, class Size, class T>
	inline ForwardIterator uninitialized_fill_n(ForwardIterator first,
		Size n, const T& x) {
		typedef typename __type_traits<T>::is_POD_type isPODType;
		return _uninitialized_fill_n_aux(first, n, x, isPODType());
	}
	template<class ForwardIterator, class Size, class T>
	ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first,
		Size n, const T& x, _true_type) {
		return std::fill_n(first, n, x);//交由高阶函数执行
		
	}
	template<class ForwardIterator, class Size, class T>
	ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first,
		Size n, const T& x, _false_type) {
		/*
		for (int i = 0; i != n; ++i) {
			construct((T*)(first + i), x);
		}
		return (first + i);
		*/
		ForwardIterator cur=first;
		//Toka: 写成前置式是有意义的,不应该写成 cur++ 或 n-- ,参见<Eff C++>
		//为说明这点的重要性,因此采用下面的写法
		//另外,此处也省略了异常处理: https://www.mzwu.com/article.asp?id=1469
		for (; n > 0; ++cur, --n)
		{
			construct(&*cur, x);
			//Toka: (&*这一写法意味着传入右值)
			//http://c.biancheng.net/view/291.html
			//https://docs.microsoft.com/zh-cn/cpp/c-language/l-value-and-r-value-expressions?view=msvc-170
			//https://www.internalpointers.com/post/understanding-meaning-lvalues-and-rvalues-c
		}
		return cur;
		//Toka: 显然, cur 被增加了一个,道理上似乎已经超出范畴了
		//但在设计迭代器(iterator)的时候,会特意为其增加一个末尾
		//此处的 cur 指向末尾
		//这样的设计或许看起来增加了些许空间消耗,但往往对于算法效率却能有巨大的提升
	}
}

#endif
