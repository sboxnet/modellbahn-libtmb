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


#ifndef _TMB_THREAD_MUTEX_H_
#define _TMB_THREAD_MUTEX_H_


#include <tmb/lang/Exception.h>


#ifdef WIN32
# include <windows.h>
#else
# include <pthread.h>
#endif

namespace tmb {

class Mutex
{

	protected:
#ifdef WIN32
		HANDLE			c_mutex;
#else
		pthread_mutex_t	c_mutex;
#endif

	public: Mutex();

	private: Mutex(const Mutex&);
	private: void operator=(const Mutex&);

	public: ~Mutex();

	public: bool lock(int a_timeout_ms = -1);
    
	public: bool trylock();
	public: bool unlock();
};

} // namespace

#endif
