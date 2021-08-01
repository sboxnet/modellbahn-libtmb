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


#include <tmb/io/BufferedInputStream.h>

#include <cstring>

using namespace std;


namespace tmb {

enum { DEFAULT_INPUT_BUFSIZE = 1024 };

void BufferedInputStream::iinit(int a_bufsize)
{
	if ( a_bufsize < 1 )
		a_bufsize = 1;

	c_bufsize = a_bufsize;
	c_buffer = new char [c_bufsize];
	c_pread = 0;
	c_phigh = 0;
}


BufferedInputStream::BufferedInputStream(InputStream& a_istream)
						: InputStream(),
						  c_stream(a_istream)
{
	iinit(DEFAULT_INPUT_BUFSIZE);
}

BufferedInputStream::BufferedInputStream(InputStream& a_istream, int a_bufsize)
						: InputStream(),
						  c_stream(a_istream)
{
	iinit(a_bufsize);
}

BufferedInputStream::~BufferedInputStream()
{
	delete [] c_buffer;
}


int BufferedInputStream::read(void* a_buffer, int a_size)
{
	if ( !a_buffer || a_size <= 0 )
		return 0;

	const char* p = (const char*)a_buffer;
	int numread = 0;

	while ( numread < a_size )
	{
		int num = c_phigh - c_pread;
		int size = a_size - numread;
		if ( num >= size )
		{
			::memcpy((void*)p, c_buffer + c_pread, size);
			c_pread += size;
			numread += size;
			break;
		}
		else
		{
			if ( num > 0 )
			{
				::memcpy((void*)p, c_buffer + c_pread, num);
				p += num;
				numread += num;
				c_pread += num;
			}
			int r = c_stream.read(c_buffer, c_bufsize);
			if ( r == 0 )
				break;
			c_pread = 0;
			c_phigh = r;
		}
	}
	return numread;
}

int BufferedInputStream::read(void* a_buffer, int a_size, int a_flags)
{
	return InputStream::read(a_buffer, a_size, a_flags);
}

void BufferedInputStream::close()
{
	c_stream.close();
}


int BufferedInputStream::getInputBufSize() const
{
	return c_bufsize;
}

int BufferedInputStream::getInputBuffered() const
{
	return c_phigh - c_pread;
}

int BufferedInputStream::getInputFree() const
{
	return c_bufsize - c_phigh;
}


InputStream& BufferedInputStream::getInputStream() const
{
	return c_stream;
}


} // namespace
