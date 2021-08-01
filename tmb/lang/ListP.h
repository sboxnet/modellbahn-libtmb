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



#ifndef _TMB_LANG_LISTP_H_
#define _TMB_LANG_LISTP_H_


#include <tmb/lang/List.h>


namespace tmb {

template<class T>
class ListP : public List<T*>
{
	private: typedef List<T*> super;

	protected: int _autodelete;

	private: void iautoDelete(int a_start, int a_end) throw()
			{
				if ( a_start < a_end )
				{
					typename super::Element *elem = super::igetDataElementAt(a_start);
					for ( ; a_start < a_end; a_start++ )
					{
						delete elem->_data;
						elem = elem->_next;
					}
				}
			}


	public: explicit ListP(AutoDelete a_autodelete = 0) throw()
								: List<T*>(), _autodelete(a_autodelete)
			{
			}


	public: ListP(const ListP<T>& a_list) throw()
							: List<T*>(a_list), _autodelete(0)
			{
			}

	public: const ListP<T>& operator=(const ListP<T>& a_list) throw();

	public: ~ListP() throw();


	public: bool isAutoDelete() const throw() { return (_autodelete != 0); }

	public: void setAutoDelete(AutoDelete a_on) throw() { _autodelete = a_on; }

	public: void remove(int a_start, int a_end) throw();


	public: void remove(int a_index) throw();

	public: void removeFirst() throw() { remove(0); }

	public: void removeLast() throw() { remove(super::isize() - 1); }

	public: void truncate(int a_size) throw();

	public:  void clear() throw() { truncate(0); }

	public: T* getAndRemove(int a_index) throw()
			{
				int autodelete = _autodelete;
				_autodelete = 0;
				T* val = super::getAndRemove(a_index);
				_autodelete = autodelete;
				return val;
			}

};


template<class T>
const ListP<T>& ListP<T>::operator=(const ListP<T>& a_list) throw()
{
	if ( this == &a_list )
		return *this;

	if ( _autodelete & AutoDelete_REMOVE )
		iautoDelete(0, super::isize());

	super::operator=(a_list);

	_autodelete = 0;

	return *this;
}

template<class T>
ListP<T>::~ListP() throw()
{
	if ( _autodelete & AutoDelete_DESTRUCT )
		iautoDelete(0, super::isize());
}

template<class T>
void ListP<T>::remove(int a_start, int a_end) throw()
{
	if ( (a_start < 0) || (a_start >= super::isize()) )
 		return;
 	if ( (a_end < 0) || (a_end > super::isize()) )
 		a_end = super::isize();
 	if ( a_end <= a_start )
 		return;

 	int len = a_end - a_start;

	while ( --len >= 0 )
	{
		remove(a_start);
	}
}

template<class T>
void ListP<T>::remove(int a_index) throw()
{
	if ( a_index < 0 || a_index >= super::isize() )
		return;

	if ( _autodelete & AutoDelete_REMOVE )
	{
		delete super::igetDataElementAt(a_index)->_data;
	}

	super::remove(a_index);
}

template<class T>
inline void ListP<T>::truncate(int a_size) throw()
{
	if ( (a_size >= 0) && (a_size < super::isize()) )
	{
		remove(a_size, super::isize());
	}
}


} // namespace


#endif
