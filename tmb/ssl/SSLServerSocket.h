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


#ifndef _TMB_SSL_SSLSERVERSOCKET_H_
#define _TMB_SSL_SSLSERVERSOCKET_H_


#include <tmb/net/ServerSocket.h>
#include <tmb/ssl/SSLContext.h>
#include <tmb/ssl/SSLSocket.h>


namespace tmb {


class SSLServerSocket : public ServerSocket
{
	protected:
		SSLContext&	c_ctx;

	public: explicit SSLServerSocket(SSLContext& a_ctx);

	public: explicit SSLServerSocket(SSLContext& a_ctx, int a_port,
									int a_backlog = 10, bool a_reuseaddr = false);

	private: SSLServerSocket(const SSLServerSocket&);
	private: void operator=(const SSLServerSocket&);

	public: virtual ~SSLServerSocket() {}

	public: virtual SSLSocket* accept(SSLSocket** a_psocket = 0) const;

	protected: virtual Socket* newSocket(const String& a_peer, int a_remoteport, int a_handle) const;
};

} // namespace

#endif
