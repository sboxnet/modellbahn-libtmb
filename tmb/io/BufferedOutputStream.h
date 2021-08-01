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



#ifndef _TMB_IO_BUFFEREDOUTPUTSTREAM_H_
#define _TMB_IO_BUFFEREDOUTPUTSTREAM_H_


#include <tmb/io/OutputStream.h>


namespace tmb {

def_subclass(BufferedOutputStream, OutputStream)
	no_copy_assign_method(BufferedOutputStream)

	protected:
		OutputStream&	c_stream;
		char*			c_buffer;
		int				c_bufsize;
		int				c_pwrite;
		int				c_plow;


	private: void iinit(int a_bufsize);

	public: BufferedOutputStream(OutputStream& a_ostream);
	public: BufferedOutputStream(OutputStream& a_ostream, int a_bufsize);

	public: virtual ~BufferedOutputStream();

	public: virtual int write(const void* a_buffer, int a_size);
	public: virtual int write(const void* a_buffer, int a_size, int a_flags);
	public: virtual int write(const String& a_s, int a_flags = WriteFlag_NONE);

	public: virtual void close();


	public: virtual void flush();

	public: virtual int getOutputBufSize() const;
	public: virtual int getOutputBuffered() const;
	public: virtual int getOutputFree() const;

	public: virtual OutputStream& getOutputStream() const;
};

} // namespace

#endif
