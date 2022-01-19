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
		return std::copy(first, last, result);//ʹ�� STL �汾�� copy ����
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
	//wchar_t �� char ��Ϊ����,ֱ�Ӹ����ڴ�����Ч�ʵ�����
	inline char* uninitialized_copy(const char* first, const char* last, char* result) {
		//Toka: memcpy Ҳ����Ч,��������ֿռ��ص�,memcpy�����
		memmove(result,first,last-first);
		return result + (last - first);//Toka: ������ž�Ȼ�Ǳ����......
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
		fill(first, last, value);//���� STL �㷨
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
		return std::fill_n(first, n, x);//���ɸ߽׺���ִ��
		
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
		//Toka: д��ǰ��ʽ���������,��Ӧ��д�� cur++ �� n-- ,�μ�<Eff C++>
		//Ϊ˵��������Ҫ��,��˲��������д��
		//����,�˴�Ҳʡ�����쳣����: https://www.mzwu.com/article.asp?id=1469
		for (; n > 0; ++cur, --n)
		{
			construct(&*cur, x);
			//Toka: (&*��һд����ζ�Ŵ�����ֵ)
			//http://c.biancheng.net/view/291.html
			//https://docs.microsoft.com/zh-cn/cpp/c-language/l-value-and-r-value-expressions?view=msvc-170
			//https://www.internalpointers.com/post/understanding-meaning-lvalues-and-rvalues-c
		}
		return cur;
		//Toka: ��Ȼ, cur ��������һ��,�������ƺ��Ѿ�����������
		//������Ƶ�����(iterator)��ʱ��,������Ϊ������һ��ĩβ
		//�˴��� cur ָ��ĩβ
		//��������ƻ�������������Щ��ռ�����,�����������㷨Ч��ȴ���о޴������
	}
}

#endif
