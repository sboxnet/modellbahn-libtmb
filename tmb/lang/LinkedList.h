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


#ifndef _TMB_LANG_LINKEDLIST_H_
#define _TMB_LANG_LINKEDLIST_H_


#include <tmb/lang/tmb.h>


namespace tmb {

template<class T>
class LinkedList
{

			/// data structure that holds a list element
	public:  class Element
			 {
				protected: T*				_ll_obj;
				protected: Element*			_ll_next; ///< pointer to next element in list
				protected: Element*			_ll_prev; ///< pointer to prev element in list
				protected: LinkedList<T>*	_ll_parent;

				friend class LinkedList<T>;

				public: Element* next() const throw() { return _ll_next; }
				public: Element* prev() const throw() { return _ll_prev; }
				public: T*		 llobj() const throw() { return _ll_obj; }
				public: void     _ll_unlink() throw();

				public: explicit Element(T* a_obj) :  _ll_obj(a_obj), _ll_next(0), _ll_prev(0), _ll_parent(0) {}
				public: virtual ~Element() { _ll_unlink(); }

			 };


			/// the size of the list (number of stored elements)
	protected: int 		_size;
	protected: Element*	_first;
	protected: Element* _last;
	protected: int		_autodelete;

	public: void _ll_unlink(Element* a_elem) throw()
			{
				if ( a_elem->_ll_prev != 0 )
					a_elem->_ll_prev->_ll_next = a_elem->_ll_next;
				else
					_first = a_elem->_ll_next;

				if ( a_elem->_ll_next != 0 )
					a_elem->_ll_next->_ll_prev = a_elem->_ll_prev;
				else
					_last = a_elem->_ll_prev;

				a_elem->_ll_next = 0;
				a_elem->_ll_prev = 0;
				a_elem->_ll_parent = 0;

				_size--;
			}

	public: explicit LinkedList(int a_autodelete = 0) throw()
				: _size(0), _first(0), _last(0), _autodelete(a_autodelete)
			{
			}

	private: LinkedList(const LinkedList<T>&);
	private: void operator=(const LinkedList<T>&);

	public: ~LinkedList()
			{
				if ( _autodelete & AutoDelete_DESTRUCT )
				{
					Element* elem = _first;
					while ( elem != 0 )
					{
						T* t = elem->llobj();
						elem->_ll_parent = 0; // deactivate _ll_unlink()
						elem = elem->next();
						delete t;
					}
				}
				else
				{
					while ( _first )
						_ll_unlink(_first);
				}
			}

	public: int size() const throw() { return _size; }

	public: bool add(Element* a_elem) throw()
			{
				if ( a_elem == 0 )
					return false;

				if ( _last != 0 )
				{
					a_elem->_ll_next = 0;
					a_elem->_ll_prev = _last;
					a_elem->_ll_parent = this;
					_last->_ll_next = a_elem;
					_last = a_elem;
				}
				else
				{
					a_elem->_ll_next = 0;
					a_elem->_ll_prev = 0;
					a_elem->_ll_parent = this;
					_first = a_elem;
					_last = a_elem;
				}
				_size++;
				return true;
			}

	public: bool append(Element* a_elem) throw() { return add(a_elem); }

	public: bool addLast(Element* a_elem) throw() { return add(a_elem); }

	public: bool addFirst(Element* a_elem) throw()
			{
				if ( a_elem == 0 )
					return false;

				if ( _first != 0 )
				{
					a_elem->_ll_next = _first;
					a_elem->_ll_prev = 0;
					a_elem->_ll_parent = this;
					_first->_ll_prev = a_elem;
					_first = a_elem;
				}
				else
				{
					a_elem->_ll_next = 0;
					a_elem->_ll_prev = 0;
					a_elem->_ll_parent = this;
					_first = a_elem;
					_last = a_elem;
				}
				_size++;
				return true;
			}

	public: bool addAfter(Element* a_pos, Element* a_elem) throw()
			{
				if ( a_pos == 0 || a_elem == 0 || a_pos->_ll_parent != this )
					return false;

				a_elem->_ll_next = a_pos->_ll_next;
				a_elem->_ll_prev = a_pos;
				a_elem->_ll_parent = this;
				if ( a_pos->_ll_next != 0 )
					a_pos->_ll_next->_ll_prev = a_elem;
				else
					_last = a_elem;
				a_pos->_ll_next = a_elem;
				_size++;
				return true;
			}

	public: bool addBefore(Element* a_pos, Element* a_elem) throw()
			{
				if ( a_pos == 0 || a_elem == 0 || a_pos->_ll_parent != this )
					return false;

				a_elem->_ll_next = a_pos;
				a_elem->_ll_prev = a_pos->_ll_prev;
				a_elem->_ll_parent = this;
				if ( a_pos->_ll_prev != 0 )
					a_pos->_ll_prev->_ll_next = a_elem;
				else
					_first = a_elem;
				a_pos->_ll_prev = a_elem;
				_size++;
				return true;
			}

	public: bool remove(Element* a_elem) throw()
			{
				if ( a_elem == 0 || a_elem->_ll_parent != this )
					return false;

				_ll_unlink(a_elem);

				if ( _autodelete & AutoDelete_REMOVE )
				{
					delete a_elem->llobj();
				}
				return true;
			}

	public: Element* first() const throw() { return _first; }
	public: Element* last()  const throw() { return _last; }
	public:	Element* iterator() const throw() { return first(); }
	public: Element* revIterator() const throw() { return last(); }
	public: void removeFirst() throw() { remove(_first); }
	public: void removeLast()  throw() { remove(_last);  }


	public: bool contains(const Element* a_elem) const throw()
			{
				return (a_elem != 0 && a_elem->_ll_parent == this);
			}

	public: bool isEmpty() const throw()
			{
				return (_size == 0);
			}

	public: void clear()
			{
				while ( _first )
				{
					remove(_first);
				}
			}
};

template<class T>
inline void LinkedList<T>::Element::_ll_unlink() throw()
{
	if (_ll_parent)
		_ll_parent->_ll_unlink(this);
}

#define LLE(_t) LinkedList<_t>::Element
#define LLOBJ(_e) ((_e)->llobj())


} // namespace


#endif
