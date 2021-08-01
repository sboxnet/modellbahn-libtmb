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

#include <tmb/thread/WaitEvent.h>

#ifndef WIN32
#include <errno.h>
#endif

namespace tmb {

WaitEvent::WaitEvent(bool a_use_broadcast)
{
	c_use_broadcast = a_use_broadcast;

#ifdef WIN32
	c_event = ::CreateEvent(NULL, c_use_broadcast, FALSE, NULL);
#else
	::pthread_mutex_init(&c_mutex, NULL);
	::pthread_cond_init(&c_cond, NULL);
#endif
}

WaitEvent::~WaitEvent()
{
#ifdef WIN32
	::CloseHandle(c_event);
#else
	::pthread_cond_broadcast(&c_cond); // to be sure...
	::pthread_cond_destroy(&c_cond);
	::pthread_mutex_unlock(&c_mutex); // to be sure...
	::pthread_mutex_destroy(&c_mutex);
#endif
}


bool WaitEvent::wait(int a_timeout_ms)
{
#ifdef WIN32
	DWORD r = ::WaitForSingleObject(c_event, (a_timeout_ms < 0 ? INFINITE : a_timeout_ms));
	if ( r == WAIT_TIMEOUT )
		throw TryAgainException();
	return (r == WAIT_OBJECT_0);
#else
	if ( a_timeout_ms >= 0 )
	{
		struct timespec ts;
		ts.tv_sec = (unsigned long)a_timeout_ms / 1000;
		ts.tv_nsec = ((unsigned long)a_timeout_ms % 1000) * 1000000ul;
		::pthread_mutex_lock(&c_mutex);
		int r = ::pthread_cond_timedwait(&c_cond, &c_mutex, &ts);
		::pthread_mutex_unlock(&c_mutex);
		if ( r == ETIMEDOUT )
			throw TryAgainException();
		if ( r != 0 )
			return false;
	}
	else
	{
		::pthread_mutex_lock(&c_mutex);
		::pthread_cond_wait(&c_cond, &c_mutex);
		::pthread_mutex_unlock(&c_mutex);
	}
	return true;
#endif
}

void WaitEvent::fire()
{
#ifdef WIN32
	::SetEvent(c_event);
#else
	if ( c_use_broadcast )
		::pthread_cond_broadcast(&c_cond);
	else
		::pthread_cond_signal(&c_cond);
#endif
}


} // namespace
