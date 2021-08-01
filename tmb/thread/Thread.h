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

#ifndef _TMB_THREAD_THREAD_H_
#define _TMB_THREAD_THREAD_H_


#include <tmb/lang/Exception.h>

#include <tmb/thread/AtomicInt.h>
#include <tmb/thread/WaitEvent.h>

#ifdef WIN32
# include <windows.h>
#else
# include <pthread.h>
#endif


namespace tmb {

class Thread
{
	public: enum State { State_INIT,
						 State_RUNNING,
						 State_STOPREQUEST,
						 State_TERMINATED,
			};

	protected:
		AtomicInt	c_thread_state;
#ifdef WIN32
		unsigned int	c_thread_id;
		HANDLE		c_thread_handle;
#else
		pthread_t	c_thread_id;
		WaitEvent	c_event_term;
#endif
		AtomicInt	c_thread_retval;


	public: Thread();

	private: Thread(const Thread&);
	private: void operator=(const Thread&);

	public: virtual ~Thread();


	public: virtual bool start();
	public: virtual bool stop();

	public: virtual bool kill();
#ifndef WIN32
	public: virtual bool kill(int a_signal);
#endif

	// callback: run the thread
	public: virtual int run() = 0;
	// callback: thread execution ended normally
	public: virtual void terminated();

	public: virtual unsigned int getThreadID() const;

	public: virtual bool waitForTermination(int a_timeout_ms = -1);

	public: virtual bool isRunning() const;
	public: virtual bool isStopRequested() const;
	public: virtual bool isTerminated() const;

	public: virtual int getThreadRetVal() const;

	public: virtual State getThreadState() const;

	public: virtual int exec();
};



} // namespace

#endif
