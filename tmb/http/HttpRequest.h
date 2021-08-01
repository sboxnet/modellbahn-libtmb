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


#ifndef _TMB_HTTP_HTTPREQUEST_H_
#define _TMB_HTTP_HTTPREQUEST_H_


#include <tmb/lang/String.h>
#include <tmb/lang/HashMap.h>
#include <tmb/io/InputStream.h>
#include <tmb/http/Http.h>
#include <tmb/lang/ArrayP.h>

namespace tmb {


def_subclass_nca(HttpRequest, Http)

	public: enum Method {
					Method_UNKNOWN	= 0,
					Method_GET		= 1,
					Method_POST		= 2,
			};


	public: typedef HashMap<String> HeaderHashMap;
	public: typedef HashMap<String> ParameterHashMap;

	protected:
    	Method				c_method;
		String				c_method_name;
		HeaderHashMap		c_header;
		ParameterHashMap	c_params;
		String				c_uri;
		String				c_protokoll;
		ArrayP<HttpCookie>	c_cookies;

	public: HttpRequest();
	public: virtual ~HttpRequest();

	public: virtual void readRequest(InputStream& a_stream);
	public: virtual void reset();

	public: virtual Method getMethod() const;
	public: virtual const String& getMethodName() const;
	public: virtual const String& getHeader(const String& a_id) const;
	public: virtual const String& getParameter(const String& a_id) const;
	public: virtual bool getHeader(const String& a_id, String& a_val) const;
	public: virtual bool getParameter(const String& a_id, String& a_val) const;
	public: virtual const String& getProtokoll() const;
	public: virtual const String& getRequestURI() const;

	public: virtual HeaderHashMap& headerHashMap();
	public: virtual ParameterHashMap& parameterHashMap();

	public: const ArrayP<HttpCookie>& cookies() const { return c_cookies; }

end_class

} // namespace

#endif
