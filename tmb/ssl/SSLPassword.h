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


#ifndef _TMB_SSL_SSLPASSWORD_H_
#define _TMB_SSL_SSLPASSWORD_H_


#include <tmb/lang/String.h>
#include <tmb/ssl/SSLException.h>


namespace tmb {

class SSLPassword
{
	protected:
		String c_passwd;

	public: explicit SSLPassword();
	public: explicit SSLPassword(const String& a_passwd);
	public: virtual ~SSLPassword();

	public: void set(const String& a_passwd);
	public: const String& get(bool a_for_encryption = false);

	public: static int _pw_callback(char* a_buf, int a_size, int a_rwflag, void* a_passwdobj);

};

} // namespace

#endif
