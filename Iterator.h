#ifndef _TOKAITERATOR_
#define _TOKAITERATOR_

namespace TokameinE {
	typedef int ptrdiff_t;
	//五种迭代器类型
	struct input_iterator_tag {};
	struct output_iterator_tag {};
	struct forward_iterator_tag :public input_iterator_tag {};
	struct bidirectional_iterator_tag :public forward_iterator_tag {};
	struct random_access_iterator_tag :public bidirectional_iterator_tag {};

	template <class T, class Distance>
	struct input_iterator
	{
		typedef input_iterator_tag	iterator_category;
		typedef T					value_type;
		typedef Distance			difference_type;
		typedef T* pointer;
		typedef T& reference;
	};
	struct output_iterator
	{
		typedef output_iterator_tag iterator_category;
		typedef void                value_type;
		typedef void                difference_type;
		typedef void                pointer;
		typedef void                reference;
	};
	template <class T, class Distance>
	struct forward_iterator
	{
		typedef forward_iterator_tag	iterator_category;
		typedef T						value_type;
		typedef Distance				difference_type;
		typedef T* pointer;
		typedef T& reference;
	};
	template <class T, class Distance>
	struct bidirectional_iterator
	{
		typedef bidirectional_iterator_tag	iterator_category;
		typedef T							value_type;
		typedef Distance					difference_type;
		typedef T* pointer;
		typedef T& reference;
	};
	template <class T, class Distance>
	struct random_access_iterator
	{
		typedef random_access_iterator_tag	iterator_category;
		typedef T							value_type;
		typedef Distance					difference_type;
		typedef T* pointer;
		typedef T& reference;
	};

	template<class Category,class T,class Distance=int,class Pointer = T*,class Reference=T&>
	struct iterator {
		typedef Category iterator_category;
		typedef T value_type;
		typedef Distance difference_type;
		typedef Pointer pointer;
		typedef Reference reference;
	};


	template<class Iterator>
	struct  iterator_traits
	{
		typedef typename Iterator::iterator_category iterator_category;//迭代器类型
		typedef typename Iterator::value_type value_type;
		typedef typename Iterator::difference_type difference_type;//迭代器间距
		typedef typename Iterator::pointer pointer;
		typedef typename Iterator::reference reference;
	};

	template<class T>
	struct iterator_traits<T*>
	{
		typedef random_access_iterator_tag 	iterator_category;
		typedef T 							value_type;
		typedef ptrdiff_t 					difference_type;
		typedef T* pointer;
		typedef T& reference;
	};

	template<class T>
	struct iterator_traits<const T*>
	{
		typedef random_access_iterator_tag 	iterator_category;
		typedef T 							value_type;
		typedef ptrdiff_t 					difference_type;
		typedef const T* pointer;
		typedef const T& reference;
	};

	template<class Iterator>
	inline typename iterator_traits<Iterator>::iterator_category iterator_category(const Iterator& It)
	{
		typedef typename iterator_traits<Iterator>::iterator_category category;
		return category();

	};
	//queation
	template<class Iterator>
	inline typename iterator_traits<Iterator>::value_type* value_type(const Iterator& It)
	{
		return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
	};

	template<class Iterator>
	inline typename iterator_traits<Iterator>::difference_type*
		difference_type(const Iterator& It) {
		return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
	}
	///////////////
	//distance
	template<class InputIterator>
	inline typename iterator_traits<InputIterator>::difference_type _distance(InputIterator first, InputIterator last,input_iterator_tag) {
		typename iterator_traits<InputIterator>::difference_type n = 0;
		while (first != last)
		{
			++n; ++first;
		}
		return n;
	}
	template<class InputIterator>
	inline typename iterator_traits<InputIterator>::difference_type _distance(InputIterator first, InputIterator last, random_access_iterator_tag) {
		auto temp = last - first;
		return temp;
	}
	template<class InputIterator>
	inline typename iterator_traits<InputIterator>::difference_type distance(InputIterator first, InputIterator last) {
		typedef typename iterator_traits<InputIterator>::iterator_category Category;
		return _distance(first, last, Category());

	}

	/////////
	//advance
	template<class InputIterator,class Distance>
	inline void _advance(InputIterator&first, Distance n,input_iterator_tag)
	{
		while (n--)++first;
	}
	template<class InputIterator, class Distance>
	inline void _advance(InputIterator &first, Distance n, bidirectional_iterator_tag)
	{
		if (n >= 0)
			while (n--)++first;
		else
			while (n++)--first;
	}

	template<class InputIterator, class Distance>
	inline void _advance(InputIterator& first, Distance n, random_access_iterator_tag)
	{
		first += n;
	}

	template<class InputIterator, class Distance>
	inline void advance(InputIterator& first, Distance n)
	{
		typedef typename iterator_traits<InputIterator>::iterator_category Category;
		_advance(first, n, Category());
	};



}





#endif // !_TOKAITERATOR_
