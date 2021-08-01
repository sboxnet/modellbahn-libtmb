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


#include <tmb/io/OutputStream.h>
#include <tmb/io/IOException.h>


namespace tmb {

OutputStream::OutputStream()
{
}

OutputStream::~OutputStream()
{
}

int OutputStream::write(const void* a_buffer, int a_size, int a_flags)
{
	int r = 0;

	if ( a_flags & WriteFlag_FORCECS )
	{
		int written = 0;
		while ( written < a_size )
		{
			try
			{
				r = write((char*)a_buffer + written, a_size - written);
				if ( r > 0 )
					written += r;
				else
					throw IOException(String("OutputStream::write(): unable to write complete data"));
			}
			catch(const TryAgainException&) {}
			catch(const InterruptedException&) {}
		}
		r = written;
	}
	else
	{
		r = write(a_buffer, a_size);

		if ( (a_flags & WriteFlag_EXCSIZEERR) && (r != a_size) )
			throw IOException(String("OutputStream::write(): unable to write complete data"));
	}
	return r;
}


int OutputStream::write(const String& a_s, int a_flags)
{
	int len = a_s.length();

	if ( a_flags & WriteFlag_STRZERO )
		len++;

	if ( len <= 0 )
		return 0;

	return write((void*)a_s.c_str(), len, a_flags);
}



} // namespace
