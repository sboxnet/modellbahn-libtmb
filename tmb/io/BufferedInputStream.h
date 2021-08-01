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


#ifndef _TMB_IO_BUFFEREDINPUTSTREAM_H_
#define _TMB_IO_BUFFEREDINPUTSTREAM_H_


#include <tmb/io/InputStream.h>


namespace tmb {

def_subclass(BufferedInputStream, InputStream)
	no_copy_assign_method(BufferedInputStream)

	protected:
		InputStream&	c_stream;
		char*			c_buffer;
		int				c_bufsize;
		int				c_pread;
		int				c_phigh;


	private: void iinit(int a_bufsize);

	public: BufferedInputStream(InputStream& a_istream);
	public: BufferedInputStream(InputStream& a_istream, int a_bufsize);

	public: virtual ~BufferedInputStream();

	public: virtual int read(void* a_buffer, int a_size);
	public: virtual int read(void* a_buffer, int a_size, int a_flags);

	public: virtual void close();

	public: virtual int getInputBufSize() const;
	public: virtual int getInputBuffered() const;
	public: virtual int getInputFree() const;

	public: virtual InputStream& getInputStream() const;
};

} // namespace


#endif
