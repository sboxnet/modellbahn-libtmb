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


#ifndef _TMB_LANG_HASHMAPP_H_
#define _TMB_LANG_HASHMAPP_H_

#include <tmb/lang/HashMap.h>
#include <tmb/lang/Exception.h>

namespace tmb {

template<class Val>
class HashMapP : public HashMap<Val*>
{
	private: typedef HashMap<Val*> super;

	protected: int _autodelete; ///< if true, all removed elements are auto deleted with "delete"


	protected: void iautoDelete() throw()
			{
				for ( int i = 0; i < super::_hashsize; i++ )
				{
					typename super::Element *elem = super::_hasharray[i];
					while ( elem != 0 )
					{
						delete elem->_data;
						elem = elem->_next;
					}
				}
			}

			/**
			 * Default constructor. Creates an empty Map.
			 */
	public: explicit HashMapP() throw()
								: HashMap<Val*>(), _autodelete(0)
			{
			}
	public: explicit HashMapP(AutoDelete a_autodelete) throw()
								: HashMap<Val*>(), _autodelete(a_autodelete)
			{
			}
	public: explicit HashMapP(int a_hashsize) throw()
								: HashMap<Val*>(a_hashsize), _autodelete(0)
			{
			}
	public: explicit HashMapP(int a_hashsize, AutoDelete a_autodelete) throw()
								: HashMap<Val*>(a_hashsize), _autodelete(a_autodelete)
			{
			}

	private: HashMapP(const HashMapP<Val>& a_map) throw();
	private: void operator=(const HashMapP<Val>& a_map) throw();

			/**
			 * Destructor
			 */
	public: ~HashMapP() throw()
			{
				if ( _autodelete & AutoDelete_DESTRUCT )
					iautoDelete();
			}

	public: bool isAutoDelete() const throw() { return _autodelete != 0; }

	public: void setAutoDelete(AutoDelete a_on) throw() { _autodelete = a_on; }


	private: bool iput(typename super::Key a_key, const String* a_key_str, Val* a_val) throw()
			{
				if ( a_key == 0 )
					return false;

                if ( super::_hashsize <= 0 )
				{
					super::_hashsize = super::DEFAULT_HASH_SIZE;
					super::_hasharray = new typename super::Element* [super::_hashsize];
					::memset(super::_hasharray, 0, super::_hashsize * sizeof(typename super::Element*));
				}

				int hashidx = super::ihashidx(a_key);
				typename super::Element* elem = super::_hasharray[hashidx];
				if ( elem == 0 )
				{
					elem = new typename super::Element(a_key, a_val);
					if ( a_key_str )
						elem->_key_str = new String(*a_key_str);
					elem->_next = 0;
					super::_hasharray[hashidx] = elem;
					super::_size++;
					return false;
				}

				while ( elem != 0 )
				{
					if ( elem->_key == a_key )
					{
						if ( (_autodelete & AutoDelete_REMOVE) && (a_val != elem->_data) )
						{
							delete elem->_data;
						}
						elem->_data = a_val;
						return true;
					}
					elem = elem->_next;
				}

				// insert
				elem = new typename super::Element(a_key, a_val);
				if ( a_key_str )
					elem->_key_str = new String(*a_key_str);
				elem->_next = super::_hasharray[hashidx];
				super::_hasharray[hashidx] = elem;
				super::_size++;
				return false;
			}
	public: bool put(typename super::Key a_key, Val* a_val) throw()
			{
				return iput(a_key, 0, a_val);
			}
	public: bool put(const String& a_key, Val* a_val) throw()
			{
				return iput(a_key.hashcode(), &a_key, a_val);
			}

			/**
			 * Remove the key/value pair specified with a_key.
			 * @@param a_key    the key to remove
			 * @@return true it the remove was successfull, means a_key was found and removed
			 */
	public: bool remove(typename super::Key a_key) throw()
			{
                typename super::Element* elem = super::iremove(a_key);
				if ( elem )
				{
					if ( _autodelete & AutoDelete_REMOVE )
						delete elem->_data;
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
			 * Remove all key/value pairs of this map. After this operation, the size of
			 * the map is 0.
			 */
	public: void clear() throw()
			{
				for ( int i = 0; i < super::_hashsize; i++ )
				{
					typename super::Element *elem = super::_hasharray[i];
					while ( elem != 0 )
					{
						typename super::Element* next = elem->_next;
						if ( _autodelete & AutoDelete_REMOVE )
						{
							delete elem->_data;
						}
						delete elem;
						elem = next;
					}
					super::_hasharray[i] = 0;
				}
				super::_size = 0;
			}

};

} // namespace


#endif
