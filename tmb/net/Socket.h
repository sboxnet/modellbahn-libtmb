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



#ifndef _TMB_NET_SOCKET_H_
#define _TMB_NET_SOCKET_H_

#ifdef WIN32
# define _MSWSOCK_
# include <winsock2.h>
#else
# include <netinet/in.h>
#endif
#include <errno.h>

#include <tmb/lang/String.h>

#include <tmb/io/IOException.h>
#include <tmb/io/InputStream.h>
#include <tmb/io/OutputStream.h>
#include <tmb/net/SocketException.h>
#include <tmb/net/BindException.h>
#include <tmb/net/ConnectException.h>
#include <tmb/net/UnknownHostException.h>

namespace tmb {

// forward declaration
class ServerSocket;

class SocketBase
{
	public: enum Flag {
				Flag_IGNORE_EINTR = 0x0001,
			};

	protected:
		int		c_socket;
		int		c_port;
		int		c_flags;

	public: enum Option { Option_DEBUG,
						  Option_REUSEADDR,
						  Option_KEEPALIVE,
						  Option_DONTROUTE,
						  Option_BROADCAST,
						  Option_OOBINLINE,
						  Option_SNDBUF,
						  Option_RCVBUF,
						  Option_DGRAM_ERRIND,
						  Option_TYPE,
						  Option_ERROR,
			};

	public: static uint32 ntoh(uint32 a_val) { return ntohl(a_val); }
	public: static uint16 ntoh(uint16 a_val) { return ntohs(a_val); }
	public: static uint32 hton(uint32 a_val) { return htonl(a_val); }
	public: static uint16 hton(uint16 a_val) { return htons(a_val); }

	protected: int transformOption(SocketBase::Option a_option);

	protected: explicit SocketBase(int a_port)
							: c_socket(-1), c_port(a_port), c_flags(0)
				{}
	protected: explicit SocketBase(int a_port, int a_socket)
							: c_socket(a_socket), c_port(a_port), c_flags(0)
				{}

	private: SocketBase(const SocketBase&);
	private: void operator=(const SocketBase&);

	public: virtual ~SocketBase()
			{
			}

	public: int getSocketHandle() const
			{
				return c_socket;
			}

	public: bool isConnected() const throw()
			{
				return (c_socket >= 0);
			}

	public: void ignoreInterrupts(bool a_on)
			{
				(a_on ? (c_flags |= Flag_IGNORE_EINTR)
						:(c_flags &= ~Flag_IGNORE_EINTR));
			}

	protected: void icheckSocket() const
			{
				if ( c_socket < 0 )
					throw IOException("socket is closed", EINVAL);
			}


	public: void setOption(int a_level, int a_option, int a_value);
	public: void setOption(SocketBase::Option a_option, int a_value);
	public: void setLinger(bool a_on, unsigned short a_timeout);

	public: void getOption(int a_level, int a_option, void* a_value, int* a_vallen);
	public: int  getOption(SocketBase::Option a_option);
	public: void getLinger(bool& a_on, unsigned short& a_timeout);
			// returns the SO_ERROR value
	public: int	 getSocketError();

	public: void setNonBlockingMode(bool a_on);

};


/**
 * class Socket: a socket.
 */
class Socket : public SocketBase, public InputStream, public OutputStream
{
	protected:
		String c_remotehost;

	friend class ServerSocket;

	public: enum ShutdownMode { ShutdownMode_READ	= 0,
								ShutdownMode_WRITE	= 1,
								ShutdownMode_READWRITE = 2,
			};


	protected: explicit Socket(const String& a_remotehost, int a_remoteport, int a_socket);

	public: explicit Socket() : SocketBase(0) {}

	public: explicit Socket(const String& a_host, int a_port);

	private: Socket(const Socket&);
	private: void operator=(const Socket&);


	public: virtual ~Socket();


	public: const String& getRemoteHost() const { return c_remotehost; }

	public: int getRemotePort() const { return c_port; }


	public: virtual void connect(const String& a_remotehost, int a_remoteport);

	public: void shutdownWrite()
			{
				shutdown(ShutdownMode_WRITE);
			}

	public: void shutdownRead()
			{
				shutdown(ShutdownMode_READ);
			}

	public: virtual void shutdown(Socket::ShutdownMode a_flag = ShutdownMode_READWRITE);


	public: virtual void close();


	public: virtual int read(void* a_buffer, int a_size);
	public: virtual int read(void* a_buffer, int a_size, int a_flags)
			{
				return InputStream::read(a_buffer, a_size, a_flags);
			}


	public: virtual int write(const void* a_buffer, int a_size);

	public: virtual int write(const void* a_buffer, int a_size, int a_flags)
			{
				return OutputStream::write(a_buffer, a_size, a_flags);
			}
	public: virtual int write(const String& a_s, int a_flags = WriteFlag_NONE)
			{
				return OutputStream::write(a_s, a_flags);
			}

};


} // namespace


#endif
