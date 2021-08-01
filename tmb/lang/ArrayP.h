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


#ifndef _TMB_LANG_ARRAYP_H_
#define _TMB_LANG_ARRAYP_H_

#include <tmb/lang/Array.h>

namespace tmb {

template<class T>
class ArrayP : public Array<T*>
{
	private: typedef Array<T*> super;

	protected: int _autodelete;

	private: void iautoDelete(int a_start, int a_end) throw()
			{
				for ( ;  a_start < a_end; a_start++ )
				{
					delete super::idata()[a_start];
				}
			}


	public: explicit ArrayP() throw()
								: Array<T*>(), _autodelete(0)
			{
			}
	public: explicit ArrayP(AutoDelete a_autodelete) throw()
								: Array<T*>(), _autodelete(a_autodelete)
			{
			}


	public: explicit ArrayP(int a_bufsize) throw()
							: Array<T*>(a_bufsize), _autodelete(0)
			{
			}
	public: explicit ArrayP(int a_bufsize, AutoDelete a_autodelete) throw()
							: Array<T*>(a_bufsize), _autodelete(a_autodelete)
			{
			}

	public: explicit ArrayP(const T** a_data, int a_size, int a_minbufsize = 0, AutoDelete a_autodelete = 0) throw()
								: Array<T*>(a_data, a_size, a_minbufsize), _autodelete(a_autodelete)
			{
			}

	public: ArrayP(const ArrayP<T>& a_array, int a_minbufsize = 0) throw()
								: Array<T*>(a_array, a_minbufsize), _autodelete(0)
			{
			}

	public: const ArrayP<T>& operator=(const ArrayP<T>& a_array) throw();

	public: ~ArrayP() throw()
			{
				if ( _autodelete & AutoDelete_DESTRUCT )
					iautoDelete(0, super::isize());
			}

	public: bool isAutoDelete() const throw() { return (_autodelete != 0); }

	public: void setAutoDelete(AutoDelete a_on) throw() { _autodelete = a_on; }

	public: void remove(int a_start, int a_end) throw();

	public: void remove(int a_index) throw()
			{
				remove(a_index, a_index+1);
			}

	public: void removeFirst() throw() { remove(0); }

	public: void removeLast() throw() { remove(super::isize() - 1); }

	public: void remove(T* a_elem) throw()
			{
				int idx = indexOf(a_elem);
				if ( idx >= 0 )
					remove(idx, idx+1);
			}

	public: T* getAndRemove(int a_index) throw()
			{
				int autodelete = _autodelete;
				_autodelete = 0;
				T* val = super::getAndRemove(a_index);
				_autodelete = autodelete;
				return val;
			}

	public: void truncate(int a_size) throw();

	public: void clear() throw() { truncate(0); }

	public: void setSize(int a_size) throw();
	public: void setSize(int a_size, int a_bufsize) throw();

};

template<class T>
const ArrayP<T>& ArrayP<T>::operator=(const ArrayP<T>& a_array) throw()
{
	if ( this == &a_array )
		return *this;

	if ( _autodelete & AutoDelete_REMOVE )
		iautoDelete(0, super::isize());

	super::operator=(a_array);

	_autodelete = 0;

	return *this;
}


template<class T>
void ArrayP<T>::remove(int a_start, int a_end) throw()
{
 	if ( (a_start < 0) || (a_start >= super::isize()) )
 		return;
 	if ( (a_end < 0) || (a_end > super::isize()) )
 		a_end = super::isize();
 	if ( a_end <= a_start )
 		return;

	if ( _autodelete & AutoDelete_REMOVE )
		iautoDelete(a_start, a_end);

	super::remove(a_start, a_end);
}

template<class T>
void ArrayP<T>::truncate(int a_size) throw()
{
 	if ( (a_size >= 0) && (a_size < super::isize()) )
 	{
		if ( _autodelete & AutoDelete_REMOVE )
 			iautoDelete(a_size, super::isize());

		super::truncate(a_size);
 	}
}

template<class T>
inline void ArrayP<T>::setSize(int a_size) throw()
{
	if ( a_size < 0 )
		return;
 	if ( a_size > super::ibufsize() )
 	{
 		super::setMinBufSize(a_size);
 	}
	else if ( (_autodelete & AutoDelete_REMOVE) && (a_size < super::_size) )
	{
		iautoDelete(a_size, super::_size);
	}
 	super::_size = a_size;
}

template<class T>
inline void ArrayP<T>::setSize(int a_size, int a_bufsize) throw()
{
	if ( a_size < 0 )
		return;
	if ( a_bufsize < a_size )
		a_bufsize = a_size;
 	super::setMinBufSize(a_size);
	if ( (_autodelete & AutoDelete_REMOVE) && (a_size < super::_size) )
	{
		iautoDelete(a_size, super::_size);
	}
 	super::_size = a_size;
}


} // namespace

#endif
