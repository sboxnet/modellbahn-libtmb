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


#ifndef _TMB_IO_INPUTSTREAM_H_
#define _TMB_IO_INPUTSTREAM_H_


#include <tmb/lang/Exception.h>
#include <tmb/lang/String.h>

namespace tmb {

def_class_nca(InputStream)

	public: enum ReadFlag  { ReadFlag_NONE		  = 0,
							 ReadFlag_EXCSIZEERR  = 0x0002,
							 ReadFlag_FORCECS     = 0x0004,
			};

	public: InputStream();
	public: virtual ~InputStream();

	public: virtual int read(void* a_buffer, int a_size) = 0;

	public: virtual int read(void* a_buffer, int a_size, int a_flags);

	public: virtual int readLine(String& a_line, char a_eolc = '\n',
								int a_max = 0, const char* a_ignorec = 0);

	public: virtual void close() = 0;
end_class

} // namespace

#endif
