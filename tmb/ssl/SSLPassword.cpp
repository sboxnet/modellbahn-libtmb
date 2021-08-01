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

#include <tmb/ssl/SSLPassword.h>


#include <openssl/ssl.h>
#include <openssl/ssl3.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#include <string.h>

namespace tmb {


SSLPassword::SSLPassword()
{
}

SSLPassword::SSLPassword(const String& a_passwd)
				: c_passwd(a_passwd)
{
}

SSLPassword::~SSLPassword()
{
}

void SSLPassword::set(const String& a_passwd)
{
	c_passwd = a_passwd;
}

const String& SSLPassword::get(bool a_for_encryption)
{
	return c_passwd;
}



int SSLPassword::_pw_callback(char* a_buf, int a_size, int a_rwflag, void* a_passwdobj)
{
	SSLPassword* passwd = (SSLPassword*)a_passwdobj;
	if ( !passwd )
	{
		a_buf[0] = 0;
		return 0;
	}

	String strpw = passwd->get((a_rwflag != 0));

	::strncpy(a_buf, strpw.c_str(), a_size);
	a_buf[a_size - 1] = 0;

	return strpw.length();
}


} // namespace
