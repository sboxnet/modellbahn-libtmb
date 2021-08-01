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



#ifndef _TMB_SSL_SSLKEY_H_
#define _TMB_SSL_SSLKEY_H_


#include <tmb/lang/String.h>


#include <tmb/ssl/SSLException.h>
#include <tmb/ssl/SSLPassword.h>
#include <tmb/ssl/SSLContext.h>
#include <tmb/ssl/SSLSignature.h>


namespace tmb {

class SSLKey
{
	private:
		void*				c_key;
	protected:
		SSLPassword*		c_passwd;
		SSLContext::KeyType	c_key_type;


	public: explicit SSLKey();

	public: SSLKey(const SSLKey&);
	public: void operator=(const SSLKey&);

	public: virtual ~SSLKey();

	public: void* getEVP_PKEY() const { return c_key; }

	public: SSLContext::KeyType getKeyType() const throw() { return c_key_type; }

	private: void ifreekey() throw();
	private: void icopykey(const SSLKey& a_key) throw();

	public: void loadPrivateKeyFile(const String& a_filename);
	public: void loadPrivateKeyASN1(const void* a_pkey, int a_size,
						SSLContext::KeyType a_kt = SSLContext::KeyType_RSA);

	public: void loadPublicKeyFile(const String& a_filename);
	public: void loadPublicKeyASN1(const void* a_pkey, int a_size,
						SSLContext::KeyType a_kt = SSLContext::KeyType_RSA);

	public: void setPassword(SSLPassword* a_passwd);

	public: void sign(const void* a_data, unsigned int a_size, SSLSignature& a_sign) const;

	public: void verify(const void* a_data, unsigned int a_size, const SSLSignature& a_sign) const;
};

} // namespace

#endif
