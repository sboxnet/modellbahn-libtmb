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


#ifndef _TMB_LANG_HASHMAP_H_
#define _TMB_LANG_HASHMAP_H_

#include <stdlib.h>
#include <string.h>

#include <tmb/lang/Exception.h>
#include <tmb/lang/String.h>

namespace tmb {


template<class Val>
class HashMap
{
	public: typedef uint32 Key;

	protected: class Element
			{
				public: Element* _next;
				public: Key		 _key;
				public: String*  _key_str;
				public: Val		 _data;

				//public: explicit Element() {}
				public: explicit Element(Key a_key, const Val& a_data)
							: _key(a_key), _key_str(0), _data(a_data)
						{}
				public: ~Element() { delete _key_str; }
			};

	public: enum { DEFAULT_HASH_SIZE = 16 };

	protected: int		 _hashsize;
	protected: Element** _hasharray;
	protected: int		 _size;


	protected: int ihashidx(uint32 a_key) const throw()
			{
				return (a_key % (uint32)_hashsize);
			}

			/**
			 * Remove all key/value pairs of this map. After this operation, the size of
			 * the map is 0.
			 */
	private: void iclearData() throw()
			{
				for ( int i = 0; i < _hashsize; i++ )
				{
					Element *elem = _hasharray[i];
					while ( elem != 0 )
					{
						Element* next = elem->_next;
						delete elem;
						elem = next;
					}
					_hasharray[i] = 0;
				}
				_size = 0;
			}

			/**
			 * Default constructor. Creates an empty Map.
			 */
	public: explicit HashMap() throw() : _hashsize(0), _hasharray(0), _size(0)
			{
			}
	public: explicit HashMap(int a_hashsize) throw()
			{
				_hashsize = a_hashsize;
				if ( _hashsize < DEFAULT_HASH_SIZE )
					_hashsize = DEFAULT_HASH_SIZE;
				_hasharray = new Element* [_hashsize];
				::memset(_hasharray, 0, _hashsize * sizeof(Element*));
				_size = 0;
			}

	private: HashMap(const HashMap<Val>& a_map) throw();
	private: void operator=(const HashMap<Val>& a_map) throw();

			/**
			 * Destructor
			 */
	public: ~HashMap() throw()
			{
				iclearData();
				delete [] _hasharray;
			}

			/**
			 * Get the size of the map
			 * @return the map size
			 */
	public: int size() const throw() { return _size; }

	public: void clear() throw() { iclearData(); }

			/**
			 * Get the value that is associated with the specified key.
			 * @param a_key    the key
			 * @return the value associated with the key
			 */
	private: Val& iget(Key a_key, const String* a_key_str) const
			{
				if ( a_key != 0 && _hashsize > 0 )
				{
					int hashidx = ihashidx(a_key);
					Element* elem = _hasharray[hashidx];
					while ( elem != 0 )
					{
						if ( elem->_key == a_key
							&& ( !a_key_str || !elem->_key_str
								|| elem->_key_str->equals(*a_key_str)) )
						{
							return elem->_data;
						}
						elem = elem->_next;
					}
				}
				throw NotFoundException();
			}
	public: Val& get(Key a_key) const
			{
				return iget(a_key, 0);
			}
			/// An alias for get()
	public: Val& operator[](Key a_key) const
			{
				return iget(a_key, 0);
			}

	public: Val& get(const String& a_s) const
			{
				return iget(a_s.hashcode(), &a_s);
			}
	public: Val& operator[](const String& a_s) const
			{
				return iget(a_s.hashcode(), &a_s);
			}

			/**
			 * Put a key/value pair into the map. It the key already exists,
			 * the value of this key is replaced by the new value.
			 * @param a_key    the key
			 * @param a_val    the value
			 * @return true if the key already exists and the value was overwritten
			 */
	private: bool iput(Key a_key, const String* a_key_str, const Val& a_val) throw()
			{
				if ( a_key == 0 )
					return false;

                if ( _hashsize <= 0 )
				{
					_hashsize = DEFAULT_HASH_SIZE;
					_hasharray = new Element* [_hashsize];
					::memset(_hasharray, 0, _hashsize * sizeof(Element*));
				}

				int hashidx = ihashidx(a_key);
				Element* elem = _hasharray[hashidx];
				if ( elem == 0 )
				{
					elem = new Element(a_key, a_val);
					if ( a_key_str )
						elem->_key_str = new String(*a_key_str);
					elem->_next = 0;
					_hasharray[hashidx] = elem;
					_size++;
					return false;
				}

				while ( elem != 0 )
				{
					if ( elem->_key == a_key )
					{
						elem->_data = a_val;
						return true;
					}
					elem = elem->_next;
				}

				// insert
				elem = new Element(a_key, a_val);
				if ( a_key_str )
					elem->_key_str = new String(*a_key_str);
				elem->_next = _hasharray[hashidx];
				_hasharray[hashidx] = elem;
				_size++;
				return false;
			}
	public: bool put(Key a_key, const Val& a_val) throw()
			{
				return iput(a_key, 0, a_val);
			}
	public: bool put(const String& a_key, const Val& a_val) throw()
			{
				return iput(a_key.hashcode(), &a_key, a_val);
			}
			/**
			 * Tests if the map contains a key.
			 * @param a_key    the key
			 * @return true it a_key exists in the map
			 */
	private: bool icontainsKey(Key a_key, const String* a_key_str) const throw()
			{
				if ( a_key == 0 || _size == 0 || _hashsize == 0 )
					return false;

				int hashidx = ihashidx(a_key);
				Element* elem = _hasharray[hashidx];
				while ( elem != 0 )
				{
					if ( elem->_key == a_key
						&& ( !a_key_str || !elem->_key_str
							|| elem->_key_str->equals(*a_key_str)) )
					{
						return true;
					}
					elem = elem->_next;
				}
				return false;
			}
	public: bool containsKey(Key a_key) const throw()
			{
				return icontainsKey(a_key, 0);
			}
	public: bool containsKey(const String& a_key) const throw()
			{
				return icontainsKey(a_key.hashcode(), &a_key);
			}

			/**
			 * Remove the key/value pair specified with a_key.
			 * @param a_key    the key to remove
			 * @return true it the remove was successfull, means a_key was found and removed
			 */
	protected: Element* iremove(Key a_key) throw()
			{
				if ( a_key != 0 && _hashsize > 0 && _size > 0 )
				{
					int hashidx = ihashidx(a_key);
					Element* elem = _hasharray[hashidx];
					Element* prev = 0;
					while ( elem != 0 )
					{
						if ( elem->_key == a_key )
						{
							if ( prev != 0 )
								prev->_next = elem->_next;
							else
								_hasharray[hashidx] = elem->_next;
							_size--;
							return elem;
						}
						prev = elem;
						elem = elem->_next;
					}
				}
				return 0;
			}
	public: bool remove(Key a_key) throw()
			{
				Element* elem = iremove(a_key);
				if ( elem )
				{
					delete elem;
					return true;
				}
				return false;
			}
	public: bool remove(const String& a_key) throw()
			{
				return remove(a_key.hashcode());
			}

			/**
			 * Gets the value associated with a key and removes the key.
			 * @param a_key    the key
			 * @return the value associated with the key
			 */
	public: Val getAndRemove(Key a_key)
			{
				Element* elem = iremove(a_key);
				if ( elem )
				{
					Val v = elem->c_data;
					delete elem;
					return v;
				}
				throw NotFoundException();
			}
	public: Val getAndRemove(const String& a_key)
			{
				return getAndRemove(a_key.hashcode());
			}


	protected: Element* inextKey(Key a_key = 0) const throw()
			{
				if ( _size == 0 )
					return 0;

				if ( a_key == 0 )
				{
					// find first
					for ( int i = 0; i < _hashsize; i++ )
					{
						Element* elem = _hasharray[i];
						if ( elem != 0 )
						{
							return elem;
						}
					}
				}
				else
				{
					int hashidx = ihashidx(a_key);
					while ( hashidx < _hashsize )
					{
						Element* elem = _hasharray[hashidx];
						while ( elem != 0 )
						{
							if ( elem->_key == a_key )
							{
								// get next
								if ( elem->_next != 0 )
									return elem->_next;
								// no next element in hash, use next hash entry
								for ( hashidx++; hashidx < _hashsize; hashidx++ )
								{
									elem = _hasharray[hashidx];
									if ( elem != 0 )
									{
										return elem;
									}
								}
								return 0;
							}
							elem = elem->_next;
						}

						hashidx++;
					}
				}

				return 0;
			}
	public: Key nextKey(Key a_key = 0) const throw()
			{
				Element* elem = inextKey(a_key);
				if ( elem )
					return elem->_key;
				return 0;
			}
	public: Key nextKey(Key a_key, Val** a_pval, const String** a_pkey_str = 0) const throw()
			{
				Element* elem = inextKey(a_key);
				if ( elem )
				{
					if ( a_pval )
						*a_pval = &(elem->_data);
					if ( a_pkey_str )
						*a_pkey_str = elem->_key_str;
					return elem->_key;
				}
				if ( a_pkey_str )
					*a_pkey_str = 0;
				return 0;
			}
	public: Key nextKey(Key a_key, Val& a_pval, const String** a_pkey_str = 0) const throw()
			{
				Element* elem = inextKey(a_key);
				if ( elem )
				{
					a_pval = elem->_data;
					if ( a_pkey_str )
						*a_pkey_str = elem->_key_str;
					return elem->_key;
				}
				if ( a_pkey_str )
					*a_pkey_str = 0;
				return 0;
			}

};

} // namespace


#endif
