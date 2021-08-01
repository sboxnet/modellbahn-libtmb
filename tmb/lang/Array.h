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


#ifndef _TMB_LANG_ARRAY_H_
#define _TMB_LANG_ARRAY_H_

#include <tmb/lang/tmb.h>

#include <typeinfo>

#include <iostream>
using namespace std;

namespace tmb {

template<class T>
class Array
{
	private: T*  _data;
	private: int _size;
	private: int _bufsize;

	private: enum { DEFAULT_BUFSIZE = 8 };

	protected: int isize() const throw() { return _size; }

	public: int size() const throw() { return _size; }

	protected: int ibufsize() const throw() { return _bufsize; }

	public: int bufSize() const throw() { return _bufsize; }

	public: int sizeBytes() const { return _size * sizeof(T); }
	public: int bufSizeBytes() const { return _bufsize * sizeof(T); }

	protected: T* idata() const throw() { return _data; }

	public: T* data() const throw() { return idata(); }

	private: void inewDataBuffer() throw()
			{
				_data = (_bufsize > 0 ? new T [_bufsize] : 0);
			}

	private: void icopyData(const T* a_data) throw()
			{
				for ( int i = 0; i < _size; i++ )
				 	_data[i] = a_data[i];
			}




	public: explicit Array() throw() : _data(0), _size(0), _bufsize(0)
			{
			}

	public: explicit Array(int a_bufsize) throw();

	public: explicit Array(int a_size, int a_bufsize) throw();

	public: explicit Array(const T* a_data, int a_size, int a_minbufsize = 0) throw();

	public: Array(const Array<T>& a_array, int a_minbufsize = 0) throw();

	public: const Array<T>& operator=(const Array<T>& a_array) throw();

	public: ~Array() throw()
			{
				delete [] _data;
			}

	public: T& get(int a_index) const { return _data[a_index]; }

	public: T& operator[](int a_index) const { return _data[a_index]; }

	public: T& getFirst() const throw() { return _data[0]; }

	public: T& getLast() const throw() { return _data[_size - 1]; }

	public: void put(const T& a_t, int a_index) throw();

	public: void putFirst(const T& a_t) throw() { put(a_t, 0); }

	public: void putLast(const T& a_t) throw() { put(a_t, _size - 1); }



	public: void insert(const T& a_t, int a_index) throw();

	public: void insertFirst(const T& a_t) throw() { insert(a_t, 0); }

	public: void insertLast(const T& a_t) throw() { insert(a_t, _size); }

	public: void append(const T& a_t) throw() { insert(a_t, _size); }

	public: void add(const T& a_t) throw() { insert(a_t, _size); }


	public: void remove(int a_start, int a_end) throw();


	public: void remove(int a_index) throw()
			{
				remove(a_index, a_index+1);
			}

	public: void removeFirst() throw() { remove(0); }

	public: void removeLast() throw() { remove(_size - 1); }


	public: void truncate(int a_size) throw();


	public: void setSize(int a_size) throw();
	public: void setSize(int a_size, int a_bufsize) throw();

	public: void setMinBufSize(int a_bufsize) throw();


	public: T getAndRemove(int a_index) throw()
			{
				T val = get(a_index);
				remove(a_index);
				return val;
			}


	public: int indexOf(const T& a_t, int a_start = 0) const throw()
			{
				if ( a_start < 0 )
					a_start = 0;
				for ( ; a_start < _size; a_start++ )
				{
					if ( get(a_start) == a_t )
						return a_start;
				}
				return -1;
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

	public: void clear() throw() { truncate(0); }

	public: bool isEmpty() const throw()
			{
				return _size == 0;
			}
};



template<class T>
inline Array<T>::Array(int a_bufsize) throw()
{
	_size = 0;
	_bufsize = (a_bufsize > 0 ? a_bufsize : 0);
	inewDataBuffer();
}

template<class T>
inline Array<T>::Array(int a_size, int a_bufsize) throw()
{
	if ( a_size < 0 )
		a_size = 0;
	if ( a_bufsize < a_size )
		a_bufsize = a_size;
	_size = a_size;
	_bufsize = a_bufsize;
	inewDataBuffer();
}

template<class T>
Array<T>::Array(const T* a_data, int a_size, int a_minbufsize) throw()
{
	if ( (a_data == 0) || (a_size < 0) )
		a_size = 0;
	if ( a_minbufsize < a_size )
		a_minbufsize = a_size;
	_size = a_size;
	_bufsize = (a_minbufsize >= DEFAULT_BUFSIZE ? a_minbufsize : DEFAULT_BUFSIZE);
	inewDataBuffer();
	icopyData(a_data);
}

template<class T>
Array<T>::Array(const Array<T>& a_array, int a_minbufsize) throw()
{
	_size = a_array.isize();
	if ( a_minbufsize == 0 )
		a_minbufsize = a_array.ibufsize();
	else if ( a_minbufsize < _size )
		a_minbufsize = _size;
	_bufsize = (a_minbufsize > 0 ? a_minbufsize : 0);
	inewDataBuffer();
	icopyData(a_array.idata());
}

template<class T>
const Array<T>& Array<T>::operator=(const Array<T>& a_array) throw()
{
	if ( this == &a_array )
		return *this;
 	if ( ibufsize() < a_array.ibufsize() )
 	{
 		delete [] _data;
 		_bufsize = a_array.ibufsize();
 		inewDataBuffer();
 	}
 	_size = a_array.isize();
 	icopyData(a_array.idata());
 	return *this;
}


template<class T>
void Array<T>::insert(const T& a_t, int a_index) throw()
{
 	if ( a_index < 0 )
 		a_index = 0;
 	else if ( a_index >= _size )
 		a_index = _size;
 	if ( _size >= _bufsize )
 	{
 		T* data = _data;
		int addbufsize = 2 * _bufsize;
		if ( addbufsize > 10000 )
			addbufsize = 10000;
 		_bufsize += (DEFAULT_BUFSIZE + addbufsize);
 		inewDataBuffer();
 		int i;
		if ( data != 0 )
		{
 			for ( i = 0; i < a_index; i++ )
 				_data[i] = data[i];
 			int j = i + 1;
 			while ( i < _size )
 				_data[j++] = data[i++];
			delete [] data;
		}
 	}
 	else
 	{
 		for ( int i = _size; i > a_index; i-- )
 			_data[i] = _data[i - 1];
 	}
 	_data[a_index] = a_t;
 	_size++;
}

template<class T>
void Array<T>::remove(int a_start, int a_end) throw()
{
 	if ( (a_end < 0) || (a_end > _size) )
 		a_end = _size;
 	if ( a_end <= a_start )
 		return;

 	int len = a_end - a_start;
	for ( int i = a_end; i < _size; i++ )
 		_data[i - len] = _data[i];
 	_size -= len;
}

template<class T>
inline void Array<T>::truncate(int a_size) throw()
{
 	if ( (a_size >= 0) && (a_size < _size) )
 	{
 		_size = a_size;
 	}
}

template<class T>
inline void Array<T>::setSize(int a_size) throw()
{
	if ( a_size < 0 )
		return;
 	if ( a_size > _bufsize )
 	{
 		setMinBufSize(a_size);
 	}
 	_size = a_size;
}

template<class T>
inline void Array<T>::setSize(int a_size, int a_bufsize) throw()
{
	if ( a_size < 0 )
		return;
	if ( a_bufsize < a_size )
		a_bufsize = a_size;
 	setMinBufSize(a_bufsize);
 	_size = a_size;
}

template<class T>
void Array<T>::setMinBufSize(int a_bufsize) throw()
{
 	if ( a_bufsize < _size )
		a_bufsize = _size;
	a_bufsize = (a_bufsize > 0  ? a_bufsize : 0);
	if ( a_bufsize != _bufsize )
 	{
 	 	T* data = _data;
 	 	_bufsize = a_bufsize;
 	 	inewDataBuffer();
 	 	if ( data != 0 )
		{
			icopyData(data);
 	 		delete [] data;
		}
 	}
}



typedef Array<byte> ByteArray;

} // namespace

#endif
