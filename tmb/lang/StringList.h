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


#ifndef _TMB_LANG_STRINGLIST_H_
#define _TMB_LANG_STRINGLIST_H_

#include <tmb/lang/String.h>
#include <tmb/lang/List.h>

namespace tmb {


class StringList : public List<String>
{
	public: explicit StringList() throw() : List<String>()
			{
			}

	public: StringList(const StringList& a_list) throw() : List<String>(a_list)
			{
			}

	public: const StringList& operator=(const StringList& a_list) throw()
			{
				List<String>::operator=(a_list);
				return *this;
			}

	public: ~StringList() throw() {}

	public: void stringToLines(const String& a_input) throw();

	public: String linesToString(const String& a_lineend = String("\n")) const throw();

};

} // namespace


#endif
