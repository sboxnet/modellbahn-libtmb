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



#include <tmb/http/HttpResponse.h>

namespace tmb {


HttpResponse::HttpResponse()
				: super(),
				  c_content(),
				  c_protokoll("HTTP/1.0"),
				  c_retcode(200)
{
}

HttpResponse::HttpResponse(int a_cntbufsize)
				: super(),
				  c_content(a_cntbufsize),
				  c_protokoll("HTTP/1.0"),
				  c_retcode(200)
{
}

HttpResponse::HttpResponse(const String& a_proto, int a_cntbufsize)
				: super(),
				  c_content(a_cntbufsize),
				  c_protokoll(a_proto),
				  c_retcode(200)
{
}

HttpResponse::~HttpResponse()
{
}

void HttpResponse::setContentBufSize(int a_bufsize)
{
	c_content.setMinBufSize(a_bufsize);
}

void HttpResponse::setContent(const String& a_cnt)
{
	c_content = a_cnt;
}

String& HttpResponse::content()
{
	return c_content;
}

void HttpResponse::setContent(ByteArray* a_data)
{
	c_binary_content.replace(a_data);
}

void HttpResponse::setContent(XmlTemplate* a_data)
{
	c_template_content.replace(a_data);
}


const String& HttpResponse::getHeader(const String& a_id) const
{
	return c_header.get(a_id);
}

void HttpResponse::setHeader(const String& a_id, const String& a_val)
{
	if ( a_id.length() > 0 )
		c_header.put(a_id, a_val);
}

void HttpResponse::setContentType(const String& a_type)
{
	setHeader("Content-Type", a_type);
}

HttpResponse::HeaderHashMap& HttpResponse::headerHashMap()
{
	return c_header;
}

const String& HttpResponse::getProtokoll() const
{
	return c_protokoll;
}

void HttpResponse::setProtokoll(const String& a_proto)
{
	c_protokoll = a_proto;
}

int HttpResponse::getRetcode() const
{
	return c_retcode;
}

void HttpResponse::setRetcode(int a_retcode)
{
	c_retcode = a_retcode;
}


void HttpResponse::writeResponse(OutputStream& a_stream)
{
	String header(4096);

	const char* sret = 0;
	switch ( c_retcode )
	{
		case 100:	sret = "Continue";				break;
		case 101:	sret = "Switching Protocols";	break;

		case 200:	sret = "OK";					break;
		case 201:	sret = "Created";				break;
		case 202:	sret = "Accepted";				break;
		case 203:	sret = "Non-Authoritative Information"; break;
		case 204:	sret = "No New Content";				break;
		case 205:	sret = "Reset Content";			break;
		case 206:	sret = "Partial Content";		break;

		case 300:	sret = "Multiple Choice";		break;
		case 301:	sret = "Moved Permanently";		break;
		case 302:	sret = "Moved Temporarily";		break;
		case 303:	sret = "See Other";				break;
		case 304:	sret = "Not Modified";			break;
		case 305:	sret = "Use Proxy";				break;

		case 400:	sret = "Bad Request";			break;
		case 401:	sret = "Access Denied";			break;
		case 402:	sret = "Payment Required";		break;
		case 403:	sret = "Forbidden";				break;
		case 404:	sret = "Not Found";				break;
		case 405:	sret = "Method Not Allowed";	break;
		case 406:	sret = "Not Acceptable";		break;
		case 407:	sret = "Proxy Authentication Required"; break;
		case 408:	sret = "Request Timeout";		break;
		case 409:	sret = "Conflict";				break;
		case 410:	sret = "Gone";					break;
		case 411:	sret = "Length Required";		break;
		case 412:	sret = "Precondition Failed";	break;
		case 413:	sret = "Request Entity Too Large"; break;
		case 414:	sret = "Request-URI Too Long";	break;
		case 415:	sret = "Unsupported Media Type"; break;

		case 500:	sret = "Internal Server Error";	break;
		case 501:	sret = "Not Implemented";		break;
		case 502:	sret = "Bad Gateway";			break;
		case 503:	sret = "Service Unavailable";	break;
		case 504:	sret = "Gateway Timeout";		break;
		case 505:	sret = "HTTP Version Not Supported"; break;
		default:	sret = "Unknown Error";			break;
	}
	if ( c_retcode >= 400 && c_retcode < 600 )
	{
		// use default error html page
		setContentType("text/html");
		c_content = "<html><head><title>";
		c_content << sret << "</title></head><body>"
		          << "HTTP Error " << c_retcode << " " << sret << "</body></html>\n";

		c_binary_content.release();
		c_template_content.release();
	}

	header << c_protokoll << " " << c_retcode << " " << sret << "\n";

    if ( c_template_content )
	{
	}
	else if ( c_binary_content )
	{
		c_header.put("Content-Length", String(32) << c_binary_content->size());
    }
	else
	{
		c_header.put("Content-Length", String(32) << c_content.length());
	}

	HeaderHashMap::Key hmkey = 0;
	String sval;
	const String* pkeystr;
	while ( (hmkey = c_header.nextKey(hmkey, sval, &pkeystr)) != 0 )
	{
		String skey;
		if ( pkeystr )
			skey = *pkeystr;
		header << skey << ": " << sval << "\n";
	}

	header << "\n";

	a_stream.write(header, OutputStream::WriteFlag_FORCECS);

	if ( c_template_content )
	{
		c_template_content->writeTo(a_stream);
	}
	else if ( c_binary_content )
	{
		if ( c_binary_content->size() > 0 )
			a_stream.write(c_binary_content->data(), c_binary_content->size(),
							OutputStream::WriteFlag_FORCECS);
	}
	else
	{
		if ( c_content.length() > 0 )
			a_stream.write(c_content, OutputStream::WriteFlag_FORCECS);
	}

	c_binary_content.release();
	c_template_content.release();
	c_content.clear();
}

} // namespace
