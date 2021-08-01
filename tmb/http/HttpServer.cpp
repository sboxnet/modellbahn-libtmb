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



#include <tmb/http/HttpServer.h>

#include <tmb/io/File.h>

namespace tmb {


HttpServer::HttpServer(const String& a_root_dir)
				: c_root_dir(a_root_dir)
{
}

HttpServer::HttpServer(const String& a_root_dir, const String& a_name)
				: c_root_dir(a_root_dir),
				  c_server_name(a_name)
{
}

HttpServer::~HttpServer()
{
}

const String& HttpServer::getRootDir() const
{
	return c_root_dir;
}

void HttpServer::setServerName(const String& a_name)
{
	c_server_name = a_name;
}

const String& HttpServer::getServerName() const
{
	return c_server_name;
}

void HttpServer::serve(InputStream& a_input_stream, OutputStream& a_output_stream)
{
	HttpRequest		request;
	HttpResponse	response;

	response.setHeader("Server", c_server_name);

	request.readRequest(a_input_stream);

	if ( validateRequest(request, response) )
	{
		execRequest(request, response);
	}

	response.writeResponse(a_output_stream);
}

bool HttpServer::validateRequest(HttpRequest& a_request, HttpResponse& a_response)
{
	int r = 0;

	const String& uri = a_request.getRequestURI();

	if ( uri.isEmpty() )
		r = 404; // Not Found
	else if ( uri.indexOf("..") >= 0 )
		r = 404;
	else if ( uri[0] != '/' )
		r = 404;
	else if ( uri.length() > 256 )
		r = 414; // URI too long

	if ( r )
	{
		a_response.setRetcode(r);
		return false;
	}

	return true;
}

void HttpServer::execRequest(HttpRequest& a_request, HttpResponse& a_response)
{
	// default implementation

	String uri = a_request.getRequestURI();
	if ( !uri.startsWith('/') )
		uri.insert('/', 0);
	uri.replace('/', File::getFileSeparator());


	File f(c_root_dir + uri);
	try
	{
		ObjPtr<ByteArray> cnt(new ByteArray());
		f.load(*cnt);
		a_response.setContent(cnt.detach());
	}
	catch(const Exception&)
	{
		a_response.setRetcode(404); // not found
		return;
	}

	String mimetype = Http::getMimeTypeFromFileName(uri);
	if ( mimetype.isEmpty() )
		mimetype = "text/plain";

	a_response.setContentType(mimetype);
}


} // namespace
