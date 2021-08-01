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


#include <tmb/ssl/SSLKey.h>


#include <openssl/ssl.h>
#include <openssl/ssl3.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/dsa.h>

namespace tmb {

inline void SSLKey::ifreekey() throw()
{
	if ( c_key )
	{
		::EVP_PKEY_free((EVP_PKEY*)c_key);
	}
	c_key = 0;
	c_key_type = SSLContext::KeyType_UNKNOWN;
}

void SSLKey::icopykey(const SSLKey& a_key) throw()
{
	c_key_type	= SSLContext::KeyType_UNKNOWN;
	c_key		= 0;
	c_passwd	= a_key.c_passwd;

	SSLContext::KeyType keytype = a_key.getKeyType();
	if ( keytype == SSLContext::KeyType_RSA || keytype == SSLContext::KeyType_DSA )
	{
		EVP_PKEY* evp = (EVP_PKEY*)a_key.getEVP_PKEY();
		if ( evp )
		{
			EVP_PKEY* newevp = ::EVP_PKEY_new();
			if ( newevp )
			{
				if ( ::EVP_PKEY_assign(newevp, ::EVP_PKEY_type(evp->type),
										(keytype == SSLContext::KeyType_RSA
											? (char*)::EVP_PKEY_get1_RSA(evp)
											: (char*)::EVP_PKEY_get1_DSA(evp)) ) > 0 )
				{
					c_key_type = keytype;
					c_key = newevp;
				}
				else
					::EVP_PKEY_free(newevp);
			}
		}
	}
}


SSLKey::SSLKey()
			: c_key(0),
			  c_passwd(0),
			  c_key_type(SSLContext::KeyType_UNKNOWN)
{
}

SSLKey::SSLKey(const SSLKey& a_key)
{
	icopykey(a_key);
}

void SSLKey::operator= (const SSLKey& a_key)
{
	if ( this == &a_key )
		return;

	ifreekey();

	icopykey(a_key);
}

SSLKey::~SSLKey()
{
	ifreekey();
}

void SSLKey::loadPrivateKeyFile(const String& a_filename)
{
	if ( a_filename.length() == 0 )
		throw SSLException(String("SSLKey::loadPrivateKeyFile(): empty file name"));

	BIO* bio = ::BIO_new(::BIO_s_file());
	if ( !bio )
		throw SSLException(String("SSLKey::loadPrivateKeyFile(): failed to create a BIO object\n") + SSLContext::getErrorMessage());

	if ( ::BIO_read_filename(bio, a_filename.c_str()) <= 0 )
	{
		String errmsg = SSLContext::getErrorMessage();
		::BIO_free(bio);
		throw SSLException(String("SSLKey::loadPrivateKeyFile(): failed to open file: ") << a_filename << "\n" << errmsg);
	}

	SSLContext::KeyType keytype = SSLContext::KeyType_UNKNOWN;

	EVP_PKEY* evpkey = ::PEM_read_bio_PrivateKey(bio, NULL, (c_passwd ? SSLPassword::_pw_callback : NULL), c_passwd);

	::BIO_free(bio);

	if ( !evpkey )
		throw SSLException(String("SSLKey::loadPrivateKeyFile(): unable to load private key\n") + SSLContext::getErrorMessage());

	int evpkeytype = ::EVP_PKEY_type(evpkey->type);
	if ( evpkeytype == EVP_PKEY_RSA )
	{
		keytype = SSLContext::KeyType_RSA;
	}
	else if ( evpkeytype == EVP_PKEY_DSA )
	{
		keytype = SSLContext::KeyType_DSA;
	}
	else
	{
		::EVP_PKEY_free(evpkey);
		throw SSLException(String("SSLKey::loadPrivateKeyFile(): not a RSA or DSA key\n") + SSLContext::getErrorMessage());
	}

	ifreekey();

	c_key = evpkey;
	c_key_type = keytype;
}

void SSLKey::loadPrivateKeyASN1(const void* a_pkey, int a_size, SSLContext::KeyType a_kt)
{
	const unsigned char* pkey = (const unsigned char*)a_pkey;

	EVP_PKEY* key = 0;

	if ( a_kt == SSLContext::KeyType_RSA )
	{
		RSA* rsa = ::d2i_RSAPrivateKey(NULL, &pkey, a_size);
		if ( rsa )
		{
			key = ::EVP_PKEY_new();
			if ( key )
			{
				if ( ::EVP_PKEY_assign_RSA(key, rsa) <= 0 )
				{
					::EVP_PKEY_free(key);
					key = 0;
				}
			}
			if (!key)
				::RSA_free(rsa);
		}
	}
	else if ( a_kt == SSLContext::KeyType_DSA )
	{
		DSA* dsa = ::d2i_DSAPrivateKey(NULL, &pkey, a_size);
		if ( dsa )
		{
			key = ::EVP_PKEY_new();
			if ( key )
			{
				if ( ::EVP_PKEY_assign_DSA(key, dsa) <= 0 )
				{
					::EVP_PKEY_free(key);
					key = 0;
				}
			}
			if (!key)
				::DSA_free(dsa);
		}
	}
	else
	{
		throw SSLException(String("SSLKey::loadPrivateKeyASN1(): unknown key format"));
	}
	if ( !key )
		throw SSLException(String("SSLKey::loadPrivateKeyASN1(): unable to load private key\n") + SSLContext::getErrorMessage());

	ifreekey();

	c_key = key;
	c_key_type = a_kt;
}

void SSLKey::loadPublicKeyFile(const String& a_filename)
{
	if ( a_filename.length() == 0 )
		throw SSLException(String("SSLKey::loadPrivateKeyFile(): empty file name"));

	BIO* bio = ::BIO_new(::BIO_s_file());
	if ( !bio )
		throw SSLException(String("SSLKey::loadPublicKeyFile(): failed to create a BIO object\n") + SSLContext::getErrorMessage());

	if ( ::BIO_read_filename(bio, a_filename.c_str()) <= 0 )
	{
		String errmsg = SSLContext::getErrorMessage();
		::BIO_free(bio);
		throw SSLException(String("SSLKey::loadPublicKeyFile(): failed to open file: ") << a_filename << "\n" << errmsg);
	}

	SSLContext::KeyType keytype = SSLContext::KeyType_UNKNOWN;

	EVP_PKEY* evpkey = ::PEM_read_bio_PUBKEY(bio, NULL, (c_passwd ? SSLPassword::_pw_callback : NULL), c_passwd);

	::BIO_free(bio);

	if ( !evpkey )
		throw SSLException(String("SSLKey::loadPublicKeyFile(): unable to load public key\n") + SSLContext::getErrorMessage());

	int evpkeytype = ::EVP_PKEY_type(evpkey->type);
	if ( evpkeytype == EVP_PKEY_RSA )
	{
		keytype = SSLContext::KeyType_RSA;
	}
	else if ( evpkeytype == EVP_PKEY_DSA )
	{
		keytype = SSLContext::KeyType_DSA;
	}
	else
	{
		::EVP_PKEY_free(evpkey);
		throw SSLException(String("SSLKey::loadPrivateKeyFile(): not a RSA or DSA key\n") + SSLContext::getErrorMessage());
	}

	ifreekey();

	c_key = evpkey;
	c_key_type = keytype;
}

void SSLKey::loadPublicKeyASN1(const void* a_pkey, int a_size, SSLContext::KeyType a_kt)
{
	const unsigned char* pkey = (const unsigned char*)a_pkey;

	EVP_PKEY* key = 0;

	if ( a_kt == SSLContext::KeyType_RSA )
	{
		RSA* rsa = ::d2i_RSAPublicKey(NULL, &pkey, a_size);
		if ( rsa )
		{
			key = ::EVP_PKEY_new();
			if ( key )
			{
				if ( ::EVP_PKEY_assign_RSA(key, rsa) <= 0 )
				{
					::EVP_PKEY_free(key);
					key = 0;
				}
			}
			if (!key)
				::RSA_free(rsa);
		}
	}
	else if ( a_kt == SSLContext::KeyType_DSA )
	{
		DSA* dsa = ::d2i_DSAPublicKey(NULL, &pkey, a_size);
		if ( dsa )
		{
			key = ::EVP_PKEY_new();
			if ( key )
			{
				if ( ::EVP_PKEY_assign_DSA(key, dsa) <= 0 )
				{
					::EVP_PKEY_free(key);
					key = 0;
				}
			}
			if (!key)
				::DSA_free(dsa);
		}
	}
	else
	{
		throw SSLException(String("SSLKey::loadPublicKeyASN1(): unknown key format"));
	}
	if ( !key )
		throw SSLException(String("SSLKey::loadPublicKeyASN1(): unable to load public key\n") + SSLContext::getErrorMessage());

	ifreekey();

	c_key = key;
	c_key_type = a_kt;
}

void SSLKey::setPassword(SSLPassword* a_passwd)
{
	c_passwd = a_passwd;
}


void SSLKey::sign(const void* a_data, unsigned int a_size, SSLSignature& a_sign) const
{
	if ( !c_key || (c_key_type != SSLContext::KeyType_RSA && c_key_type != SSLContext::KeyType_DSA) )
		throw SSLException("SSLKey::sign(): no key set");

	if ( a_data == 0 || a_size == 0 )
		throw SSLException("SSLKey::sign(): no data given");

	// make a SHA-1 hash of the data
	unsigned char digest[SHA_DIGEST_LENGTH];
	::SHA1((unsigned char*)a_data, a_size, digest);

	int r = 0;

	if ( c_key_type == SSLContext::KeyType_RSA )
	{
		RSA* rsa = ::EVP_PKEY_get1_RSA((EVP_PKEY*)c_key);
		if ( rsa )
		{
			a_sign.setSize(::RSA_size(rsa));

			unsigned char* sigbuf = (unsigned char*)a_sign.data();
			unsigned int   siglen = a_sign.size();

			r = ::RSA_sign(NID_sha1, digest, sizeof(digest), sigbuf, &siglen, rsa);
			::RSA_free(rsa);
		}
	}
	else if ( c_key_type == SSLContext::KeyType_DSA )
	{
		DSA* dsa = ::EVP_PKEY_get1_DSA((EVP_PKEY*)c_key);
		if ( dsa )
		{
			a_sign.setSize(::DSA_size(dsa));

			unsigned char* sigbuf = (unsigned char*)a_sign.data();
			unsigned int   siglen = a_sign.size();

			r = ::DSA_sign(NID_sha1, digest, sizeof(digest), sigbuf, &siglen, dsa);
			::DSA_free(dsa);
		}
	}
	if ( r <= 0 )
	{
		throw SSLException(String("SSLKey::sign(): signing failed\n") + SSLContext::getErrorMessage());
	}
}

void SSLKey::verify(const void* a_data, unsigned int a_size, const SSLSignature& a_sign) const
{
	if ( !c_key || (c_key_type != SSLContext::KeyType_RSA && c_key_type != SSLContext::KeyType_DSA) )
		throw SSLException("SSLKey::sign(): no key set");

	if ( a_data == 0 || a_size == 0 )
		throw SSLException("SSLKey::sign(): no data given");

	unsigned char* sigbuf = (unsigned char*)a_sign.data();
	unsigned int   siglen = a_sign.size();

	if ( sigbuf == 0 || siglen == 0 )
		throw SSLException("SSLKey::verify(): empty signature");

	// make a SHA-1 hash of the data
	unsigned char digest[SHA_DIGEST_LENGTH];
	::SHA1((unsigned char*)a_data, a_size, digest);

	int r = 0;

	if ( c_key_type == SSLContext::KeyType_RSA )
	{
		RSA* rsa = ::EVP_PKEY_get1_RSA((EVP_PKEY*)c_key);
		if ( rsa )
		{
			r = ::RSA_verify(NID_sha1, digest, sizeof(digest), sigbuf, siglen, rsa);
			::RSA_free(rsa);
		}
	}
	else if ( c_key_type == SSLContext::KeyType_DSA )
	{
		DSA* dsa = ::EVP_PKEY_get1_DSA((EVP_PKEY*)c_key);
		if ( dsa )
		{
			r = ::DSA_verify(NID_sha1, digest, sizeof(digest), sigbuf, siglen, dsa);
			::DSA_free(dsa);
		}
	}
	if ( r <= 0 )
	{
		throw SSLException(String("SSLKey::verify(): verify failed\n") + SSLContext::getErrorMessage());
	}
}

} // namespace
