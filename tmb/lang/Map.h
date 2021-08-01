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


#ifndef _TMB_LANG_MAP_H_
#define _TMB_LANG_MAP_H_

#include <tmb/lang/List.h>
#include <tmb/lang/Exception.h>

namespace tmb {


template<class Key, class Val, class ListT = List<Val> >
class Map
{
	protected: ListT _keys;
	protected: ListT _values;

	public: explicit Map();

	public: Map(const Map<Key,Val,ListT>& a_map);

	public: const Map<Key,Val,ListT>& operator=(const Map<Key,Val,ListT>& a_map);

	public: ~Map() {}

	public: int size() const { return _keys.size(); }

	public: Val& get(const Key& a_key) const;
	public: Val& operator[](const Key& a_key) const
			{
				return get(a_key);
			}

	public: bool put(const Key& a_key, const Val& a_val);

	public: bool containsKey(const Key& a_key) const;

	public: bool containsValue(const Val& a_val) const;

	public: bool remove(const Key& a_key);

	public: Val& getAndRemove(const Key& a_key);

	public: void clear();

	public: Key& getKeyAtIndex(int a_index) const
			{
				return _keys.get(a_index);
			}

	public: Val& getValueAtIndex(int a_index) const
			{
				return _values.get(a_index);
			}
};



template<class Key, class Val, class ListT>
inline Map<Key,Val,ListT>::Map() : _keys(), _values()
{
}

template<class Key, class Val, class ListT>
inline Map<Key,Val,ListT>::Map(const Map<Key,Val,ListT>& a_map)
						: _keys(a_map._keys), _values(a_map._values)
{
}

template<class Key, class Val, class ListT>
const Map<Key,Val,ListT>& Map<Key,Val,ListT>::operator=(const Map<Key,Val,ListT>& a_map)
{
	if ( this != &a_map )
	{
		_keys = a_map._keys;
		_values = a_map._values;
	}
	return *this;
}


template<class Key, class Val, class ListT>
bool Map<Key,Val,ListT>::put(const Key& a_key, const Val& a_val)
{
	int pos = _keys.indexOf(a_key, 0);
	if ( pos < 0 )
	{
	 	_keys.append(a_key);
	 	_values.append(a_val);
	 	return false;
	}
	else
	{
		_values.put(a_val, pos);
		return true;
	}
}

template<class Key, class Val, class ListT>
inline bool Map<Key,Val,ListT>::containsKey(const Key& a_key) const
{
 	return _keys.contains(a_key);
}

template<class Key, class Val, class ListT>
inline bool Map<Key,Val,ListT>::containsValue(const Val& a_val) const
{
 	return _values.contains(a_val);
}

template<class Key, class Val, class ListT>
bool Map<Key,Val,ListT>::remove(const Key& a_key)
{
 	int pos = _keys.indexOf(a_key, 0);
 	if ( pos < 0 )
 		return false;
 	_keys.remove(pos);
 	_values.remove(pos);
 	return true;
}

template<class Key, class Val, class ListT>
Val& Map<Key,Val,ListT>::getAndRemove(const Key& a_key)
{
	int pos = _keys.indexOf(a_key, 0);
	if ( pos >= 0 )
	{
		Val val = _values.get(pos);
		_keys.remove(pos);
		_values.remove(pos);
		return val;
	}
	throw NotFoundException();
}

template<class Key, class Val, class ListT>
inline Val& Map<Key,Val,ListT>::get(const Key& a_key) const
{
 	int pos = _keys.indexOf(a_key, 0);
	if ( pos >= 0 )
		return _values.get(pos);
	throw NotFoundException();
}


template<class Key, class Val, class ListT>
inline void Map<Key,Val,ListT>::clear()
{
 	_keys.clear();
 	_values.clear();
}



} // namespace


#endif
