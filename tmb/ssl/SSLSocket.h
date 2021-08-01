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


#ifndef _TMB_SSL_SSLSOCKET_H_
#define _TMB_SSL_SSLSOCKET_H_

#include <tmb/ssl/SSLContext.h>
#include <tmb/net/Socket.h>
#include <tmb/ssl/SSLKey.h>



namespace tmb {

class SSLServerSocket;


class SSLSocket : public Socket
{
	protected:
		SSLContext&	c_ctx;
		void*		c_ssl;

	friend class SSLServerSocket;

	private: void SSLinit();


	protected: explicit SSLSocket(SSLContext& a_ctx, const String& a_host, int a_port, int a_socket);

	public: explicit SSLSocket(SSLContext& a_ctx);

	public: explicit SSLSocket(SSLContext& a_ctx, const String& a_host, int a_port);

	private: SSLSocket(const SSLSocket&);
	private: void operator=(const SSLSocket&);

	public: virtual ~SSLSocket();


	public: void SSLconnect();
	public: void SSLshutdown();
	public: void SSLaccept();
	public: int  SSLpending();
	public: bool SSLwantRead();
	public: bool SSLwantWrite();
	public: bool SSLisShutdownSent();
	public: bool SSLisShutdownReceived();

			///
	public: virtual void close();

			///
	public: virtual int read(void* a_buffer, int a_size);
	public: virtual int read(void* a_buffer, int a_size, int a_flags);

			///
	public: virtual int write(const void* a_buffer, int a_size);
			///
	public: virtual int write(const void* a_buffer, int a_size, int a_flags);
			///
	public: virtual int write(const String& a_s, int a_flags = Socket::WriteFlag_NONE);


	public: bool getVerifyResult();
	public: void checkPeerCertificate(const String& a_common_name);

	public: void useCertificateFile(const String& a_filename, SSLContext::FileType a_filetype = SSLContext::FileType_PEM);
	public: void useCertificateASN1(const void* a_pcert, int a_size);

	public: void usePrivateKeyFile(const String& a_filename, SSLContext::FileType a_filetype = SSLContext::FileType_PEM);
	public: void usePrivateKeyASN1(const void* a_pkey, int a_size, SSLContext::KeyType a_keytype = SSLContext::KeyType_RSA);
	public: void usePrivateKey(const SSLKey& a_key);

	public: void checkPrivateKey();

	public: void requestPublicKeyAuth(const SSLKey& a_publickey);
	public: void confirmPublicKeyAuth(const SSLKey& a_privatekey);
};

} // namespace

#endif
