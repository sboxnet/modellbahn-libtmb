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


#ifndef _TMB_HTTP_HTTPRESPONSE_H_
#define _TMB_HTTP_HTTPRESPONSE_H_


#include <tmb/lang/String.h>
#include <tmb/lang/Array.h>
#include <tmb/lang/HashMap.h>
#include <tmb/io/OutputStream.h>
#include <tmb/http/Http.h>
#include <tmb/xml/XmlTemplate.h>


namespace tmb {

def_subclass_nca(HttpResponse, Http)

	public: typedef HashMap<String> HeaderHashMap;

	protected:
		HeaderHashMap			c_header;
		String					c_content;
		ObjPtr<ByteArray>		c_binary_content;
		ObjPtr<XmlTemplate>		c_template_content;
		String					c_protokoll;
		int						c_retcode;

	public: HttpResponse();
	public: HttpResponse(int a_cntbufsize);
	public: HttpResponse(const String& a_proto, int a_cntbufsize);

	public: virtual ~HttpResponse();

	public: virtual void setContentBufSize(int a_bufsize);
	public: virtual void setContent(const String& a_cnt);
	public: virtual String& content();
	public: virtual void setContent(ByteArray* a_data);
	public: virtual void setContent(XmlTemplate* a_tmpl);

	public: virtual const String& getHeader(const String& a_id) const;
	public: virtual void setHeader(const String& a_id, const String& a_val);
	public: virtual void setContentType(const String& a_type);

	public: virtual const String& getProtokoll() const;
	public: virtual void setProtokoll(const String& a_proto);

	public: virtual int getRetcode() const;
	public: virtual void setRetcode(int a_retcode);

	public: virtual HeaderHashMap& headerHashMap();

	public: virtual void writeResponse(OutputStream& a_stream);

end_class

} // namespace

#endif
