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


#include <tmb/http/Http.h>

namespace tmb {


Http::Http()
{
}

Http::~Http()
{
}

String Http::unescape(const String& a_s)
{
	int len = a_s.length();
	if ( len == 0 )
		return a_s;

	String s(len+1);

	const char* p = a_s.c_str();
	const char* pend = p + len;
	while ( p < pend )
	{
		char c = *p++;
		switch (c)
		{
			case '%':
				if ( p + 2 <= pend )
				{
					String hexc(4);
					hexc << p[0];
					hexc << p[1];
					try
					{
						c = (char)hexc.toLong(16);
						p += 2;
					}
					catch(...)
					{
					}
				}
				s << c;
				break;

			case '+':
				s << ' ';
				break;

			default:
				s << c;
				break;
		}
	}

	return s;
}


String Http::getMimeTypeFromFileName(const String& a_name)
{
	static struct _mimeext
	{
		const char* mime;
		const char* exts[4];
	} mime_exts[] = {

			{ "text/html",		{ ".html", ".htm", 0 } },
			{ "text/css",		{ ".css", 0 } },
			{ "text/richtext",	{ ".rtf", 0 } },
			{ "text/plain",		{ ".txt", 0 } },

			{ "image/jpeg",		{ ".jpeg", ".jpg", 0 } },
			{ "image/gif",		{ ".gif", 0 } },
			{ "image/bmp",		{ ".bmp", 0 } },
			{ "image/tiff",		{ ".tiff", ".tif", 0 } },
			{ "image/x-png",	{ ".png", 0 } },

			{ "video/avi",		{ ".avi", 0 } },
			{ "video/mpeg",		{ ".mpeg", 0 } },

			{ "application/x-javascript",		{ ".js", 0 } },
			{ "application/postscript",			{ ".ps", 0 } },
			{ "application/pdf",				{ ".pdf", 0 } },
			{ "application/x-zip-compressed",	{ ".zip", 0 } },
			{ "application/x-gzip-compressed",	{ ".gz", 0 } },
			{ "application/x-bzip2-compressed",	{ ".bz2", 0 } },
			{ "application/java",				{ ".class", 0 } },

			{ 0 },
	};

	int i;
	for ( i = 0; mime_exts[i].mime != 0; i++ )
	{
		int k;
		for ( k = 0; mime_exts[i].exts[k] != 0; k++ )
		{
			if ( a_name.endsWithIgnoreCase(mime_exts[i].exts[k]) )
				return String(mime_exts[i].mime);
		}
	}
	return String::null;
}


} // namespace
