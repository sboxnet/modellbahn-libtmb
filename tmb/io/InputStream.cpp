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


#include <tmb/io/InputStream.h>
#include <tmb/lang/String.h>
#include <tmb/io/IOException.h>

namespace tmb {

InputStream::InputStream()
{
}

InputStream::~InputStream()
{
}

int InputStream::read(void* a_buffer, int a_size, int a_flags)
{
	int r = 0;

	if ( a_flags & ReadFlag_FORCECS )
	{
		int n = 0;
		while ( n < a_size )
		{
			try
			{
				r = read((char*)a_buffer + n, a_size - n);
				if ( r > 0 )
					n += r;
				else
					throw IOException(String("InputStream::read(): unable to read complete data"));
			}
			catch(const TryAgainException&) {}
			catch(const InterruptedException&) {}
		}
		r = n;
	}
	else
	{
		r = read(a_buffer, a_size);

		if ( (a_flags & ReadFlag_EXCSIZEERR) && (r != a_size) )
			throw IOException(String("InputStream::read(): unable to read complete data"));
	}
	return r;
}

int InputStream::readLine(String& a_line, char a_eolc, int a_max, const char* a_ignorec)
{
	int numread = 0;

	a_line.clear();

	while(true)
	{
		char c;

		if ( a_max > 0 && numread >= a_max )
			break;

		int r = read(&c, 1);
		if ( r <= 0 )
			break;
		numread++;
		if ( c == a_eolc || c == 0 )
			break;
		if ( a_ignorec )
		{
			const char* p = a_ignorec;
			bool ign = false;
			while (*p)
			{
				if ( *p++ == c )
				{
					ign = true;
					break;
				}
			}
			if (ign)
				continue;
		}
		a_line << c;
	}

	return numread;
}


} // namespace
