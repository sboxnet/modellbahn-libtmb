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



#include <tmb/net/ServerSocket.h>

#include <sys/types.h>

#ifdef WIN32

# include <io.h>

// define _MSWSOCK_, kollidiert sonst mit _WIN32_WINNT !!!
# define _MSWSOCK_
# include <winsock2.h>

#else

# include <errno.h>
# include <string.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <netdb.h>
# include <arpa/inet.h>

#endif


namespace tmb {


ServerSocket::ServerSocket()
				: SocketBase(0)
{
	icreateSocket();
}

ServerSocket::ServerSocket(int a_port, int a_backlog, bool a_reuseaddr)
				: SocketBase(a_port)
{
	icreateSocket();

	try
	{
		if ( a_reuseaddr )
		{
			setOption(Option_REUSEADDR, true);
		}

		bind(a_port);

		listen(a_backlog);
	}
	catch(const IOException& )
	{
		idestroySocket();
		throw;
	}
}

ServerSocket::~ServerSocket()
{
	idestroySocket();
}

void ServerSocket::icreateSocket()
{
#ifdef WIN32

	WSADATA WSAData;
	if ( ::WSAStartup (2, &WSAData) != 0 )
		throw SocketException("WSAStartup failed");
#endif

	c_socket = ::socket(AF_INET, SOCK_STREAM, 0);
#ifdef WIN32
	if ( c_socket == INVALID_SOCKET )
	{
		int m_err = WSAGetLastError();
		WSACleanup();
		throw SocketException(String("can not create socket: ") << System::winErrorToString(m_err), m_err);
	}
#else
	if ( c_socket == -1 )
	{
		int eno = errno;
		throw SocketException(String("can not create socket: ") << ::strerror(eno), eno);
	}
#endif
}

void ServerSocket::idestroySocket()
{
	if ( c_socket >= 0 )
	{
#ifdef WIN32
		::closesocket(c_socket);
		WSACleanup();
#else
		::close(c_socket);
#endif
	}
	c_socket = -1;
}

void ServerSocket::bind(int a_port)
{
	icheckSocket();

	struct sockaddr_in m_sin;
	::memset(&m_sin, 0, sizeof(m_sin));
	m_sin.sin_addr.s_addr = INADDR_ANY;
	m_sin.sin_port = htons((unsigned short)a_port);
	m_sin.sin_family = AF_INET;

	int m_err = ::bind(c_socket, (struct sockaddr*)&m_sin, sizeof(m_sin));
#ifdef WIN32
	if ( m_err == SOCKET_ERROR )
	{
		m_err = WSAGetLastError();
		throw BindException(String("port: ",32) << a_port << " : " << System::winErrorToString(m_err), m_err);
	}
#else
	if ( m_err != 0 )
	{
		int eno = errno;
		throw BindException(String("port: ",32) << a_port << " : " << ::strerror(eno), eno);
	}
#endif

	c_port = a_port;
}

void ServerSocket::listen(int a_backlog)
{
	icheckSocket();

	int m_err = ::listen(c_socket, a_backlog);
#ifdef WIN32
	if ( m_err == SOCKET_ERROR )
	{
		m_err = WSAGetLastError();
		throw SocketException(String("failed to listen on port: ") << c_port << " : " << System::winErrorToString(m_err), m_err);
	}
#else
	if ( m_err != 0 )
	{
		int eno = errno;
		throw SocketException(String("failed to listen on port: ") <<  c_port << " : " << ::strerror(eno), eno);
	}
#endif
}



void ServerSocket::close()
{
	if ( c_socket >= 0 )
	{
#ifdef WIN32
		if ( ::closesocket(c_socket) == SOCKET_ERROR )
		{
			int m_err = WSAGetLastError();
			throw IOException(String("ServerSocket::close(): ") << System::winErrorToString(m_err), m_err);
		}
#else
		while ( ::close(c_socket) != 0 )
		{
			int eno = errno;
			if ( eno == EINTR )
			{
				if ( c_flags & Flag_IGNORE_EINTR ) continue;
				throw InterruptedException("ServerSocket::close(): interrupted");
			}
			throw IOException(String("ServerSocket::close(): ") << ::strerror(eno), eno);
		}
#endif
		c_socket = -1;
	}
}

Socket* ServerSocket::accept(Socket** a_psocket) const
{
	if ( a_psocket )
		*a_psocket = 0;

	icheckSocket();

	struct sockaddr_in m_epaddr;
#ifdef WIN32
	int m_epaddrlen = sizeof(m_epaddr);
#else
	unsigned int m_epaddrlen = sizeof(m_epaddr);
#endif

#ifdef WIN32
	int m_newsocket = ::accept(c_socket, (struct sockaddr*)&m_epaddr, &m_epaddrlen);
	if ( m_newsocket == INVALID_SOCKET )
	{
		int m_err = WSAGetLastError();
		throw SocketException(String("ServerSocket::accept(): ") << System::winErrorToString(m_err), m_err);
	}
#else
	int m_newsocket;
	while ( (m_newsocket = ::accept(c_socket, (struct sockaddr*)&m_epaddr, &m_epaddrlen)) < 0 )
	{
		int eno = errno;
		if ( eno == EINTR )
		{
			if ( c_flags & Flag_IGNORE_EINTR ) continue;
			throw InterruptedException("ServerSocket::accept(): interrupted");
		}
		throw SocketException(String("ServerSocket::accept(): ") << ::strerror(eno), eno);
	}
#endif

	Socket* s = newSocket(String(inet_ntoa(m_epaddr.sin_addr)), (int)ntohs(m_epaddr.sin_port) , m_newsocket);

	if ( a_psocket )
		*a_psocket = s;

	return s;
}

Socket* ServerSocket::newSocket(const String& a_peer, int a_remoteport, int a_handle) const
{
	return new Socket(a_peer, a_remoteport , a_handle);
}

} // namespace
