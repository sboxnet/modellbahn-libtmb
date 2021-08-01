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


#ifndef _TMB_IO_SELECT_H_
#define _TMB_IO_SELECT_H_


#include <tmb/io/IOException.h>
#include <tmb/net/Socket.h>
#include <tmb/net/ServerSocket.h>


# include <sys/time.h>
# include <sys/types.h>
# include <unistd.h>

namespace tmb {


/**
 * class Select: this class represents the select() system call.
 */
class Select
{
	public: enum Mode { MODE_RD = 0x1,
						MODE_WR = 0x2,
						MODE_RW = MODE_RD|MODE_WR,
						MODE_EX = 0x4,
						MODE_ALL = MODE_RW|MODE_EX };

	protected: fd_set c_readfds;
	protected: fd_set c_writefds;
	protected: fd_set c_exceptfds;
	protected: struct timeval c_timeout;
	protected: int c_max_fd_rd;
	protected: int c_max_fd_wr;
	protected: int c_max_fd_ex;

			/**
			 * class Select::Result: represent the result of a select() call
			 */
	public: class Result
			{
				protected: fd_set c_readfds;
				protected: fd_set c_writefds;
				protected: fd_set c_exceptfds;

				friend class Select;

				private: void ireset() throw()
						{
							FD_ZERO(&c_readfds);
							FD_ZERO(&c_writefds);
							FD_ZERO(&c_exceptfds);
						}

						///
				public: explicit Result() throw()
						{
							ireset();
						}

						///
				public: Result(const Result& a_r) throw()
						{
							c_readfds = a_r.c_readfds;
							c_writefds = a_r.c_writefds;
							c_exceptfds = a_r.c_exceptfds;
						}

						///
				public: const Result& operator=(const Result& a_r) throw()
						{
							if ( this != &a_r )
							{
								c_readfds = a_r.c_readfds;
								c_writefds = a_r.c_writefds;
								c_exceptfds = a_r.c_exceptfds;
							}
							return *this;
						}

						///
				public: virtual ~Result() throw() {}

						///
				public: bool canRead(int a_fd) const throw() { return FD_ISSET(a_fd, &c_readfds) != 0; }
						///
				public: bool canWrite(int a_fd) const throw() { return FD_ISSET(a_fd, &c_writefds) != 0; }
						///
				public: bool wasInterrupted(int a_fd) const throw() { return FD_ISSET(a_fd, &c_exceptfds) != 0; }

						///
				public: bool canRead(const Socket& a_fd) const throw() { return canRead(a_fd.getSocketHandle()); }
						///
				public: bool canWrite(const Socket& a_fd) const throw() { return canWrite(a_fd.getSocketHandle()); }
						///
				public: bool wasInterrupted(const Socket& a_fd) const throw() { return wasInterrupted(a_fd.getSocketHandle()); }

				public: bool canRead(const ServerSocket& a_fd) const throw() { return canRead(a_fd.getSocketHandle()); }
						///
				public: bool canWrite(const ServerSocket& a_fd) const throw() { return canWrite(a_fd.getSocketHandle()); }
						///
				public: bool wasInterrupted(const ServerSocket& a_fd) const throw() { return wasInterrupted(a_fd.getSocketHandle()); }
			};

	private: int imaxfd() const throw();
	private: void ireset() throw();

			///
	public: explicit Select() throw()
			{
				ireset();
			}

	private: Select(const Select&);
	private: void operator=(const Select&);

			///
	public: virtual ~Select() throw() {}

			///
	public: void add(int a_fd, Mode a_mode = MODE_RW) throw();
			///
	public: void addRD(int a_fd) throw() { add(a_fd, MODE_RD); }
			///
	public: void addWR(int a_fd) throw() { add(a_fd, MODE_WR); }
			///
	public: void addEX(int a_fd) throw() { add(a_fd, MODE_EX); }

			///
	public: void add(const Socket& a_fd, Mode a_mode = MODE_RW) throw()
			{
				add(a_fd.getSocketHandle(), a_mode);
			}
			///
	public: void add(const ServerSocket& a_fd, Mode a_mode = MODE_RW) throw()
			{
				add(a_fd.getSocketHandle(), a_mode);
			}


			///
	public: void remove(int a_fd, Mode a_mode = MODE_RW) throw();
			///
	public: void removeRD(int a_fd) throw() { remove(a_fd, MODE_RD); }
			///
	public: void removeWR(int a_fd) throw() { remove(a_fd, MODE_WR); }
			///
	public: void removeEX(int a_fd) throw() { remove(a_fd, MODE_EX); }

			///
	public: void remove(const Socket& a_fd, Mode a_mode = MODE_RW) throw()
			{
				remove(a_fd.getSocketHandle(), a_mode);
			}
			///
	public: void remove(const ServerSocket& a_fd, Mode a_mode = MODE_RW) throw()
			{
				remove(a_fd.getSocketHandle(), a_mode);
			}


			///
	public: bool isSet(int a_fd, Mode a_mode = MODE_RW) const throw();
			///
	public: bool isSetRD(int a_fd) throw() { return isSet(a_fd, MODE_RD); }
			///
	public: bool isSetWR(int a_fd) throw() { return isSet(a_fd, MODE_WR); }
			///
	public: bool isSetEX(int a_fd) throw() { return isSet(a_fd, MODE_EX); }

			///
	public: bool isSet(const Socket& a_fd, Mode a_mode = MODE_RW) const throw()
			{
				return isSet(a_fd.getSocketHandle(), a_mode);
			}
			///
	public: bool isSetRD(const Socket& a_fd) throw() { return isSet(a_fd, MODE_RD); }
			///
	public: bool isSetWR(const Socket& a_fd) throw() { return isSet(a_fd, MODE_WR); }
			///
	public: bool isSetEX(const Socket& a_fd) throw() { return isSet(a_fd, MODE_EX); }

			///
	public: void clear() throw()
			{
				ireset();
			}

			///
	public: void setTimeout(long a_sec, long a_usec = 0) throw()
			{
				c_timeout.tv_sec = a_sec;
				c_timeout.tv_usec = a_usec;
			}

			///
	public: Select::Result select() const;
			///
	public: int select(Select::Result& a_result) const;
			///
	public: Select::Result select(long a_sec, long a_usec = 0) const;
			///
	public: int select(Select::Result& a_result, long a_sec, long a_usec = 0) const;
};


} // namespace


#endif
