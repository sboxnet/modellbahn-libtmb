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


#include <tmb/lang/RegEx.h>


#ifdef WIN32

# define STDC_HEADERS 1
# define __STDC__ 1
extern "C" {
# include "gnuregex/regex.h"
}

#else

# include <regex.h>

#endif



namespace tmb {


RegEx::RegEx()
		: _regex_t(0)
{
}

RegEx::RegEx(const String& a_re, int a_compflags)
		: _regex_t(0)
{
	compile(a_re, a_compflags);
}

RegEx::~RegEx()
{
	if ( _regex_t )
	{
		::regfree((regex_t*)_regex_t);
		delete (regex_t*)_regex_t;
	}
}

void RegEx::compile(const String& a_re, int a_compflags)
{
	if ( _regex_t )
	{
		::regfree((regex_t*)_regex_t);
		delete (regex_t*)_regex_t;
		_regex_t = 0;
	}

	regex_t* re = new regex_t;

	int flags = REG_NOSUB;
	if ( a_compflags & Compile_EXTENDED )
		flags |= REG_EXTENDED;
	if ( a_compflags & Compile_ICASE )
		flags |= REG_ICASE;
	if ( a_compflags & Compile_NEWLINE )
		flags |= REG_NEWLINE;

	int r = ::regcomp(re, a_re.c_str(), flags);
	if ( r != 0 )
	{
		char buf[256];
		::regerror(r, re, buf, sizeof(buf));
		delete re;
		throw ParseException(String("RegEx::compile(\"", 256) << a_re << "\"): " << buf);
	}

	_regex_t = re;
}

bool RegEx::match(const String& a_str, int a_matchflags)
{
	if ( _regex_t == 0 )
		throw InvalidOperationException("RegEx::match(): no compiled regular expression");

	int flags = 0;
	if ( a_matchflags & Match_NOTBOL )
		flags |= REG_NOTBOL;
	if ( a_matchflags & Match_NOTEOL )
		flags |= REG_NOTEOL;

	int r = ::regexec((const regex_t*)_regex_t, a_str.c_str(), 0, 0, flags);
	if ( r == 0 )
		return true;
	if ( r == REG_NOMATCH )
		return false;

	char buf[256];
	::regerror(r, (const regex_t*)_regex_t, buf, sizeof(buf));
	throw InvalidOperationException(String("RegEx::match(\"", 256) << a_str << "\"): " << buf);
}


} // namespace
