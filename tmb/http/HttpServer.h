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



#ifndef _TMB_HTTP_HTTPSERVER_H_
#define _TMB_HTTP_HTTPSERVER_H_


#include <tmb/lang/String.h>
#include <tmb/http/Http.h>
#include <tmb/http/HttpRequest.h>
#include <tmb/http/HttpResponse.h>
#include <tmb/io/OutputStream.h>
#include <tmb/io/InputStream.h>


namespace tmb {

def_subclass(HttpServer, Http)
	no_copy_assign_method(HttpServer)

	protected:
		String	c_root_dir;
		String	c_server_name;

	public: HttpServer(const String& a_root_dir);
	public: HttpServer(const String& a_root_dir, const String& a_name);
	public: virtual ~HttpServer();

	public: virtual const String& getRootDir() const;

	public: virtual void setServerName(const String& a_name);
	public: virtual const String& getServerName() const;

	public: virtual void serve(InputStream& a_input_stream, OutputStream& a_output_stream);

	public: virtual bool validateRequest(HttpRequest& a_request, HttpResponse& a_response);

	public: virtual void execRequest(HttpRequest& a_request, HttpResponse& a_response);
};



} // namespace

#endif
