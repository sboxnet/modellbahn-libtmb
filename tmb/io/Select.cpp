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




#include <tmb/io/Select.h>
#include <tmb/lang/String.h>

#include <errno.h>
#include <string.h>

namespace tmb {

void Select::ireset() throw()
{
	FD_ZERO(&c_readfds);
	FD_ZERO(&c_writefds);
	FD_ZERO(&c_exceptfds);
	c_timeout.tv_sec = -1; // infinite wait
	c_timeout.tv_usec = 0;
	c_max_fd_rd = -1;
	c_max_fd_wr = -1;
	c_max_fd_ex = -1;
}

inline int Select::imaxfd() const throw()
{
	return (c_max_fd_rd > c_max_fd_wr ? c_max_fd_rd :
						(c_max_fd_wr > c_max_fd_ex ? c_max_fd_wr : c_max_fd_ex));
}

inline void addHelper(int a_fd, fd_set* a_fdset, int* a_cnt) throw()
{
	FD_SET(a_fd, a_fdset);
	if ( a_fd > *a_cnt )
		*a_cnt = a_fd;
}

void Select::add(int a_fd, Mode a_mode) throw()
{
	if ( a_fd < 0 )
		return;

	if ( a_mode & MODE_RD )
		addHelper(a_fd, &c_readfds, &c_max_fd_rd);
	if ( a_mode & MODE_WR )
		addHelper(a_fd, &c_writefds, &c_max_fd_wr);
	if ( a_mode & MODE_EX )
		addHelper(a_fd, &c_exceptfds, &c_max_fd_ex);
}

inline void removeHelper(int a_fd, fd_set* a_fdset, int* a_cnt) throw()
{
	if ( (a_fd <= *a_cnt) && FD_ISSET(a_fd, a_fdset) )
	{
		FD_CLR(a_fd, a_fdset);
		while ( *a_cnt >= 0 )
		{
			if ( FD_ISSET(*a_cnt, a_fdset) )
				break;
			(*a_cnt)--;
		}
	}
}

void Select::remove(int a_fd, Mode a_mode) throw()
{
	if ( a_fd < 0 )
		return;

	if ( a_mode & MODE_RD )
		removeHelper(a_fd, &c_readfds, &c_max_fd_rd);
	if ( a_mode & MODE_WR )
		removeHelper(a_fd, &c_writefds, &c_max_fd_wr);
	if ( a_mode & MODE_EX )
		removeHelper(a_fd, &c_exceptfds, &c_max_fd_ex);
}

bool Select::isSet(int a_fd, Mode a_mode) const throw()
{
	if ( (a_fd < 0) || (a_fd > imaxfd()) )
		return false;

	bool m_set = true;

	if ( a_mode & MODE_RD )
		m_set = m_set && FD_ISSET(a_fd, &c_readfds);
	if ( a_mode & MODE_WR )
		m_set = m_set && FD_ISSET(a_fd, &c_writefds);
	if ( a_mode & MODE_EX )
		m_set = m_set && FD_ISSET(a_fd, &c_exceptfds);

	return m_set;
}


Select::Result Select::select() const
{
	return select(c_timeout.tv_sec, c_timeout.tv_usec);
}

int Select::select(Select::Result& a_result) const
{
	return select(a_result, c_timeout.tv_sec, c_timeout.tv_usec);
}

Select::Result Select::select(long a_sec, long a_usec) const
{
	Select::Result m_result;
	select(m_result, a_sec, a_usec);
	return m_result;
}

int Select::select(Select::Result& a_result, long a_sec, long a_usec) const
{
	int m_max_fd = imaxfd();

	a_result.ireset();

	if ( m_max_fd < 0 )
	{
		return 0;
	}

	fd_set readfds;
	fd_set writefds;
	fd_set exceptfds;

	fd_set* m_prd = 0;
	fd_set* m_pwr = 0;
	fd_set* m_pex = 0;

	if ( c_max_fd_rd >= 0 )
	{
		readfds = c_readfds;
		m_prd = &readfds;
	}
	if ( c_max_fd_wr >= 0 )
	{
		writefds = c_writefds;
		m_pwr = &writefds;
	}
	if ( c_max_fd_ex >= 0 )
	{
		exceptfds = c_exceptfds;
		m_pex = &exceptfds;
	}

	struct timeval m_timeout;
	struct timeval *m_ptimeout = 0;
	if ( a_sec >= 0 )
	{
		m_timeout.tv_sec = a_sec;
		m_timeout.tv_usec = a_usec;
		m_ptimeout = &m_timeout;
	}

	int m_err = ::select(m_max_fd + 1, m_prd, m_pwr, m_pex, m_ptimeout);
	if ( m_err < 0 )
	{
		if ( errno == EINTR )
			throw InterruptedException();
		throw IOException(String(::strerror(errno)), errno);
	}
	else if ( m_err > 0 )
	{
		if ( m_prd != 0 )
			a_result.c_readfds = readfds;
		if ( m_pwr != 0 )
			a_result.c_writefds = writefds;
		if ( m_pex != 0 )
			a_result.c_exceptfds = exceptfds;
	}
	return m_err;
}


} // namespace
