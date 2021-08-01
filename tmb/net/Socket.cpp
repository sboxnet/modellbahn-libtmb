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

#include <tmb/net/Socket.h>

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
# include <fcntl.h>
# include <netinet/tcp.h>

#endif


namespace tmb {


inline int SocketBase::transformOption(SocketBase::Option a_option)
{
	int optname;
	switch ( a_option )
	{
#define __optcase(_o) case Option_##_o: optname = SO_##_o; break;
		__optcase(DEBUG)
		__optcase(REUSEADDR)
		__optcase(KEEPALIVE)
		__optcase(DONTROUTE)
		__optcase(BROADCAST)
		__optcase(OOBINLINE)
		__optcase(SNDBUF)
		__optcase(RCVBUF)
		__optcase(TYPE)
		__optcase(ERROR)
		default: throw IOException("unknown socket option, not a member of Socket::Option");
#undef __optcase
	}
	return optname;
}

void SocketBase::setOption(int a_level, int a_option, int a_value)
{
	icheckSocket();

#ifdef WIN32
	if ( ::setsockopt(c_socket, a_level, a_option, (const char*)&a_value, sizeof(a_value)) == SOCKET_ERROR )
	{
		int m_err = WSAGetLastError();
		throw IOException(String("Socket::setsockopt(): ") << System::winErrorToString(m_err), m_err);
	}
#else
	if ( ::setsockopt(c_socket, a_level, a_option, (void*)&a_value, sizeof(a_value)) != 0 )
	{
		int eno = errno;
		throw IOException(String("Socket::setsockopt(): ") << ::strerror(eno), eno);
	}
#endif
}

void SocketBase::setOption(SocketBase::Option a_option, int a_value)
{
	setOption(SOL_SOCKET, transformOption(a_option), a_value);
}

void SocketBase::setLinger(bool a_on, unsigned short a_timeout)
{
	icheckSocket();

	struct linger m_linger;
	m_linger.l_onoff = (unsigned short)a_on;
	m_linger.l_linger = a_timeout;

#ifdef WIN32
	if ( ::setsockopt(c_socket, SOL_SOCKET, SO_LINGER, (const char*)&m_linger, sizeof(m_linger)) == SOCKET_ERROR )
	{
		int m_err = WSAGetLastError();
		throw IOException(String("Socket::setsockopt(): ") << System::winErrorToString(m_err), m_err);
	}
#else
	if ( ::setsockopt(c_socket, SOL_SOCKET, SO_LINGER, (void*)&m_linger, sizeof(m_linger)) != 0 )
	{
		int eno = errno;
		throw IOException(String("Socket::setsockopt(): ") << ::strerror(eno), eno);
	}
#endif
}

void SocketBase::getOption(int a_level, int a_option, void* a_value, int* a_vallen)
{
	icheckSocket();

#ifdef WIN32
	if ( ::getsockopt(c_socket, a_level, a_option, (char*)a_value, a_vallen) == SOCKET_ERROR )
	{
		int m_err = WSAGetLastError();
		throw IOException(String("Socket::getsockopt(): ") << System::winErrorToString(m_err), m_err);
	}
#else
	if ( ::getsockopt(c_socket, a_level, a_option, (void*)a_value, (socklen_t*)a_vallen) != 0 )
	{
		int eno = errno;
		throw IOException(String("Socket::setsockopt(): ") << ::strerror(eno), eno);
	}
#endif
}

int SocketBase::getOption(SocketBase::Option a_option)
{
	int optval = 0;
	int optsize = sizeof(optval);
	getOption(SOL_SOCKET, transformOption(a_option), &optval, &optsize);
	return optval;
}


void SocketBase::getLinger(bool& a_on, unsigned short& a_timeout)
{
	icheckSocket();

	struct linger m_linger;
	m_linger.l_onoff = 0;
	m_linger.l_linger = 0;


#ifdef WIN32
	int m_size = sizeof(m_linger);
	if ( ::getsockopt(c_socket, SOL_SOCKET, SO_LINGER, (char*)&m_linger, &m_size) == SOCKET_ERROR )
	{
		int m_err = WSAGetLastError();
		throw IOException(String("Socket::setsockopt(): ") << System::winErrorToString(m_err), m_err);
	}
#else
	socklen_t m_size = sizeof(m_linger);
	if ( ::getsockopt(c_socket, SOL_SOCKET, SO_LINGER, (void*)&m_linger, &m_size) != 0 )
	{
		int eno = errno;
		throw IOException(String("Socket::setsockopt(): ") << ::strerror(eno), eno);
	}
#endif

	a_on = (m_linger.l_onoff != 0);
	a_timeout = m_linger.l_linger;
}

int SocketBase::getSocketError()
{
	return getOption(Option_ERROR);
}

void SocketBase::setNonBlockingMode(bool a_on)
{
	icheckSocket();

#ifdef WIN32
	u_long nodelay = a_on;
	if ( ::ioctlsocket(c_socket, FIONBIO, &nodelay) == SOCKET_ERROR )
	{
		int m_err = WSAGetLastError();
		throw IOException(String("Socket::setNonBlockingMode(): ") << System::winErrorToString(m_err), m_err);
	}
#else
	if ( ::fcntl(c_socket, F_SETFD, (int)(a_on ? O_NONBLOCK : 0)) == -1 )
	{
		int eno = errno;
		throw IOException(String("Socket:setNonBlockingMode(): ") << ::strerror(eno), eno);
	}
#endif
}







Socket::Socket(const String& a_remotehost, int a_remoteport, int a_socket)
			: SocketBase(a_remoteport, a_socket),
			  c_remotehost(a_remotehost)
{
#ifdef WIN32
	if ( c_socket >= 0 )
	{
		WSADATA WSAData;
		if ( ::WSAStartup (2, &WSAData) != 0 )
			throw SocketException("WSAStartup failed");
	}
#endif
}

Socket::Socket(const String& a_host, int a_port)
			: SocketBase(a_port),
			  c_remotehost(a_host)
{
	connect(a_host, a_port);
}

void Socket::connect(const String& a_host, int a_port)
{
#ifdef WIN32

	// Windows implementation
	// Note: gethostbyname() and gethostbyaddr() is already thread safe under Windows...
	// Unluckily getaddrinfo() is only available on Windows XP or .NET
	WSADATA WSAData;
	if ( ::WSAStartup (2, &WSAData) != 0 )
		throw SocketException("WSAStartup failed");

	struct hostent *m_hostent;
	unsigned long m_inetaddr = inet_addr(a_host.c_str());
	if ( m_inetaddr == INADDR_NONE )
	{
		m_hostent = ::gethostbyname(a_host.c_str());
	}
	else
	{
		m_hostent = ::gethostbyaddr((const char*)&m_inetaddr, sizeof(m_inetaddr), AF_INET);
	}
	if ( m_hostent == NULL )
	{
		WSACleanup();
		throw UnknownHostException(String("host: ",128) << a_host);
	}

	struct sockaddr_in m_sin;
	::memset(&m_sin, 0, sizeof(m_sin));
	::memcpy(&m_sin.sin_addr.s_addr, m_hostent->h_addr_list[0], sizeof(m_sin.sin_addr.s_addr));
	m_sin.sin_port = htons((unsigned short)a_port);
	m_sin.sin_family = AF_INET;

	c_socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if ( c_socket == INVALID_SOCKET )
	{
		c_socket = -1;
		int m_err = WSAGetLastError();
		WSACleanup();
		throw SocketException(String("can not create socket: ") << System::winErrorToString(m_err), m_err);
	}

	int m_err = ::connect(c_socket, (struct sockaddr*)&m_sin, sizeof(m_sin));
	if ( m_err == SOCKET_ERROR )
	{
		m_err = WSAGetLastError();
		::closesocket(c_socket);
		WSACleanup();
		c_socket = -1;
		throw ConnectException(String("host: ",128) << a_host << ":" << a_port << " : "
								<< System::winErrorToString(m_err), m_err);
	}


#else

	// UNIX implementation

	// NOTE: do not use gethostbyname() or gethostbyaddr() here, since we need a thread
	// safe constructor here. Use the newer function getaddrinfo().
	String strport;
	strport << a_port; // convert a_port into a string;

	struct addrinfo saddrinfo;
	::memset(&saddrinfo, 0, sizeof(saddrinfo));
	saddrinfo.ai_flags = 0;
	saddrinfo.ai_family = PF_INET;
	saddrinfo.ai_socktype = SOCK_STREAM;
	saddrinfo.ai_protocol = IPPROTO_TCP;

	struct addrinfo* paddrinfo = 0;
	int r = ::getaddrinfo(a_host.c_str(), strport.c_str(), &saddrinfo, &paddrinfo);
	if ( r != 0 )
	{
		throw UnknownHostException(String("host: ",128) << a_host
			<< " : " << ::gai_strerror(r), r);
	}

	c_socket = ::socket(paddrinfo->ai_family, paddrinfo->ai_socktype, paddrinfo->ai_protocol);
	if ( c_socket < 0 )
	{
		c_socket = -1;
		int eno = errno;
		::freeaddrinfo(paddrinfo);
		throw SocketException(String("can not create socket: ") << ::strerror(eno), eno);
	}

	int m_err = ::connect(c_socket, paddrinfo->ai_addr, paddrinfo->ai_addrlen);
	if ( m_err != 0 )
	{
		int eno = errno;
		::close(c_socket);
		c_socket = -1;
		::freeaddrinfo(paddrinfo);
		throw ConnectException(String("host: ",128) << a_host << ":" << a_port << " : " << ::strerror(eno), eno);
	}

	::freeaddrinfo(paddrinfo);
#endif
}

Socket::~Socket()
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
}


void Socket::shutdown(Socket::ShutdownMode a_mode)
{
	icheckSocket();

#ifdef WIN32
	if ( ::shutdown(c_socket, a_mode) == SOCKET_ERROR )
	{
		int m_err = WSAGetLastError();
		throw IOException(String("Socket::shutdown(): ") << System::winErrorToString(m_err), m_err);
	}
#else
	if ( ::shutdown(c_socket, a_mode) != 0 )
	{
		int eno = errno;
		throw IOException(String("Socket::shutdown(): ") << ::strerror(eno), eno);
	}
#endif
}


void Socket::close()
{
	if ( c_socket >= 0 )
	{
#ifdef WIN32
		if ( ::closesocket(c_socket) == SOCKET_ERROR )
		{
			int m_err = WSAGetLastError();
			throw IOException(String("Socket::close(): ") << System::winErrorToString(m_err), m_err);
		}
#else
		while ( ::close(c_socket) != 0 )
		{
			int eno = errno;
			if ( eno == EINTR )
			{
				if ( c_flags & Flag_IGNORE_EINTR ) continue;
				throw InterruptedException("Socket::close(): interrupted");
			}
			throw IOException(String("Socket::close(): ") << ::strerror(eno), eno);
		}
#endif
		c_socket = -1;
	}
}

int Socket::read(void* a_buffer, int a_size)
{
	icheckSocket();

	if ( a_buffer == NULL )
		return 0;

#ifdef WIN32
	int m_read = ::recv(c_socket, (char*)a_buffer, a_size, 0);
	if ( m_read == SOCKET_ERROR )
	{
		int m_err = WSAGetLastError();
		if ( m_err == WSAEWOULDBLOCK )
			throw TryAgainException();
		throw IOException(String("Socket::read(): ") << System::winErrorToString(m_err), m_err);
	}
#else
	int m_read;
	while ( (m_read = ::recv(c_socket, (char*)a_buffer, a_size, 0)) < 0 )
	{
		int eno = errno;
		if (eno == EWOULDBLOCK || eno == EAGAIN)
			throw TryAgainException();
		if (eno == EINTR)
		{
			if ( c_flags & Flag_IGNORE_EINTR ) continue;
			throw InterruptedException("Socket::read(): interrupted");
		}
		throw IOException(String("Socket::read(): ") << ::strerror(eno), eno);
	}
#endif
	return m_read;
}


int Socket::write(const void* a_buffer, int a_size)
{
	icheckSocket();

	if ( a_buffer == NULL )
		return 0;

#ifdef WIN32
	int m_written = ::send(c_socket, (const char*)a_buffer, a_size, 0);
	if ( m_written == SOCKET_ERROR )
	{
		int m_err = WSAGetLastError();
		if ( m_err == WSAEWOULDBLOCK )
			throw TryAgainException();
		throw IOException(String("Socket::write(): ") << System::winErrorToString(m_err), m_err);
	}
#else
	int m_written;
	while ( (m_written = ::send(c_socket, (const char*)a_buffer, a_size, 0)) < 0 )
	{
		int eno = errno;
		if (eno == EWOULDBLOCK || eno == EAGAIN)
			throw TryAgainException();
		if (eno == EINTR)
		{
			if ( c_flags & Flag_IGNORE_EINTR ) continue;
			throw InterruptedException("Socket::write(): interrupted");
		}
		throw IOException(String("Socket::write(): ") << ::strerror(eno), eno);
	}
#endif
	return m_written;
}



} // namespace
