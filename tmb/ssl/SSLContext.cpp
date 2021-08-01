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


#include <tmb/ssl/SSLContext.h>
#include <tmb/ssl/SSLKey.h>


#include <openssl/ssl.h>
#include <openssl/ssl3.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#include <string.h>


#ifdef WIN32
# include <process.h>
#else
# include <unistd.h>
#endif

#include <time.h>



namespace tmb {

int SSLContext::c_is_initialized = 0;
Mutex SSLContext::c_init_mutex;


String SSLContext::getErrorMessage()
{
	ulong err;
	String msg(256);
	bool first = true;

	while ( (err = ::ERR_get_error()) != 0 )
	{
		char buffer[256];
		::ERR_error_string_n(err, buffer, sizeof(buffer));
		if ( !first )
			msg << "\n";
		else
			first = false;
		buffer[sizeof(buffer)-1] = 0;
		msg << buffer;
	}

	return msg;
}


SSLContext::SSLContext(SSLContext::Method a_method)
					: c_sslctx(0), c_prkey_passwd(0)
{
	initSSLlibrary();

	const SSL_METHOD* method = NULL;
	switch ( a_method )
	{
		case Method_SSLv2:
			method = ::SSLv23_method();
			break;
		case Method_SSLv3:
			method = ::SSLv3_method();
			break;
		case Method_TLSv1:
			method = ::TLSv1_method();
			break;
		case Method_SSLv23:
		default:
			method = ::SSLv23_method();
			break;
	}
	if ( !method )
		throw SSLException("SSLContext: unabled to get SSL method");

	SSL_CTX* ctx = ::SSL_CTX_new(method);
	if ( !ctx )
		throw SSLException("SSLContext: failed to get SSL context");

	c_sslctx = ctx;
}

SSLContext::~SSLContext()
{
	if ( c_sslctx )
	{
		::SSL_CTX_free((SSL_CTX*)c_sslctx);
		c_sslctx = 0;
	}

	c_init_mutex.lock();
	if ( c_is_initialized-- == 1 )
	{
		::ERR_free_strings();
	}
	c_init_mutex.unlock();
}

void SSLContext::initSSLlibrary()
{
	c_init_mutex.lock();

	if ( c_is_initialized++ > 0 )
	{
		c_init_mutex.unlock();
		return;
	}

	::SSL_load_error_strings();
	::SSL_library_init();

	// seed Pseudo Random Generator (SSL uses /dev/urandom if available,
	// but if not we have to seed the PRNG)
	struct SeedBuf
	{
		int		pid;
		void*	p1;
		time_t	time;
		void*	p2;
	} seedbuf = { ::getpid(), &seedbuf, ::time(NULL), this};
	::RAND_seed(&seedbuf, sizeof(seedbuf));

	c_init_mutex.unlock();
}

int SSLContext::checkFileType(SSLContext::FileType a_filetype)
{
	switch ( a_filetype )
	{
		case FileType_PEM:	return SSL_FILETYPE_PEM;
		case FileType_ASN1:	return SSL_FILETYPE_ASN1;

		default:
			break;
	}
	throw SSLException("SSLContext::checkFileType(): unknown file type");
}

int SSLContext::checkKeyType(SSLContext::KeyType a_keytype)
{
	switch ( a_keytype )
	{
		case KeyType_RSA:	return EVP_PKEY_RSA;
		case KeyType_DSA:	return EVP_PKEY_DSA;

		default:
			break;
	}
	throw SSLException("SSLContext::checkKeyType(): unknown key type");
}

void SSLContext::genRandom(void* a_data, int a_size)
{
	int r = ::RAND_bytes((unsigned char*)a_data, a_size);
	if ( r <= 0 )
	{
		throw SSLException("SSLContext::genRandom(): random function failed");
	}
}
unsigned long SSLContext::genRandom()
{
	unsigned long rnd;
	genRandom(&rnd, sizeof(rnd));
	return rnd;
}

void SSLContext::useCertificateFile(const String& a_filename, SSLContext::FileType a_filetype)
{
	if ( a_filename.length() == 0 )
		throw SSLException(String("SSLContext::useCertificateFile(): empty file name"));

	SSL_CTX* ctx = (SSL_CTX*)c_sslctx;

	int filetype = checkFileType(a_filetype);
	int r = ::SSL_CTX_use_certificate_file(ctx, a_filename.c_str(), filetype);
	if ( r <= 0 )
	{
		throw SSLException(String("SSLContext::useCertificateFile(): can not load certificate file\n") + getErrorMessage());
	}
}

void SSLContext::useCertificateASN1(const void* a_pcert, int a_size)
{
	SSL_CTX* ctx = (SSL_CTX*)c_sslctx;
	int r = ::SSL_CTX_use_certificate_ASN1(ctx, a_size, (unsigned char*)a_pcert);
	if ( r <= 0 )
	{
		throw SSLException(String("SSLContext::useCertificateASN1(): can not set certificate\n") + getErrorMessage());
	}
}

void SSLContext::useCertificateChainFile(const String& a_filename)
{
	if ( a_filename.length() == 0 )
		throw SSLException(String("SSLContext::useCertificateChainFile(): empty file name"));

	SSL_CTX* ctx = (SSL_CTX*)c_sslctx;

	int r = ::SSL_CTX_use_certificate_chain_file(ctx, a_filename.c_str());
	if ( r <= 0 )
	{
		throw SSLException(String("SSLContext::useCertificateChainFile(): can not load certificate chain file\n") + getErrorMessage());
	}
}


void SSLContext::usePrivateKeyFile(const String& a_filename, SSLContext::FileType a_filetype)
{
	if ( a_filename.length() == 0 )
		throw SSLException(String("SSLContext::usePrivateKeyFile(): empty file name"));

	SSL_CTX* ctx = (SSL_CTX*)c_sslctx;

	int filetype = checkFileType(a_filetype);
	int r = ::SSL_CTX_use_PrivateKey_file(ctx, a_filename.c_str(), filetype);
	if ( r <= 0 )
	{
		throw SSLException(String("SSLContext::usePrivateKeyFile(): can not load private key file\n") + getErrorMessage());
	}
}

void SSLContext::usePrivateKeyASN1(const void* a_pkey, int a_size, SSLContext::KeyType a_keytype)
{
	SSL_CTX* ctx = (SSL_CTX*)c_sslctx;

	int pk = checkKeyType(a_keytype);
	int r = ::SSL_CTX_use_PrivateKey_ASN1(pk, ctx, (unsigned char*)a_pkey, a_size);
	if ( r <= 0 )
	{
		throw SSLException(String("SSLContext::usePrivateKeyASN1(): can not set private key\n") + getErrorMessage());
	}
}

void SSLContext::usePrivateKey(const SSLKey& a_key)
{
	EVP_PKEY* evp = (EVP_PKEY*)a_key.getEVP_PKEY();
	if ( !evp )
		throw SSLException("SSLContext::usePrivateKey(): can not set an empty SSLKey");

	SSL_CTX* ctx = (SSL_CTX*)c_sslctx;

	int r = ::SSL_CTX_use_PrivateKey(ctx, evp);
	if ( r <= 0 )
	{
		throw SSLException(String("SSLContext::usePrivateKey(): can not set private key\n") + getErrorMessage());
	}
}


void SSLContext::setPrivateKeyPassword(SSLPassword* a_passwd)
{
	SSL_CTX* ctx = (SSL_CTX*)c_sslctx;

	if ( a_passwd )
	{
		::SSL_CTX_set_default_passwd_cb(ctx, SSLPassword::_pw_callback);
		::SSL_CTX_set_default_passwd_cb_userdata(ctx, (void*)a_passwd);
	}
	else
	{
		::SSL_CTX_set_default_passwd_cb(ctx, NULL);
		::SSL_CTX_set_default_passwd_cb_userdata(ctx, NULL);
	}
	c_prkey_passwd = a_passwd;
}


void SSLContext::loadVerifyLocations(const String& a_filename)
{
	if ( a_filename.length() == 0 )
		throw SSLException(String("SSLContext::loadVerifyLocations(): empty file name"));

	SSL_CTX* ctx = (SSL_CTX*)c_sslctx;

	int r = ::SSL_CTX_load_verify_locations(ctx, a_filename.c_str(), 0);
	if ( r <= 0 )
	{
		throw SSLException(String("SSLContext::loadVerifyLocations(): can not load verify location\n") + getErrorMessage());
	}
#if (OPENSSL_VERSION_NUMBER < 0x0090600fL)
	::SSL_CTX_set_verify_depth(ctx, 1);
#endif
}

void SSLContext::setVerifyDepth(int a_depth)
{
	SSL_CTX* ctx = (SSL_CTX*)c_sslctx;

	::SSL_CTX_set_verify_depth(ctx, a_depth);
}

void SSLContext::setVerify(int a_flag)
{
	SSL_CTX* ctx = (SSL_CTX*)c_sslctx;

	int flag = 0;

	if ( a_flag & VerifyFlag_PEER )
		flag |= SSL_VERIFY_PEER;

	if ( a_flag & VerifyFlag_FAIL_IF_NO_PEER_CERT )
		flag |= SSL_VERIFY_FAIL_IF_NO_PEER_CERT;

	if ( a_flag & VerifyFlag_CLIENT_ONCE )
		flag |= SSL_VERIFY_CLIENT_ONCE;

	::SSL_CTX_set_verify(ctx, flag, NULL);
}

void SSLContext::checkPrivateKey()
{
	SSL_CTX* ctx = (SSL_CTX*)c_sslctx;

	int r = ::SSL_CTX_check_private_key(ctx);
	if ( r <= 0 )
	{
		throw SSLException(String("SSLContext::checkPrivateKey(): failed to check private key\n") + getErrorMessage());
	}
}


} // namespace
