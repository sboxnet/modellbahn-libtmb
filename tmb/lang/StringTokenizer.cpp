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



#include <tmb/lang/StringTokenizer.h>


namespace tmb {

const String& StringTokenizer::next()
{
	if ( _index < 0 )
		throw NotFoundException();
	_index = _string.nextToken(_index, _token,
							(_linecomment.length() ? _linecomment.c_str() : 0),
							(_quotes.length() ? _quotes.c_str() : 0),
							_use_escapes,
							(_sep_chars.length() ? _sep_chars.c_str() : 0));
	return _token;
}

bool StringTokenizer::hasNext() {
	if (_index < 0)
		return false;
	return _string.hasNextToken(_index,
							(_linecomment.length() ? _linecomment.c_str() : 0),
							(_sep_chars.length() ? _sep_chars.c_str() : 0));
}


} // namespace
