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


#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cerrno>

#include <ctype.h>
#include <stdarg.h>

#include <tmb/lang/String.h>
#include <tmb/lang/Exception.h>

namespace tmb {


enum NumType  { NumType_int =    0,
				NumType_short =  1,
				NumType_long =   2,
                NumType_uint =   3,
				NumType_ushort = 4,
				NumType_ulong =  5,
                NumType_double = 6 };

static const char* formats[7][5] =
	{ // NumType_int
    { "%d", "%x", "%o", "0x%x", "0%o" },
    // NumType_short
    { "%hd", "%hx", "%ho", "0x%hx", "0%ho" },
    // NumType_long
    { "%ld", "%lx", "%lo", "0x%lx", "0%lo" },
    // NumType_uint
    { "%u", "%x", "%o", "0x%x", "0%o" },
    // NumType_ushort
    { "%hu", "%hx", "%ho", "0x%hx", "0%ho" },
    // NumType_ulong
    { "%lu", "%lx", "%lo", "0x%lx", "0%lo" },
    // NumType_double
    { "%.15g", "%.15g", "%.15g", "%.15g", "%.15g" }
  };


String::nullstringdata String::_nullstring = { { 2, 0, 0 }, "" };

const String String::null;

int String::iinitData(int a_length, int a_minbufsize) throw()
{
	if ( a_length < 0 )
		a_length = 0;
	if ( a_minbufsize <= a_length )
		a_minbufsize = a_length+1;
	_data = (struct stringdata*)new char [sizeof(struct stringdata) + a_minbufsize];
	isetlinks(1);
	isetlength(a_length);
	isetbufsize(a_minbufsize);
	_data->string[_data->length] = 0;
	return _data->length;
}

void String::iinitCopyData(const char* a_s, int a_minbufsize) throw()
{
	iinitCopyData(a_s, a_s == 0 ? 0 : ::strlen(a_s), a_minbufsize);
}

void String::iinitCopyData(const char* a_s, int a_len, int a_minbufsize) throw()
{
	if ( iinitData(a_len, a_minbufsize) > 0 )
	{
		::strncpy(_data->string, a_s, _data->length);
	}
}

String::String(const String& a_initstring, int a_minbufsize,
					   bool a_forcedeepcopy) throw()
{
	if ( !a_forcedeepcopy && (a_minbufsize == a_initstring.bufsize()) )
		iflatCopyData(a_initstring);
	else
		iinitCopyData(a_initstring, a_minbufsize);
}

void String::setCString(const char* a_s) throw()
{
	// save c_data for case a_s is part of c_data->string !!!
	struct stringdata* data = _data;
	iinitCopyData(a_s, 0);
	ireleaseData(data);
}



const String& String::format(const char* a_fmt, ...) throw()
{
	va_list va;
	va_start(va, a_fmt);
	vformat(a_fmt, va);
	va_end(va);
	return *this;
}
const String& String::format(const String& a_fmt, ...) throw()
{
	va_list va;
	va_start(va, a_fmt);
	vformat(a_fmt.c_str(), va);
	va_end(va);
	return *this;
}

void String::vformat(const char* a_fmt, va_list a_va)
{
	char buf[256];

	int n = ::vsnprintf(buf, sizeof(buf), a_fmt, a_va);
	if ( n >= (int)sizeof(buf) )
	{
		struct stringdata* data = _data;
		iinitData(n, 0);
		::vsnprintf(str(), bufsize(), a_fmt, a_va);
		ireleaseData(data);
		return;
	}
	else if ( n < 0 )
		throw InvalidOperationException("String::vformat(): vsnprintf error");

	setCString(buf);
}





void String::setMinBufSize(int a_minbufsize) throw()
{
	if ( (a_minbufsize > _data->length) && (a_minbufsize != _data->bufsize) )
	{
		iunlinkCopyData(a_minbufsize);
	}
}

void String::copy(char* a_s) const throw()
{
	if ( a_s )
		::strcpy(a_s, _data->string);
}

void String::copy(char* a_s, unsigned int a_num) const throw()
{
	if ( a_s )
		::strncpy(a_s, _data->string, a_num);
}

String& String::iinsert(const char* a_s, int a_len, int a_pos) throw()
{
	if ( a_len == 0 )
		return *this;

	if ( a_pos < 0 )
		a_pos = 0;
	else if ( a_pos > _data->length )
		a_pos = _data->length;

	if ( !iislinked() )
	{
	 	if ( (_data->length + a_len) < _data->bufsize )
	 	{
			// handles also a_s == this.c_data->string
			::memmove(_data->string + a_pos + a_len, _data->string + a_pos, 1 + _data->length - a_pos);
			::memmove(_data->string + a_pos, a_s, a_len);
	 	 	isetlength(_data->length + a_len);
	 	 	return *this;
	 	}
	}

 	struct stringdata* m_data = _data;
	if ( iinitData(_data->length + a_len, _data->bufsize) > 0 )
	{
		if ( a_pos > 0 )
			::memcpy(_data->string, m_data->string, a_pos);
		::strcpy(_data->string + a_pos, a_s);
		::strcpy(_data->string + a_pos + a_len, m_data->string + a_pos);
	}
	ireleaseData(m_data);
	return *this;
}

String& String::insert(const String& a_s, int a_pos) throw()
{
	return iinsert(a_s.c_str(), a_s.length(), a_pos);
}

String& String::insert(const char* a_s, int a_pos) throw()
{
	if ( a_s == NULL )
		return *this;
	return iinsert(a_s, ::strlen(a_s), a_pos);
}

String& String::insert(char a_char, int a_pos) throw()
{
	char m_buf[] = { a_char, 0 };
	return iinsert(m_buf, 1, a_pos);
}



#define INSERTNUM(_numtype, _numf, _num, _p) char m_buf[32]; \
                             ::snprintf(m_buf, sizeof(m_buf), formats[_numtype][_numf], _num); \
                             return insert(m_buf, (_p))

String& String::insert(short a_short, int a_pos, NumFormat a_f) throw()
{
	INSERTNUM(NumType_short, a_f, a_short, a_pos);
}

String& String::insert(int a_integer, int a_pos, NumFormat a_f) throw()
{
	INSERTNUM(NumType_int, a_f, a_integer, a_pos);
}

String& String::insert(long a_long, int a_pos, NumFormat a_f) throw()
{
	INSERTNUM(NumType_long, a_f, a_long, a_pos);
}


String& String::insert(unsigned short a_short, int a_pos, NumFormat a_f) throw()
{
	INSERTNUM(NumType_ushort, a_f, a_short, a_pos);
}

String& String::insert(unsigned int a_integer, int a_pos, NumFormat a_f) throw()
{
 	INSERTNUM(NumType_uint, a_f, a_integer, a_pos);
}

String& String::insert(unsigned long a_long, int a_pos, NumFormat a_f) throw()
{
	INSERTNUM(NumType_ulong, a_f, a_long, a_pos);
}

String& String::insert(float a_float, int a_pos) throw()
{
	INSERTNUM(NumType_double, dec, ((double)a_float), a_pos);
}

String& String::insert(double a_double, int a_pos) throw()
{
 	INSERTNUM(NumType_double, dec, a_double, a_pos);
}

String& String::insert(bool a_bool, int a_pos) throw()
{
 	return insert(a_bool ? "true" : "false", a_pos);
}


bool String::operator==(const String& a_s) const throw()
{
	if ( _data == a_s._data )
		return true;
	return (::strcmp(_data->string, a_s.c_str()) == 0);
}

bool String::operator== (const char* a_s) const throw()
{
	if ( a_s == NULL )
		return false;
	return (::strcmp(_data->string, a_s) == 0);
}


bool String::equals(const char* a_s, unsigned int a_num) const throw()
{
	if ( a_s == NULL )
		return false;
	return (::strncmp(_data->string, a_s, a_num) == 0);
}

bool String::equalsIgnoreCase(const String& a_s) const throw()
{
	if ( _data == a_s._data )
		return true;
	return (::strcasecmp(_data->string, a_s.c_str()) == 0);
}
bool String::equalsIgnoreCase(const char* a_s) const throw()
{
 	if ( a_s == NULL )
 		return false;
 	return (::strcasecmp(_data->string, a_s) == 0);
}
bool String::equalsIgnoreCase(const char* a_s, unsigned int a_num) const throw()
{
 	if ( a_s == NULL )
 		return false;
 	return (::strncasecmp(_data->string, a_s, a_num) == 0);
}

int String::compare(const String& a_s) const throw()
{
	if ( _data == a_s._data )
		return 0;
	return ::strcmp(_data->string, a_s.c_str());
}

int String::compare(const char* a_s) const throw()
{
	if ( a_s == NULL )
		return 1;
	return ::strcmp(_data->string, a_s);
}

int String::compare(const char* a_s, unsigned int a_num) const throw()
{
	if ( a_s == NULL )
		return 1;
	return ::strncmp(_data->string, a_s, a_num);
}

int String::compareIgnoreCase(const String& a_s) const throw()
{
	if ( _data == a_s._data )
		return 0;
	return ::strcasecmp(_data->string, a_s.c_str());
}

int String::compareIgnoreCase(const char* a_s) const throw()
{
 	if ( a_s == NULL )
 		return 1;
 	return ::strcasecmp(_data->string, a_s);
}

int String::compareIgnoreCase(const char* a_s, unsigned int a_num) const throw()
{
 	if ( a_s == NULL )
 		return 1;
 	return ::strncasecmp(_data->string, a_s, a_num);
}


char String::charAt(int a_index) const // throw(IndexOutOfBoundsException)
{
	if ( (a_index < 0) || (a_index >= _data->length) )
		throw IndexOutOfBoundsException();
	return _data->string[a_index];
}



String String::substring(int a_start, int a_end) const throw()
{
	if ( a_start < 0 )
		a_start = 0;
	if ( a_start >= _data->length )
		return String::null;
	if ( (a_end < 0) || (a_end >= _data->length) )
		a_end = _data->length;
	if ( a_end <= a_start )
		return String::null;

	String m_tmp(a_end - a_start, 0);
	if ( m_tmp.length() > 0 )
	{
		::strncpy(m_tmp.str(), _data->string + a_start, m_tmp.length());
		// no need to append a '0' char because Init() will do this !!
	}
	return m_tmp;
}

String& String::lowerCase() throw()
{
	if ( iislinked() )
	{
		iunlinkCopyData();
	}

	for ( char* m_p = _data->string; *m_p; m_p++ )
	 	*m_p = tolower(*m_p);

	return *this;
}


String& String::upperCase() throw()
{
	if ( iislinked() )
	{
		iunlinkCopyData();
	}

	for ( char* m_p = _data->string; *m_p; m_p++ )
		*m_p = toupper(*m_p);

	return *this;
}


long String::toLong(int a_base) const //throw(NumberFormatException)
{
	if ( _data->length == 0 )
		throw NumberFormatException();
	const char* m_p = _data->string;
	if ( isspace(*m_p) )
		throw NumberFormatException();
	char* m_endp;
	errno = 0;
	long m_long = ::strtol(m_p, &m_endp, a_base); // decimal, hex or octal
	if ( (errno != 0) || (m_p == m_endp) || (*m_endp != 0) )
		throw NumberFormatException();
	return m_long;
}

unsigned long String::toUnsignedLong(int a_base) const //throw(NumberFormatException)
{
	if ( _data->length == 0 )
		throw NumberFormatException();
	const char* m_p = _data->string;
	if ( isspace(m_p[0]) || (m_p[0] == '-') )
		throw NumberFormatException();
	char* m_endp;
	errno = 0;
	unsigned long m_long = ::strtoul(m_p, &m_endp, a_base); // decimal, hex or octal
	if ( (errno != 0) || (m_p == m_endp) || (*m_endp != 0) )
		throw NumberFormatException();
	return m_long;
}

double String::toDouble() const //throw(NumberFormatException)
{
	if ( _data->length == 0 )
		throw NumberFormatException();
	const char* m_p = _data->string;
	if ( isspace(*m_p) )
		throw NumberFormatException();
	char* m_endp;
	errno = 0;
	double m_double = ::strtod(m_p, &m_endp); // decimal, hex or octal
	if ( (errno != 0) || (m_p == m_endp) || (*m_endp != 0) )
		throw NumberFormatException();
	return m_double;
}


String& String::stripWhiteSpaceAtBegin() throw()
{
	if ( _data->length > 0 )
	{
		const char* m_p = _data->string;
		while ( isspace(*m_p) )
		{
			m_p++;
		}
		int m_len = m_p - _data->string;
		if ( m_len > 0 )
		{
			m_len = _data->length - m_len;
			if ( iislinked() )
			{
				iunlinkCopyData(m_p, m_len, _data->bufsize);
			}
			else
			{
				::strcpy(_data->string, m_p);
				isetlength(m_len);
			}
		}
	}
	return *this;
}

String& String::stripWhiteSpaceAtEnd() throw()
{
	if ( _data->length > 0 )
	{
		int m_len = _data->length;
		const char* m_p = _data->string;
		while ( m_len > 0 )
		{
			if ( !isspace(m_p[m_len-1]) )
				break;
			m_len--;
		}
		truncate(m_len);
	}
	return *this;
}


int String::indexOf(char a_char, int a_start) const throw()
{
	if ( a_start < 0 )
		a_start = 0;
	else if ( a_start >= _data->length )
		return -1;
	while ( a_start < _data->length )
	{
	 	if ( _data->string[a_start] == a_char )
	 		return a_start;
	 	a_start++;
	}
	return -1;
}

int String::indexOf(const String& a_s, int a_start) const throw()
{
	if ( _data == a_s._data )
	{
		return (a_start <= 0 ? 0 : -1);
	}
	if ( a_s.length() > _data->length )
		return -1;

	if ( a_start < 0 )
		a_start = 0;
	if ( a_start >= _data->length )
		return -1;
	int m_len = a_s.length();
	int m_endindex = _data->length - m_len;
	while ( a_start <= m_endindex )
	{
		if ( ::strncmp(a_s.c_str(), _data->string + a_start, m_len) == 0 )
			return a_start;
		a_start++;
	}
	return -1;
}

int String::indexOf(const char* a_s, int a_start) const throw()
{
	if ( a_s == NULL )
		return -1;
	if ( a_start < 0 )
		a_start = 0;
	if ( a_start >= _data->length )
		return -1;
	int m_len = ::strlen(a_s);
	int m_endindex = _data->length - m_len;
	while ( a_start <= m_endindex )
	{
		if ( ::strncmp(a_s, _data->string + a_start, m_len) == 0 )
			return a_start;
		a_start++;
	}
	return -1;
}



int String::lastIndexOf(char a_char) const throw()
{
	int m_index = _data->length;
	while ( m_index > 0 )
	{
		if ( _data->string[--m_index] == a_char )
			return m_index;
	}
	return -1;
}

int String::lastIndexOf(const String& a_s) const throw()
{
	if ( _data == a_s._data )
		return 0;
	if ( a_s.length() > _data->length )
		return -1;

	int m_pos = -1;
	for(;;)
	{
		int m_i = indexOf(a_s, m_pos + 1);
		if ( m_i < 0 )
			break;
		m_pos = m_i;
	}
	return m_pos;
}

int String::lastIndexOf(const char* a_s) const throw()
{
	int m_pos = -1;
	for(;;)
	{
		int m_i = indexOf(a_s, m_pos + 1);
		if ( m_i < 0 )
			break;
		m_pos = m_i;
	}
	return m_pos;
}


String& String::truncate(int a_length) throw()
{
	if ( (a_length >= 0) && (a_length < _data->length) )
	{
		if ( iislinked() )
		{
		 	iunlinkCopyData(_data->string, a_length, _data->bufsize);
		}
		else
		{
			isetlength(a_length);
			_data->string[a_length] = 0;
		}
	}
	return *this;
}

bool String::startsWith(char a_char) const throw()
{
	if ( _data->length > 0 )
		return _data->string[0] == a_char;
	return false;
}

bool String::startsWith(const char* a_start) const throw()
{
	if ( a_start == NULL )
		return false;
	return ::strncmp(_data->string, a_start, ::strlen(a_start)) == 0;
}

bool String::startsWith(const String& a_start) const throw()
{
	if ( _data == a_start._data )
		return true;
	if ( _data->length < a_start.length() )
		return false;
	return ::strncmp(_data->string, a_start.c_str(), a_start.length()) == 0;
}


bool String::startsWithIgnoreCase(char a_char) const throw()
{
#ifdef WIN32
	return ::strnicmp(_data->string, &a_char, 1) == 0;
#else
	return ::strncasecmp(_data->string, &a_char, 1) == 0;
#endif
}

bool String::startsWithIgnoreCase(const char* a_start) const throw()
{
	if ( a_start == NULL )
		return false;
#ifdef WIN32
	return ::strnicmp(_data->string, a_start, ::strlen(a_start)) == 0;
#else
	return ::strncasecmp(_data->string, a_start, ::strlen(a_start)) == 0;
#endif
}

bool String::startsWithIgnoreCase(const String& a_start) const throw()
{
	if ( _data == a_start._data )
		return true;
	if ( _data->length < a_start.length() )
		return false;
#ifdef WIN32
	return ::strnicmp(_data->string, a_start.c_str(), a_start.length()) == 0;
#else
	return ::strncasecmp(_data->string, a_start.c_str(), a_start.length()) == 0;
#endif
}


bool String::endsWith(char a_char) const throw()
{
	if ( _data->length > 0 )
		return _data->string[_data->length - 1] == a_char;
	return false;
}

bool String::endsWith(const char* a_end) const throw()
{
	if ( a_end == NULL )
		return false;
	int m_pos = _data->length - ::strlen(a_end);
	if ( m_pos < 0 )
		return false;
	return ::strcmp(_data->string + m_pos, a_end) == 0;
}

bool String::endsWith(const String& a_end) const throw()
{
	if ( _data == a_end._data )
		return true;
	int m_pos = _data->length - a_end.length();
	if ( m_pos < 0 )
		return false;
	return ::strcmp(_data->string + m_pos, a_end.c_str()) == 0;
}


bool String::endsWithIgnoreCase(char a_char) const throw()
{
	if ( _data->length == 0 )
		return false;
#ifdef WIN32
	return ::strnicmp(&(_data->string[_data->length - 1]), &a_char, 1) == 0;
#else
	return ::strncasecmp(&(_data->string[_data->length - 1]), &a_char, 1) == 0;
#endif
}

bool String::endsWithIgnoreCase(const char* a_end) const throw()
{
	if ( a_end == NULL )
		return false;
	int m_pos = _data->length - ::strlen(a_end);
	if ( m_pos < 0 )
		return false;
#ifdef WIN32
	return ::stricmp(_data->string + m_pos, a_end) == 0;
#else
	return ::strcasecmp(_data->string + m_pos, a_end) == 0;
#endif
}

bool String::endsWithIgnoreCase(const String& a_end) const throw()
{
	if ( _data == a_end._data )
		return true;
	int m_pos = _data->length - a_end.length();
	if ( m_pos < 0 )
		return false;
#ifdef WIN32
	return ::stricmp(_data->string + m_pos, a_end.c_str()) == 0;
#else
	return ::strcasecmp(_data->string + m_pos, a_end.c_str()) == 0;
#endif
}


String& String::remove(int a_start, int a_end) throw()
{
	if ( a_start < 0 )
		a_start = 0;
	else if ( a_start >= _data->length )
		return *this;

	if ( (a_end < 0) || (a_end > _data->length) )
		a_end = _data->length;
	if ( a_end < a_start )
		a_end = a_start;

	int m_len = a_end - a_start;

	if ( !iislinked() )
	{
		::memmove(_data->string + a_start, _data->string + a_end, 1 + _data->length - a_end);
		isetlength(_data->length - m_len);
		return *this;
	}
	struct stringdata* m_data = _data;
	if ( iinitData(_data->length - m_len, _data->bufsize) > 0 )
	{
		::strncpy(_data->string, m_data->string, a_start);
		::strcpy(_data->string + a_start, m_data->string + a_end);
	}
	ireleaseData(m_data);
	return *this;
}

String& String::replace(char a_1, char a_2) throw()
{
	if ( a_1 == 0 || a_2 == 0 )
		return *this;

 	for ( int i = 0; i < _data->length; i++ )
 	{
 	 	if ( _data->string[i] == a_1 )
		{
 			if ( iislinked() )
 			{
 	 			iunlinkCopyData();
 			}
 	 		_data->string[i] = a_2;
		}
 	}
 	return *this;
}

String& String::replace(const String& a_1, const String& a_2) throw()
{
	if ( a_1.length() == 0 )
		return *this;

	int idx = 0;

	for(;;)
	{
		idx = indexOf(a_1, idx);
		if ( idx < 0 )
			break;

		remove(idx, idx + a_1.length());
		if ( a_2.length() > 0 )
		{
			insert(a_2, idx);
			idx += a_2.length();
		}
	}

	return *this;
}


String& String::erase(char a_c) throw()
{
	if ( a_c == 0 )
		return *this;

	for ( int m_i = 0; m_i < _data->length; )
	{
		if ( _data->string[m_i] == a_c )
		{
			if ( iislinked() )
			{
				iunlinkCopyData();
			}
			::strcpy(_data->string+m_i, _data->string+m_i+1);
			isetlength(_data->length - 1);
		}
		else
		{
			m_i++;
		}
	}

	return *this;
}



static inline bool is_sep_char(char c, const char* a_sepchars) {
	if (a_sepchars && a_sepchars[0]) {
		return ::strchr(a_sepchars, c) != 0;
	}
	return ::isspace(c);
}

int String::nextToken(int a_index, String& a_token, const char* a_linecomment,
					  const char* a_quotes, bool a_use_escapes, const char* a_sepchars) const //throw(NotFoundException)
{
/*
	e.g.: linecomment: #
	      quote:       " '

	tok1 tok2 "tok3" 'tok4' "tok'5" "to""k6" #comment
	"tokwithoutendquote tok7 # no end quote, token ends at line end!
	tok8#xyz

  gives tokes:
	tok1
	tok2
	tok3
	tok4
	tok'5
	to
	k6
	tokwithoutendquote tok7
	tok8
*/

	if ((a_index < 0) || (a_index >= _data->length))
		throw NotFoundException();

	const char* m_p = _data->string + a_index;
	char quotec = 0;
	for(;;) {
		while (*m_p && is_sep_char(*m_p, a_sepchars)) {
			m_p++;
		}
		if (*m_p == 0)
			throw NotFoundException();

		if (a_linecomment && ::strchr(a_linecomment, *m_p))	{
			// skip line
			while (*m_p && *m_p != '\n') {
				m_p++;
			}
		} else {
			if (a_quotes && ::strchr(a_quotes, *m_p)) {
				quotec = *m_p++;
			}
			break;
		}
	}

	const char* m_pstart = m_p;
	char lastc = 0;
	int count = 0;
	bool escapes = false;
	while (*m_p) {
		char c = *m_p;
		if (quotec) {
			if (c == quotec) {
				m_p++;
				break;
			}
			if (c == '\n') {
				if (lastc == '\r')
					count--;
				m_p++;
				break;
			}
		} else if (a_use_escapes && c == '\\' && m_p[1]) {
			c = *(++m_p);
			escapes = true;
		} else {
			if (is_sep_char(c, a_sepchars))
				break;
			if (a_linecomment && ::strchr(a_linecomment, c))
				break;
		}
		lastc = c;
		m_p++;
		count++;
	}

	// faster then using substring()
	if (a_token.iislinked() || a_token.bufsize() < count+1) {
		a_token.ireleaseData();
		a_token.iinitData(count, 0);
	
	} else {
		a_token.str()[count] = 0;
		a_token.isetlength(count);
	}
	if (escapes) {
		char* d = a_token.str();
		while(count > 0) {
			char c = *m_pstart++;
			if (c == '\\') {
				c = *m_pstart++;
				switch (c) {
					case '\\':	c = '\\'; break;
					case '\'':	c = '\''; break;
					case '\"':	c = '\"'; break;
					case 'a':	c = '\a'; break;
					case 'b':	c = '\b'; break;
					case 'f':	c = '\f'; break;
					case 'n':	c = '\n'; break;
					case 'r':	c = '\r'; break;
					case 't':	c = '\t'; break;
					case 'v':	c = '\v'; break;
					case '0':	c = '\0'; break;
					
					default: break;
				}
			}
			*d++ = c;
			count--;
		}
	} else
		::strncpy(a_token.str(), m_pstart, count);

	int m_idx = m_p - _data->string;
	return (*m_p ? m_idx : -1);
}

bool String::hasNextToken(int a_index, const char* a_linecomment, const char* a_sepchars) const {
	if ((a_index < 0) || (a_index >= _data->length))
		return false;

	const char* m_p = _data->string + a_index;
	for(;;) {
		while (*m_p && is_sep_char(*m_p, a_sepchars)) {
			m_p++;
		}
		if (*m_p == 0)
			return false;

		if (a_linecomment && ::strchr(a_linecomment, *m_p))	{
			// skip line
			while (*m_p && *m_p != '\n') {
				m_p++;
			}
		} else {
			break;
		}
	}
	return true;
}


uint32 String::hashcode() const throw()
{
	uint32 hash = 0;
	const char* pchar = _data->string;
	const char* pend = pchar + _data->length;
	while ( pchar < pend )
	{
		hash = (31ul * hash) + (uint32)(*pchar++);
	}
	return hash;
}

bool String::containsAlpha() const throw()
{
	const char* pchar = _data->string;
	while ( *pchar )
	{
		if ( ::isalpha(*pchar++) )
			return true;
	}
	return false;
}
bool String::containsAlphaNum() const throw()
{
	const char* pchar = _data->string;
	while ( *pchar )
	{
		if ( ::isalnum(*pchar++) )
			return true;
	}
	return false;
}
bool String::containsDigits() const throw()
{
	const char* pchar = _data->string;
	while ( *pchar )
	{
		if ( ::isdigit(*pchar++) )
			return true;
	}
	return false;
}

int String::count(char a_c) const throw()
{
	int n = 0;
	const char* pchar = _data->string;
	while ( *pchar )
	{
		if ( *pchar++ == a_c )
			n++;
	}
	return n;
}


} // namespace
