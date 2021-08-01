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


#ifndef _TMB_LANG_MAPP_H_
#define _TMB_LANG_MAPP_H_


#include <tmb/lang/Map.h>
#include <tmb/lang/ListP.h>

namespace tmb {

template<class Key, class Val>
class MapP : public Map<Key,Val*,PointerList<Val> >
{
	public: explicit MapP(AutoDelete a_autodelete = 0) throw()
							: Map<Key,Val*,PointerList<Val> >()
			{
				_values.setAutoDelete(a_autodelete);
			}

	public: MapP(const MapP<Key,Val>& a_map) throw()
							: Map<Key,Val*,PointerList<Val> >(a_map)
			{
			}

	public: const MapP<Key,Val>& operator=(const MapP<Key,Val>& a_map) throw()
			{
				Map<Key,Val*,PointerList<Val> >::operator=(a_map);
				return *this;
			}

	public: ~MapP() throw() {}

	public: bool isAutoDelete() const throw()
			{
				return _values.isAutoDelete();
			}

	public: void setAutoDelete(AutoDelete a_on) throw()
			{
				_values.setAutoDelete(a_on);
			}
};

} // namespace

#endif
