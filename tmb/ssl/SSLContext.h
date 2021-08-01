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


#ifndef _TMB_SSL_SSLCONTEXT_H_
#define _TMB_SSL_SSLCONTEXT_H_


#include <tmb/lang/String.h>
#include <tmb/thread/Mutex.h>

#include <tmb/ssl/SSLException.h>
#include <tmb/ssl/SSLPassword.h>



namespace tmb {


class SSLKey;


/**
 * class SSLContext: a SSL context.
 */
class SSLContext
{
	public: static String getErrorMessage();

	public: enum Method {
					Method_SSLv23,
					Method_SSLv2,
					Method_SSLv3,
					Method_TLSv1,
			};

	public: enum FileType {
					FileType_PEM,
					FileType_ASN1,
			};

	public: enum VerifyFlag {
					VerifyFlag_NONE					= 0x0000,
					VerifyFlag_PEER					= 0x0001,
					VerifyFlag_FAIL_IF_NO_PEER_CERT = 0x0002,
					VerifyFlag_CLIENT_ONCE			= 0x0004,
			};

	public: enum KeyType {
					KeyType_UNKNOWN = 0,
					KeyType_RSA = 1,
					KeyType_DSA = 2,
			};


	private:
		static int		c_is_initialized;
		static Mutex	c_init_mutex;

	protected:
		void*			c_sslctx;
		SSLPassword*	c_prkey_passwd;


	public: explicit SSLContext(Method a_method = Method_SSLv23);

	private: SSLContext(const SSLContext&);
	private: void operator=(const SSLContext&);

	public: virtual ~SSLContext();


	public: void* getSSL_CTX() { return c_sslctx; }

	private: void initSSLlibrary();
	public: static int  checkFileType(SSLContext::FileType a_filetype);
	public: static int  checkKeyType(SSLContext::KeyType a_keytype);

	public: void genRandom(void* a_data, int a_size);
	public: unsigned long genRandom();


	public: void useCertificateFile(const String& a_filename, SSLContext::FileType a_filetype = FileType_PEM);
	public: void useCertificateASN1(const void* a_pcert, int a_size);

	public: void useCertificateChainFile(const String& a_filename);

	public: void usePrivateKeyFile(const String& a_filename, SSLContext::FileType a_filetype = FileType_PEM);
	public: void usePrivateKeyASN1(const void* a_pkey, int a_size, SSLContext::KeyType a_keytype = KeyType_RSA);
	public: void usePrivateKey(const SSLKey& a_key);

	public: void setPrivateKeyPassword(SSLPassword* a_passwd);

	public: void loadVerifyLocations(const String& a_filename);

	public: void setVerifyDepth(int a_depth);
	public: void setVerify(int a_flags);

	public: void checkPrivateKey();
};


} // namespace

#endif
