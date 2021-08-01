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


#ifndef _TMB_HTTP_HTTP_H_
#define _TMB_HTTP_HTTP_H_


#include <tmb/lang/String.h>


namespace tmb {

def_class(Http)
	no_copy_assign_method(Http)

	public: Http();
	public: virtual ~Http();


	public: static String unescape(const String& a_s);

	public: static String getMimeTypeFromFileName(const String& a_name);
end_class


def_class(HttpCookie)
	no_copy_assign_method(HttpCookie)

	protected:
		String	c_name;
		String	c_value;

	public: HttpCookie() {}
	public: ~HttpCookie() {}

	public: const String& name() const { return c_name; }
	public: const String& value() const { return c_value; }

	public: void setName(const String& a_name) { c_name = a_name; }
	public: void setValue(const String& a_val) { c_value = a_val; }

end_class

} // namespace

#endif
