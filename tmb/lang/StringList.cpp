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


#include <tmb/lang/StringList.h>


namespace tmb {

void StringList::stringToLines(const String& a_input) throw()
{
	int index = 0;
	while (index < a_input.length())
	{
		int index2 = a_input.indexOf('\n', index);

		append(a_input.substring(index, index2).erase('\r'));

		if ( index2 < 0 )
			break;
		index = index2 + 1;
	}
}

String StringList::linesToString(const String& a_eol) const throw()
{
	// first count the size of the string buffer to avoid many copy operations
	int i;
	int num = size();
	int s = 0;

	for ( i = 0; i < num; i++ )
	{
		s += get(i).length();
	}

	String result(s + a_eol.length() * num + 32);

	for ( i = 0; i < num; i++ )
	{
		result << get(i) << a_eol;
	}

	return result;
}


} // namespace
