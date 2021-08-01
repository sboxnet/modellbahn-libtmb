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

#ifndef _TMB_LANG_REGEX_H_
#define _TMB_LANG_REGEX_H_

#include <tmb/lang/String.h>
#include <tmb/lang/Exception.h>

namespace tmb {

def_class(RegEx)
	no_copy_assign_method(RegEx)

	public:
		enum Compile {
			Compile_EXTENDED	= 1,
			Compile_ICASE		= 2,
			Compile_NEWLINE		= 4,
		};
		enum Match {
			Match_NOTBOL		= 1,
			Match_NOTEOL		= 2,
		};

	protected:
		void*	_regex_t;

	public: RegEx();
	public: RegEx(const String& a_re, int a_compflags = 0);
	public: virtual ~RegEx();

	public: void compile(const String& a_re, int a_compflags = 0);

	public: bool match(const String& a_str, int a_matchflags = 0);

end_class


} // namespace

#endif
