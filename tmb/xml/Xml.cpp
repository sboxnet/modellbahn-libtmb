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


#include <tmb/xml/Xml.h>


namespace tmb {


Xml::Xml()
{
}

Xml::~Xml()
{
}

String Xml::encodeText(const String& a_txt)
{
	int len = a_txt.length();

	String s(len + 256);

	const char* p = a_txt.c_str();
	const char* pend = p + len;

	while ( p < pend )
	{
		char c = *p++;
		switch (c)
		{
			case '&': s << "&amp;";   break;
			case 'ä': s << "&auml;";  break;
			case 'ö': s << "&ouml;";  break;
			case 'ü': s << "&uuml;";  break;
			case 'Ä': s << "&Auml;";  break;
			case 'Ö': s << "&Ouml;";  break;
			case 'Ü': s << "&Uuml;";  break;
			case 'ß': s << "&szlig;"; break;
			case '<': s << "&lt;";    break;
			case '>': s << "&gt;";    break;
			case '\"': s << "&quot;"; break;

			default:
				s << c;
				break;
		}
	}

	return s;
}


} // namespace
