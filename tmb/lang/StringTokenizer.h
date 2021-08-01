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


#ifndef _TMB_LANG_STRINGTOKENIZER_H_
#define _TMB_LANG_STRINGTOKENIZER_H_


#include <tmb/lang/String.h>
#include <tmb/lang/Exception.h>

namespace tmb {

class StringTokenizer
{
	protected:
		String 	_string;
		String	_token;
		int		_index;
		String	_linecomment;
        String	_quotes;
		bool	_use_escapes;
		String	_sep_chars;

	public: StringTokenizer()
				: _index(0)
			{
			}

	public: StringTokenizer(const String& a_string, const char* a_linecomment = 0,
							const char* a_quotes = 0, int a_tokenbufsize = 32) throw()
				: _string(a_string),
				  _token(a_tokenbufsize),
				  _index(0),
				  _linecomment(a_linecomment),
				  _quotes(a_quotes),
				  _use_escapes(false)
			{
			}
	public: StringTokenizer(const String& a_string, const char* a_linecomment,
							const char* a_quotes, bool a_use_escapes, int a_tokenbufsize = 32) throw()
				: _string(a_string),
				  _token(a_tokenbufsize),
				  _index(0),
				  _linecomment(a_linecomment),
				  _quotes(a_quotes),
				  _use_escapes(a_use_escapes)
			{
			}
	public: StringTokenizer(const String& a_string, const char* a_linecomment,
							const char* a_quotes, bool a_use_escapes, const char* a_sepchars,
							int a_tokenbufsize = 32) throw()
				: _string(a_string),
				  _token(a_tokenbufsize),
				  _index(0),
				  _linecomment(a_linecomment),
				  _quotes(a_quotes),
				  _use_escapes(a_use_escapes),
				  _sep_chars(a_sepchars)
			{
			}

	private: StringTokenizer(const StringTokenizer&);
	private: void operator=(const StringTokenizer&);

	public: ~StringTokenizer()
			{
			}

	public: const String& next();

	public: void reset() throw()
			{
				_index = 0;
			}

	public: void set(const String& a_s)
			{
				_string = a_s;
				_index = 0;
			}

	public: void setCommentChars(const String& a_s)
			{
				_linecomment = a_s;
			}

	public: void setQuoteChars(const String& a_s)
			{
				_quotes = a_s;
			}
			
	public: void setUseEscapes(bool a_yes) {
				_use_escapes = a_yes;
			}
			
	public: void setSeparatorChars(const String& a_s)
			{
				_sep_chars = a_s;
			}
	
	public: bool hasNext();
};


} // namespace


#endif
