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


#include <tmb/thread/Mutex.h>

#ifndef WIN32
# include <unistd.h>
# include <errno.h>
#endif


namespace tmb {

Mutex::Mutex()
{
#ifdef WIN32
	c_mutex = ::CreateMutex(NULL, FALSE, NULL);
#else
	::pthread_mutex_init(&c_mutex, NULL);
#endif
}

Mutex::~Mutex()
{
#ifdef WIN32
	::CloseHandle(c_mutex);
#else
	::pthread_mutex_unlock(&c_mutex); // to be sure...
	::pthread_mutex_destroy(&c_mutex);
#endif
}

bool Mutex::lock(int a_timeout_ms)
{
#ifdef WIN32
	DWORD r = ::WaitForSingleObject(c_mutex, (a_timeout_ms < 0 ? INFINITE : a_timeout_ms));
	if ( r == WAIT_TIMEOUT )
		throw TryAgainException();
	return (r == WAIT_OBJECT_0);
#else
	int r;
	if ( a_timeout_ms >= 0 )
	{
		struct timespec ts;
		ts.tv_sec = (unsigned long)a_timeout_ms / 1000;
		ts.tv_nsec = ((unsigned long)a_timeout_ms % 1000) * 1000000ul;
		r = ::pthread_mutex_timedlock(&c_mutex, &ts);
		if ( r == ETIMEDOUT )
			throw TryAgainException();
		if ( r == 0 )
			return true;
	}
	else
	{
		r = ::pthread_mutex_lock(&c_mutex);
		if ( r == 0 )
			return true;
	}
	return false;
#endif
}

bool Mutex::trylock()
{
#ifdef WIN32
	return (::WaitForSingleObject(c_mutex, 0) == WAIT_OBJECT_0);
#else
	return (::pthread_mutex_trylock(&c_mutex) == 0);
#endif
}

bool Mutex::unlock()
{
#ifdef WIN32
	return (::ReleaseMutex(c_mutex) != 0);
#else
	return (::pthread_mutex_unlock(&c_mutex) == 0);
#endif
}


} // namespace
