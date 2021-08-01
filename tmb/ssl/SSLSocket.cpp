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


#include <tmb/ssl/SSLSocket.h>
#include <tmb/lang/System.h>

#include <openssl/ssl.h>
#include <openssl/ssl3.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#include <string.h>

namespace tmb {

// helper function
static String soErrorString(SSLSocket* a_sock)
{
	String s(128);
	int eno = a_sock->getSocketError();
	if ( eno )
	{
#ifdef WIN32
		s << System::winErrorToString(eno);
#else
		s << System::errnoToString(eno);
#endif
}
	s << "\n" << SSLContext::getErrorMessage();
	return s;
}



SSLSocket::SSLSocket(SSLContext& a_ctx, const String& a_host, int a_port, int a_socket)
			: Socket(a_host, a_port, a_socket),
			  c_ctx(a_ctx),
			  c_ssl(0)
{
}


SSLSocket::SSLSocket(SSLContext& a_ctx)
				: Socket(),
				  c_ctx(a_ctx),
				  c_ssl(0)
{
}

SSLSocket::SSLSocket(SSLContext& a_ctx, const String& a_host, int a_port)
				: Socket(a_host, a_port),
				  c_ctx(a_ctx),
				  c_ssl(0)
{
}

SSLSocket::~SSLSocket()
{
	if ( c_ssl )
	{
		::SSL_free((SSL*)c_ssl);
		c_ssl = 0;
	}
}

void SSLSocket::SSLinit()
{
	if ( c_ssl )
		return;

	Socket::icheckSocket();

	c_ssl = 0;

	SSL_CTX* ctx = (SSL_CTX*)c_ctx.getSSL_CTX();
	if ( !ctx )
		throw SSLException("SSLSocket::SSLinit(): no SSL context available");

	SSL* ssl = ::SSL_new(ctx);
	if ( !ssl )
		throw SSLException(String("SSLSocket::SSLinit(): unable to create SSL socket structure\n") + SSLContext::getErrorMessage());

	BIO* sbio = ::BIO_new_socket(getSocketHandle(), BIO_NOCLOSE);
	if ( !sbio )
	{
		::SSL_free(ssl);
		throw SSLException(String("SSLSocket::SSLinit(): failed to alloc BIO\n") + SSLContext::getErrorMessage());
	}
	::SSL_set_bio(ssl, sbio, sbio);

	c_ssl = ssl;
}


static inline SSL* _checkSSL(void* a_ssl)
{
	if ( !a_ssl )
		throw SSLException("SSLSocket: no SSL connection available");
	return (SSL*)a_ssl;
}


void SSLSocket::SSLconnect()
{
	// note: after SSLinit(), c_ssl is valid
	SSLinit();

	SSL* ssl = (SSL*)c_ssl;

	int r = ::SSL_connect(ssl);
	switch(::SSL_get_error(ssl, r))
	{
		case SSL_ERROR_NONE:
			break;
		case SSL_ERROR_WANT_CONNECT:
			throw TryAgainException();
		default:
			throw SSLException(String("SSLSocket::SSLconnect(): SSL_connect failed: ", 128)
							<< soErrorString(this));
	}
}

void SSLSocket::SSLaccept()
{
	// note: after SSLinit(), c_ssl is valid
	SSLinit();

	SSL* ssl = (SSL*)c_ssl;

	int r = ::SSL_accept(ssl);
	switch(::SSL_get_error(ssl, r))
	{
		case SSL_ERROR_NONE:
			break;

#ifdef SSL_ERROR_WANT_ACCEPT
		case SSL_ERROR_WANT_ACCEPT:
			throw TryAgainException();
#endif

		default:
			throw SSLException(String("SSLSocket::SSLaccept(): SSL_accept failed: ", 128)
							<< soErrorString(this));
	}
}

int SSLSocket::SSLpending()
{
	return ::SSL_pending(_checkSSL(c_ssl));
}

bool SSLSocket::SSLwantRead()
{
	return ( ::SSL_want(_checkSSL(c_ssl)) == SSL_READING );
}
bool SSLSocket::SSLwantWrite()
{
	return ( ::SSL_want(_checkSSL(c_ssl)) == SSL_WRITING );
}

bool SSLSocket::SSLisShutdownSent()
{
	return ( ::SSL_get_shutdown(_checkSSL(c_ssl)) == SSL_SENT_SHUTDOWN );
}
bool SSLSocket::SSLisShutdownReceived()
{
	return ( ::SSL_get_shutdown(_checkSSL(c_ssl)) == SSL_RECEIVED_SHUTDOWN );
}


void SSLSocket::SSLshutdown()
{
	// SSL shutdown...

	Socket::icheckSocket();

	SSL* ssl = _checkSSL(c_ssl);

	int r = ::SSL_shutdown(ssl);
	if ( r == 0 )
	{
		/* If we called SSL_shutdown() first then we always get return value 0. In this
		   case, try again, but first send a TCP FIN to trigger the other side's
		   close_notify
		*/
		try { Socket::shutdown(ShutdownMode_WRITE); } catch(const Exception&) {}
		r = ::SSL_shutdown(ssl);
		if ( r == 0 )
			throw SSLException("SSLSocket::SSLshutdown(): shutdown failed");
	}
	if ( r < 0 )
	{
		switch(::SSL_get_error(ssl, r))
		{
			case SSL_ERROR_NONE:
				break;
			case SSL_ERROR_WANT_READ:
			case SSL_ERROR_WANT_WRITE:
				throw TryAgainException();
			default:
				throw SSLException(String("SSLSocket::SSLshutdown(): shutdown failed: ", 128)
									<< soErrorString(this));
		}
	}
}

void SSLSocket::close()
{
	SSL* ssl = (SSL*)c_ssl;
	if ( ssl )
	{
		::SSL_free(ssl);
		c_ssl = 0;
	}
	Socket::close();
}

int SSLSocket::read(void* a_buffer, int a_size)
{
	SSL* ssl = (SSL*)c_ssl;
	if ( !ssl )
	{
		// if SSL is not initialized yet, use normal read() operation
		return Socket::read(a_buffer, a_size);
		//throw SSLException("SSLSocket: no SSL connection available");
	}

	Socket::icheckSocket();

	if ( a_buffer == NULL )
		return 0;

	int r = ::SSL_read(ssl, a_buffer, a_size);
	switch(::SSL_get_error(ssl, r))
	{
		case SSL_ERROR_NONE:
			break;
		case SSL_ERROR_ZERO_RETURN:
			break;
		case SSL_ERROR_WANT_READ:
		case SSL_ERROR_WANT_WRITE:
			throw TryAgainException();
		default:
			throw SSLException(String("SSLSocket::read(): read failed: ", 128)
							<< soErrorString(this));
	}
	return r;
}

int SSLSocket::read(void* a_buffer, int a_size, int a_flags)
{
	return Socket::read(a_buffer, a_size, a_flags);
}

int SSLSocket::write(const void* a_buffer, int a_size)
{
	SSL* ssl = (SSL*)c_ssl;
	if ( !ssl )
	{
		// if SSL is not initialized yet, use normal write() operation
		return Socket::write(a_buffer, a_size);
		//throw SSLException("SSLSocket: no SSL connection available");
	}

	Socket::icheckSocket();

	if ( a_buffer == NULL )
		return 0;

	int r = ::SSL_write(ssl, a_buffer, a_size);
	switch(::SSL_get_error(ssl, r))
	{
		case SSL_ERROR_NONE:
			break;
		case SSL_ERROR_ZERO_RETURN:
			break;
		case SSL_ERROR_WANT_READ:
		case SSL_ERROR_WANT_WRITE:
			throw TryAgainException();
		default:
			throw SSLException(String("SSLSocket::write(): write failed: ", 128)
							<< soErrorString(this));
	}
	return r;
}

int SSLSocket::write(const void* a_buffer, int a_size, int a_flags)
{
	// note: write() is virtual, Socket::write() will call SSLSocket::write(char*,int)
	// for low level writes
	return Socket::write(a_buffer, a_size, a_flags);
}

int SSLSocket::write(const String& a_s, int a_flags)
{
	// note: write() is virtual, Socket::write() will call SSLSocket::write(char*,int)
	// for low level writes
	return Socket::write(a_s, a_flags);
}


bool SSLSocket::getVerifyResult()
{
	int r = ::SSL_get_verify_result(_checkSSL(c_ssl));
	return (r == X509_V_OK);
}

void SSLSocket::checkPeerCertificate(const String& a_common_name)
{
	SSL* ssl = _checkSSL(c_ssl);

	if ( ! getVerifyResult() )
		throw SSLException(String("SSLSocket::checkPeerCertificate(): no valid peer certificate: ", 256)
						<< soErrorString(this));

	X509* peer = ::SSL_get_peer_certificate(ssl);
	if ( !peer )
		throw SSLException(String("SSLSocket::checkPeerCertificate(): no peer certificate available: ", 256)
						<< soErrorString(this));

	char peername[256];
	::X509_NAME_get_text_by_NID(::X509_get_subject_name(peer), NID_commonName,
								peername, sizeof(peername));
	peername[sizeof(peername)-1] = 0;

	if ( !a_common_name.equalsIgnoreCase(peername) )
		throw SSLException("SSLSocket::checkPeerCertificate(): check of peer certificate failed");
}


void SSLSocket::useCertificateFile(const String& a_filename, SSLContext::FileType a_filetype)
{
	if ( a_filename.length() == 0 )
		throw SSLException(String("SSLSocket::useCertificateFile(): empty file name"));
	int filetype = SSLContext::checkFileType(a_filetype);
	int r = ::SSL_use_certificate_file(_checkSSL(c_ssl), a_filename.c_str(), filetype);
	if ( r <= 0 )
	{
		throw SSLException(String("SSLSocket::useCertificateFile(): can not load certificate file\n") + SSLContext::getErrorMessage());
	}
}

void SSLSocket::useCertificateASN1(const void* a_pcert, int a_size)
{
	int r = ::SSL_use_certificate_ASN1(_checkSSL(c_ssl), (unsigned char*)a_pcert, a_size);
	if ( r <= 0 )
	{
		throw SSLException(String("SSLSocket::useCertificateASN1(): can not set certificate\n") + SSLContext::getErrorMessage());
	}
}


void SSLSocket::usePrivateKeyFile(const String& a_filename, SSLContext::FileType a_filetype)
{
	if ( a_filename.length() == 0 )
		throw SSLException(String("SSLSocket::usePrivateKeyFile(): empty file name"));
	int filetype = SSLContext::checkFileType(a_filetype);
	int r = ::SSL_use_PrivateKey_file(_checkSSL(c_ssl), a_filename.c_str(), filetype);
	if ( r <= 0 )
	{
		throw SSLException(String("SSLSocket::usePrivateKeyFile(): can not load private key file\n") + SSLContext::getErrorMessage());
	}
}

void SSLSocket::usePrivateKeyASN1(const void* a_pkey, int a_size, SSLContext::KeyType a_keytype)
{
	int pk = SSLContext::checkKeyType(a_keytype);
	int r = ::SSL_use_PrivateKey_ASN1(pk, _checkSSL(c_ssl), (unsigned char*)a_pkey, a_size);
	if ( r <= 0 )
	{
		throw SSLException(String("SSLSocket::usePrivateKeyASN1(): can not set private key\n") + SSLContext::getErrorMessage());
	}
}

void SSLSocket::usePrivateKey(const SSLKey& a_key)
{
	EVP_PKEY* evp = (EVP_PKEY*)a_key.getEVP_PKEY();
	if ( !evp )
		throw SSLException("SSLSocket: can not set an empty SSLKey");

	int r = ::SSL_use_PrivateKey(_checkSSL(c_ssl), evp);
	if ( r <= 0 )
	{
		throw SSLException(String("SSLSocket::usePrivateKey(): can not set private key\n") + SSLContext::getErrorMessage());
	}
}

void SSLSocket::checkPrivateKey()
{
	int r = ::SSL_check_private_key(_checkSSL(c_ssl));
	if ( r <= 0 )
	{
		throw SSLException(String("SSLSocket::checkPrivateKey(): failed to check private key\n") + SSLContext::getErrorMessage());
	}
}

void SSLSocket::requestPublicKeyAuth(const SSLKey& a_publickey)
{
	String cookie_challenge = "PUBKEYAUTH-CHALLENGE";
	String cookie_signature = "PUBKEYAUTH-SIGNATURE";
	String cookie_authok    = "PUBKEYAUTH-OK";
	String cookie_authfail  = "PUBKEYAUTH-NO";

	// generate challenge data
	char challenge[64];
	cookie_challenge.copy(challenge);
	c_ctx.genRandom(challenge + cookie_challenge.length(), sizeof(challenge) - cookie_challenge.length());

	// send challenge
	if ( sizeof(challenge) != write(challenge, sizeof(challenge)) )
		throw SSLException(String("SSLSocket::requestPublicKeyAuth(): failed to send challenge cookie: ", 256)
							<< soErrorString(this));

	char lenbuf[64];
	int lensize = cookie_signature.length() + 2;
	if ( lensize != read(lenbuf, lensize) )
		throw SSLException(String("SSLSocket::requestPublicKeyAuth(): failed to read signature cookie: ", 256) << soErrorString(this));
	if ( !cookie_signature.equals(lenbuf, cookie_signature.length()) )
		throw SSLException("SSLSocket::requestPublicKeyAuth(): wrong signature cookie");

	unsigned int siglen = ((unsigned int)(unsigned char)lenbuf[lensize-2] << 8)
							+ (unsigned char)lenbuf[lensize-1];

	SSLSignature signature;
	signature.setSize(siglen);
	if ( signature.data() == 0 || signature.size() == 0 )
		throw SSLException("SSLSocket::requestPublicKeyAuth(): failed to setup signature object");

	if ( (int)signature.size() != read((char*)signature.data(), signature.size()) )
		throw SSLException(String("SSLSocket::requestPublicKeyAuth(): failed to read signature: ", 256) << soErrorString(this));

	try
	{
		a_publickey.verify(challenge, sizeof(challenge), signature);
	}
	catch(const Exception& ex)
	{
		if ( cookie_authfail.length() != write(cookie_authfail) )
			throw SSLException(String("SSLSocket::requestPublicKeyAuth(): failed to send FAIL cookie: ", 256) << soErrorString(this));
		throw SSLException(String("SSLSocket::requestPublicKeyAuth(): authentication failed\n") + ex.getMessage());
	}

	if ( cookie_authok.length() != write(cookie_authok) )
		throw SSLException(String("SSLSocket::requestPublicKeyAuth(): failed to send OK cookie: ", 256) << soErrorString(this));
}

void SSLSocket::confirmPublicKeyAuth(const SSLKey& a_privatekey)
{
	String cookie_challenge = "PUBKEYAUTH-CHALLENGE";
	String cookie_signature = "PUBKEYAUTH-SIGNATURE";
	String cookie_authok    = "PUBKEYAUTH-OK";
	String cookie_authfail  = "PUBKEYAUTH-NO";

	char challenge[64];

	if ( sizeof(challenge) != read(challenge, sizeof(challenge)) )
		throw SSLException(String("SSLSocket::confirmPublicKeyAuth(): failed to read challenge cookie: ", 256) << soErrorString(this));
	if ( !cookie_challenge.equals(challenge, cookie_challenge.length()) )
		throw SSLException("SSLSocket::confirmPublicKeyAuth(): invalid challenge cookie");

	SSLSignature signature;

	a_privatekey.sign(challenge, sizeof(challenge), signature);

	int siglen = signature.size();
	char lenbuf[64];
	int lensize = cookie_signature.length() + 2;
	cookie_signature.copy(lenbuf);
	lenbuf[lensize-2] = (char)(siglen >> 8);
	lenbuf[lensize-1] = (char)(siglen & 0x00ff);
	if ( lensize != write(lenbuf, lensize) )
		throw SSLException(String("SSLSocket::confirmPublicKeyAuth(): failed to write signature cookie: ", 256) << soErrorString(this));

	if ( siglen != write((char*)signature.data(), siglen) )
		throw SSLException(String("SSLSocket::confirmPublicKeyAuth(): failed to write signature object: ", 256) << soErrorString(this));

	lenbuf[0] = 0;
	if ( cookie_authok.length() != read(lenbuf, cookie_authok.length()) )
		throw SSLException(String("SSLSocket::confirmPublicKeyAuth(): failed to read answer cookie: ", 256) << soErrorString(this));

	if ( !cookie_authok.equals(lenbuf, cookie_authok.length()) )
		throw SSLException("SSLSocket::confirmPublicKeyAuth(): authentication failed");
}


} // namespace
