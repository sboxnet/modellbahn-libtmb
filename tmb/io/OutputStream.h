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


#ifndef _TMB_IO_OUTPUTSTREAM_H_
#define _TMB_IO_OUTPUTSTREAM_H_


#include <tmb/lang/String.h>

#include <tmb/lang/Exception.h>


namespace tmb {


def_class_nca(OutputStream)

	public: enum WriteFlag { WriteFlag_NONE		   = 0,
							 WriteFlag_STRZERO	   = 0x0001, // write string including terminator char '\0'
							 WriteFlag_EXCSIZEERR  = 0x0002, // exception on size error
							 WriteFlag_FORCECS     = 0x0004, // force complete size
			};

	public: OutputStream();
	public: virtual ~OutputStream();

	public: virtual int write(const void* a_buffer, int a_size) = 0;

	public: virtual int write(const void* a_buffer, int a_size, int a_flags);

	public: virtual int write(const String& a_s, int a_flags = WriteFlag_NONE);

	public: virtual void close() = 0;
end_class

} // namespace

#endif
