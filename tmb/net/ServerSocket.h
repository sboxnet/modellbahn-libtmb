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



#ifndef _TMB_NET_SERVERSOCKET_H_
#define _TMB_NET_SERVERSOCKET_H_


#include <tmb/lang/String.h>

#include <tmb/io/IOException.h>
#include <tmb/net/SocketException.h>
#include <tmb/net/BindException.h>
#include <tmb/net/ConnectException.h>
#include <tmb/net/UnknownHostException.h>

#include <tmb/net/Socket.h>

namespace tmb {

class ServerSocket : public SocketBase
{
	public: explicit ServerSocket();

			/// does a bind() and listen() too !
	public: explicit ServerSocket(int a_port, int a_backlog = 10, bool a_reuseaddr = false);

	private: ServerSocket(const ServerSocket&);
	private: void operator=(const ServerSocket&);

	public: virtual ~ServerSocket();

	private: void icreateSocket();
	private: void idestroySocket();

	public: int getLocalPort() const { return c_port; }

	public: void bind(int a_port);

	public: void listen(int a_backlog = 10);

	public: virtual void close();

	public: virtual Socket* accept(Socket** a_psocket = 0) const;

	protected: virtual Socket* newSocket(const String& a_peer, int a_remoteport, int a_handle) const;
};


} // namespace


#endif
