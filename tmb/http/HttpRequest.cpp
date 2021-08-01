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


#include <tmb/http/HttpRequest.h>
#include <tmb/lang/StringTokenizer.h>

#include <iostream>


using namespace std;



namespace tmb {


HttpRequest::HttpRequest()
				: super(),
				  c_method(Method_UNKNOWN)
{
}

HttpRequest::~HttpRequest()
{
}


void HttpRequest::readRequest(InputStream& a_stream)
{
	reset();

	String line(2048);
	StringTokenizer tz;
	int numread;

#define READ_LINE() if ( (numread = a_stream.readLine(line, '\n', 2048, "\r")) == 0 ) throw ParseException()

	try
	{
		// read the first line of the request (GET or POST)
		{
			READ_LINE();
			tz.set(line);
			String method = tz.next();
			c_uri = Http::unescape(tz.next());
			c_protokoll = tz.next(); //   HTTP/x.y

			if ( method == "GET" )
				c_method = Method_GET;
			else if ( method == "POST" )
				c_method = Method_POST;
			else
				throw ParseException();
			c_method_name = method;

			if ( !c_protokoll.startsWith("HTTP/") )
				throw ParseException();

			int idx = c_uri.indexOf('?');
			if ( idx >= 0 )
			{
				String query = c_uri.substring(idx+1);
				c_uri = c_uri.substring(0, idx);

				if ( c_method == Method_GET && query.length() > 0 )
				{
					idx = 0;
					while(true)
					{
						int fndidx = query.indexOf('&', idx);

						String param = query.substring(idx, fndidx);
						if ( param.length() > 0 )
						{
							int paridx = param.indexOf('=', 0);
							if ( paridx > 0 )
							{
								String paramkey = param.substring(0, paridx).lowerCase();
								String paramval = Http::unescape(param.substring(paridx + 1));

								c_params.put(paramkey, paramval);
							}
						}
						if ( fndidx < 0 )
							break;
						idx = fndidx + 1;
					}
				}
			}
		}

		// read header
		while(true)
		{
			READ_LINE();
			if ( line.length() == 0 ) // header end marked by empty line
				break;

			int idx = line.indexOf(':', 0);
			if ( idx > 0 )
			{
				String key = line.substring(0, idx).lowerCase();
				String val = line.substring(idx + 2); // one space after the ':' !

				if ( key == "cookie" )
				{

					continue;
				}

				try
				{
					String curval = c_header.get(key);
					c_header.put(key, curval + " " + val); // if key already exists, concat values
				}
				catch(const NotFoundException&)
				{
					c_header.put(key, val);
				}
			}
		}

		// do we have a content?
		String cntlenstr;
		try
		{
			cntlenstr = c_header.get("content-length");
			long cntlen = cntlenstr.toLong(10);
			if ( cntlen < 0 || cntlen > 10*1024*1024 ) // max 10 MB input content length
				throw ParseException("invalid or to huge content length");
			while ( cntlen > 0 )
			{
				if ( (numread = a_stream.readLine(line, '\n', cntlen, "\r")) == 0 )
					throw ParseException();
				cntlen -= numread;
				if ( c_method == Method_POST && line.length() > 0 )
				{
					int idx = 0;
					while(true)
					{
						int fndidx = line.indexOf('&', idx);
						String param = line.substring(idx, fndidx);
						if ( param.length() > 0 )
						{
							int paridx = param.indexOf('=', 0);
							if ( paridx > 0 )
							{
								String paramkey = param.substring(0, paridx).lowerCase();
								String paramval = Http::unescape(param.substring(paridx + 1));
								c_params.put(paramkey, paramval);
							}
						}
						if ( fndidx < 0 )
							break;
						idx = fndidx + 1;
					}
				}
			}
		}
		catch(const NotFoundException&)
		{
			// no content
		}
		catch(const NumberFormatException&)
		{
			// no content
		}
	}
	catch(const NotFoundException&)
	{
		throw ParseException();
	}
}

void HttpRequest::reset()
{
	c_method = Method_UNKNOWN;
	c_method_name.clear();
	c_header.clear();
	c_params.clear();
	c_uri.clear();
	c_protokoll.clear();
}

HttpRequest::Method HttpRequest::getMethod() const
{
	return c_method;
}
const String& HttpRequest::getMethodName() const
{
	return c_method_name;
}

const String& HttpRequest::getHeader(const String& a_id) const
{
	return c_header.get(a_id.getLowerCase());
}

const String& HttpRequest::getParameter(const String& a_id) const
{
	return c_params.get(a_id.getLowerCase());
}

bool HttpRequest::getHeader(const String& a_id, String& a_val) const
{
	try
	{
		a_val = c_header.get(a_id.getLowerCase());
		return true;
	}
	catch(const NotFoundException&)
	{
	}
	return false;
}

bool HttpRequest::getParameter(const String& a_id, String& a_val) const
{
	try
	{
		a_val = c_params.get(a_id.getLowerCase());
		return true;
	}
	catch(const NotFoundException&)
	{
	}
	return false;
}


const String& HttpRequest::getProtokoll() const
{
	return c_protokoll;
}

const String& HttpRequest::getRequestURI() const
{
	return c_uri;
}

HttpRequest::HeaderHashMap& HttpRequest::headerHashMap()
{
	return c_header;
}

HttpRequest::ParameterHashMap& HttpRequest::parameterHashMap()
{
	return c_params;
}


} // namespace
