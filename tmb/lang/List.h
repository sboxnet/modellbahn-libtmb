/***************************************************************************
 *   Copyright (C) 2000 - 2005 by Thomas Maier                             *
 *   balagi@justmail.de                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef _TMB_LANG_LIST_H_
#define _TMB_LANG_LIST_H_


#include <tmb/lang/tmb.h>


namespace tmb {


template<class T>
class List
{
	private: int _size;

	public:  class Element
			 {
				public: Element* _next;
				public: Element* _prev;
				public: T		 _data;

				public: explicit Element() {}

				public: explicit Element(const T& a_data) : _data(a_data) {}

				public: const Element* next() const throw() { return _next; }
				public: const Element* prev() const throw() { return _prev; }
				public: const T& data() const throw() { return _data; }
			 };

	private: Element* _root;
	private: mutable Element* _curp;
	private: mutable int _curindex;


	// iterators
	public:	const Element* iterator() const throw()
			{
				return _root;
			}


	protected: int isize() const throw() { return _size; }

	public: int size() const throw() { return _size; }

	private: void idestroyData() throw();

	private: void icopyData(const List<T>& a_list) throw();


	public: explicit List() throw() : _size(0), _root(0),
							 _curp(0), _curindex(0)
			{
			}

	public: List(const List<T>& a_list) throw()
			{
				icopyData(a_list);
			}

	public: const List<T>& operator=(const List<T>& a_list) throw();

	public: ~List() throw() { idestroyData(); }

	protected: Element* igetDataElementAt(int a_index) const throw();

	public: T& get(int a_index) const throw();

	public: T& operator[](int a_index) const throw()
			{
				return get(a_index);
			}

	public: T& getFirst() const throw() { return get(0); }

	public: T& getLast() const throw() { return get(_size - 1); }


	public: void put(const T& a_t, int a_index) throw();

	public: void putFirst(const T& a_t) throw() { put(a_t, 0); }

	public: void putLast(const T& a_t) throw() { put(a_t, _size - 1); }


	public: void insert(const T& a_t, int a_index) throw();

	public: void insertFirst(const T& a_t) throw() { insert(a_t, 0); }

	public: void insertLast(const T& a_t) throw() { insert(a_t, _size); }

	public: void append(const T& a_t) throw() { insert(a_t, _size); }

	public: void add(const T& a_t) throw() { insert(a_t, _size); }


	public: void remove(int a_start, int a_end) throw();


	public: void remove(int a_index) throw();

	public: void removeFirst() throw() { remove(0); }

	public: void removeLast() throw() { remove(_size - 1); }


	public: int indexOf(const T& a_t, int a_start = 0) const throw();


	public: void truncate(int a_size) throw();


	public: T getAndRemove(int a_index) throw()
			{
				T val = get(a_index);
				remove(a_index);
				return val;
			}


	public: int lastIndexOf(const T& a_t) const throw()
			{
				int pos = -1;
				for(;;)
				{
					int i = indexOf(a_t, pos + 1);
					if ( i < 0 )
						break;
					pos = i;
				}
				return pos;
			}

	public: bool contains(const T& a_t) const throw()
			{
				return (indexOf(a_t) >= 0);
			}


	public:  void clear() throw() { truncate(0); }


	public: bool isEmpty() const throw()
			{
				return _size == 0;
			}
};


template<class T>
inline void List<T>::idestroyData() throw()
{
 	while ( _root != 0 )
 	{
 	 	Element* elem = _root;
 	 	_root = _root->_next;
 	 	delete elem;
 	}
}

template<class T>
void List<T>::icopyData(const List<T>& a_list) throw()
{
 	_size = a_list.isize();
 	_root = 0;
 	Element* src = a_list._root;
 	Element* prev = 0;
	_curp = 0;
 	while ( src != 0 )
 	{
 		Element* elem = new Element(src->_data);
 		elem->_next = 0;
		elem->_prev = prev;
 		if ( prev == 0 )
 			_root = elem;
 		else
 			prev->_next = elem;
		if ( src == a_list._curp )
			_curp = elem;
 		prev = elem;
 		src = src->_next;
 	}
 	_curindex = a_list._curindex;
}


template<class T>
inline const List<T>& List<T>::operator=(const List<T>& a_list) throw()
{
 	if ( this != &a_list )
 	{
 	 	idestroyData();
 	 	icopyData(a_list);
 	}
	return *this;
}


template<class T>
typename List<T>::Element* List<T>::igetDataElementAt(int a_index) const throw()
{
	Element* elem;
	if ( a_index == _curindex ) // some fast special handlings ;)
		return _curp;
	else if ( a_index == (_curindex + 1))
		elem = _curp->_next;
	else if ( a_index == (_curindex - 1))
		elem = _curp->_prev;
	else if ( a_index == 0 )
		elem = _root;
	else
	{
		elem = _curp; // case a_index == c_curindex
		if ( a_index > _curindex )
		{
			int index = a_index - _curindex;
			while ( --index >= 0 )
				elem = elem->_next;
		}
		else if ( a_index > (_curindex/2) )
		{
			int index = a_index - _curindex;
			while ( ++index <= 0 )
				elem = elem->_prev;
		}
		else
		{
			elem = _root;
			int index = a_index;
			while ( --index >= 0 )
				elem = elem->_next;
		}
	}
	_curindex = a_index;
	_curp = elem;
 	return elem;
}

template<class T>
inline T& List<T>::get(int a_index) const throw()
{
 	return igetDataElementAt(a_index)->_data;
}

template<class T>
inline void List<T>::put(const T& a_t, int a_index) throw()
{
 	igetDataElementAt(a_index)->_data = a_t;
}

template<class T>
void List<T>::insert(const T& a_t, int a_index) throw()
{
	Element *n = new Element(a_t);
	if ( _size == 0 )
	{
		n->_next = 0;
		n->_prev = 0;
		_root = n;
		_curindex = 0;
		_curp = n;
	}
	else if ( a_index <= 0 )
	{
		n->_next = _root;
		n->_prev = 0;
		_root->_prev = n; // note: c_root always != 0 here! see above "if"
		_root = n;
		_curindex = 0;
		_curp = n;
	}
	else
	{
		if ( a_index > _size )
			a_index = _size;

		Element *elem = igetDataElementAt(a_index - 1); // get the previous element

		n->_next = elem->_next;
		n->_prev = elem;
		if ( elem->_next )
			elem->_next->_prev = n;
		elem->_next = n;

		_curindex = a_index;
		_curp = n;
	}

	_size++;
}

template<class T>
void List<T>::remove(int a_start, int a_end) throw()
{
	if ( (a_start < 0) || (a_start >= _size) )
 		return;
 	if ( (a_end < 0) || (a_end > _size) )
 		a_end = _size;
 	if ( a_end <= a_start )
 		return;

 	int len = a_end - a_start;

	while ( --len >= 0 )
	{
		remove(a_start);
	}
}

template<class T>
void List<T>::remove(int a_index) throw()
{
	if ( a_index < 0 || a_index >= _size )
		return;

	Element *elem = igetDataElementAt(a_index);

	if ( elem->_next != 0 )
		elem->_next->_prev = elem->_prev;
	if ( elem->_prev != 0 )
		elem->_prev->_next = elem->_next;
	else
		_root = elem->_next;

	if ( elem->_next != 0 )
	{
		_curindex = a_index;
		_curp = elem->_next;
	}
	else
	{
		_curindex = a_index - 1;
		_curp = elem->_prev;
	}

	_size--;

	delete elem;
}

template<class T>
int List<T>::indexOf(const T& a_t, int a_start) const throw()
{
 	if ( a_start < 0 )
 		a_start = 0;
 	else if ( a_start >= _size )
 		return -1;

 	Element* elem = igetDataElementAt(a_start);
 	while ( elem != 0 )
 	{
 	 	if ( elem->_data == a_t )
 	 		return a_start;
 	 	a_start++;
 	 	elem = elem->_next;
 	}
 	return -1;
}

template<class T>
inline void List<T>::truncate(int a_size) throw()
{
	if ( (a_size >= 0) && (a_size < _size) )
	{
		remove(a_size, _size);
	}
}


} // namespace


#endif
