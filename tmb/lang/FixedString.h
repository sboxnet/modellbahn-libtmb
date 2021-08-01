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


#ifndef _TMB_LANG_FIXEDSTRING_H_
#define _TMB_LANG_FIXEDSTRING_H_

#include <string.h>
#include <tmb/lang/String.h>

namespace tmb {

template<int maxlen>
struct FixedString
{
	char	string[maxlen+1];
	int		len;

	FixedString()
	{
		string[0] = 0;
		len = 0;
	}

	FixedString(const char* a_s)
	{
		::strncpy(string, a_s, maxlen);
        string[maxlen] = 0;
		len = ::strlen(string);
	}

	FixedString(const String& a_s)
	{
		::strncpy(string, a_s.c_str(), maxlen);
		string[maxlen] = 0;
		len = a_s.length();
		if ( len > maxlen )
			len = maxlen;
	}

	FixedString(const FixedString& a_s)
	{
		::strcpy(string, a_s.string);
		len = a_s.length;
	}

	void operator= (const char* a_s)
	{
		::strncpy(string, a_s, maxlen);
		string[maxlen] = 0;
		len = ::strlen(string);
	}
	void operator= (const String& a_s)
	{
		::strncpy(string, a_s.c_str(), maxlen);
		string[maxlen] = 0;
		len = a_s.length();
		if ( len > maxlen )
			len = maxlen;
	}
	void operator= (const FixedString& a_s)
	{
		::strcpy(string, a_s.string);
		len = a_s.length;
	}

    int length() const { return len; }
	int maxlength() const { return maxlen; }

	const char* c_str() const { return string; }
	char* str() { return string; }

	operator const char* () { return string; }


	bool operator== (const FixedString& a_s)
	{
		return ::strcmp(string, a_s.string) == 0;
	}
	bool operator!= (const FixedString& a_s)
	{
		return !operator==(a_s);
	}

	bool operator== (const String& a_s)
	{
		return ::strcmp(string, a_s.c_str()) == 0;
	}
	bool operator!= (const String& a_s)
	{
		return !operator==(a_s);
	}

	bool operator== (const char* a_s)
	{
		return ::strcmp(string, a_s) == 0;
	}
	bool operator!= (const char* a_s)
	{
		return !operator==(a_s);
	}

};


} // namespace

#endif
