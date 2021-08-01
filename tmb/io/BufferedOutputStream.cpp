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


#include <tmb/io/BufferedOutputStream.h>

#include <cstring>

using namespace std;


namespace tmb {


enum { DEFAULT_OUTPUT_BUFSIZE = 1024 };

void BufferedOutputStream::iinit(int a_bufsize)
{
	if ( a_bufsize < 1 )
		a_bufsize = 1;

	c_bufsize = a_bufsize;
	c_buffer = new char [c_bufsize];
	c_pwrite = 0;
	c_plow = 0;
}


BufferedOutputStream::BufferedOutputStream(OutputStream& a_ostream)
						: OutputStream(),
						  c_stream(a_ostream)
{
	iinit(DEFAULT_OUTPUT_BUFSIZE);
}

BufferedOutputStream::BufferedOutputStream(OutputStream& a_ostream, int a_bufsize)
						: OutputStream(),
						  c_stream(a_ostream)
{
	iinit(a_bufsize);
}

BufferedOutputStream::~BufferedOutputStream()
{
	delete [] c_buffer;
}


int BufferedOutputStream::write(const void* a_buffer, int a_size)
{
	if ( !a_buffer || a_size <= 0 )
		return 0;

	const char* p = (const char*)a_buffer;
	int size = a_size;

	while ( size > 0 )
	{
		int freespace = c_bufsize - c_pwrite;
		if ( freespace <= 0 )
		{
			flush();
		}
		else if ( size <= freespace )
		{
			::memcpy(c_buffer + c_pwrite, p, size);
			c_pwrite += size;
			if ( c_pwrite >= c_bufsize )
				flush();
			break;
		}
		else
		{
			::memcpy(c_buffer + c_pwrite, p, freespace);
			c_pwrite += freespace;
			p += freespace;
			size -= freespace;
		}
	}
	return a_size;
}

int BufferedOutputStream::write(const void* a_buffer, int a_size, int a_flags)
{
	return OutputStream::write(a_buffer, a_size, a_flags);
}

int BufferedOutputStream::write(const String& a_s, int a_flags)
{
	return OutputStream::write(a_s, a_flags);
}


void BufferedOutputStream::close()
{
	try
	{
		flush();
	}
	catch(...)
	{
	}

	c_stream.close();
}

void BufferedOutputStream::flush()
{
	int size = c_pwrite;
	while ( size > 0 )
	{
		int r = c_stream.write(c_buffer + c_plow, size);
		size -= r;
		c_plow += r;
	}
	c_pwrite = 0;
	c_plow = 0;
}


int BufferedOutputStream::getOutputBufSize() const
{
	return c_bufsize;
}

int BufferedOutputStream::getOutputBuffered() const
{
	return c_pwrite - c_plow;
}

int BufferedOutputStream::getOutputFree() const
{
	return c_bufsize - c_pwrite;
}


OutputStream& BufferedOutputStream::getOutputStream() const
{
	return c_stream;
}


} // namespace
