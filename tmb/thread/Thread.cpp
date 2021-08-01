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


#include <tmb/thread/Thread.h>

#ifndef WIN32
# include <errno.h>
# include <signal.h>
# ifdef __sun__
#  include <thread.h>
# endif
#endif

namespace tmb {


#ifdef WIN32

static DWORD WINAPI __threadRun(void* lpParameter)
{
	Thread* tb = (Thread*)lpParameter;

	if (tb)
		tb->exec();
	return 0;
}

#else

static void* __threadRun(void* lpParameter)
{
	Thread* tb = (Thread*)lpParameter;

	if (tb)
		tb->exec();
	return NULL;
}

#endif


Thread::Thread()
{
	c_thread_id = 0;
	c_thread_state = State_INIT;
#ifdef WIN32
	c_thread_handle = 0;
#endif
	c_thread_retval = 0;
}

Thread::~Thread()
{
#ifdef WIN32
	if ( c_thread_handle )
	{
		::CloseHandle(c_thread_handle);
	}
#endif
}


bool Thread::start()
{
	int state = c_thread_state;
	if ( state == State_INIT || state == State_TERMINATED )
	{
#ifdef WIN32
		ULONG threadid;
		c_thread_handle = ::CreateThread(NULL, 0, __threadRun, this, 0, &threadid);
		if ( c_thread_handle )
		{
			c_thread_id = threadid;
			return true;
		}
#else
		// note: create a detached thread, so that resources are automatically freed after thread exit
		// such threads are NOT joinable! we use WaitEvent c_event_term instead.
		pthread_attr_t attr;
		::pthread_attr_init(&attr);
        ::pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		int r = ::pthread_create(&c_thread_id, &attr, __threadRun, this);
		::pthread_attr_destroy(&attr);
		if ( r == 0 )
		{
			return true;
		}
		else if ( r == ENOMEM || r == EAGAIN )
			throw OutOfMemoryException();
		else if ( r == EPERM )
			throw AccessDeniedException(r);
#endif
	}
	return false;
}

bool Thread::stop()
{
	if ( c_thread_state == State_RUNNING )
	{
		c_thread_state = State_STOPREQUEST;
		return true;
	}
	return false;
}

bool Thread::kill()
{
#ifndef WIN32
	return kill(SIGKILL);
}
bool Thread::kill(int a_signal)
{
#endif
	int state = c_thread_state;
	if ( state == State_RUNNING || state == State_STOPREQUEST )
	{
#ifdef WIN32
		bool term = (::TerminateThread(c_thread_handle, 0) != 0);
#else
		bool term = (pthread_kill(c_thread_id, a_signal) == 0);
#endif
		if ( term && a_signal == SIGKILL )
		{
			c_thread_state = State_TERMINATED;
#ifndef WIN32
			c_event_term.fire();
#endif
			terminated();
		}
		return term;
	}
	return false;
}


unsigned int Thread::getThreadID() const
{
	return c_thread_id;
}

bool Thread::waitForTermination(int a_timeout_ms)
{
	if ( c_thread_state == State_INIT )
		return false;

#ifdef WIN32
	if ( c_thread_state != State_TERMINATED && c_thread_handle )
	{
		DWORD r = ::WaitForSingleObject(c_thread_handle, (a_timeout_ms < 0 ? INFINITE : a_timeout_ms));
		if ( r == WAIT_TIMEOUT )
			throw TryAgainException();
		return (r == WAIT_OBJECT_0);
	}
	return true;
#else
	if ( c_thread_state != State_TERMINATED )
	{
		c_event_term.wait(a_timeout_ms);
	}
	return true;
#endif
}


bool Thread::isRunning() const
{
	return (c_thread_state == (int)State_RUNNING);
}

bool Thread::isStopRequested() const
{
	return (c_thread_state == State_STOPREQUEST);
}

bool Thread::isTerminated() const
{
	return (c_thread_state == State_TERMINATED);
}

int Thread::getThreadRetVal() const
{
	return c_thread_retval;
}

Thread::State Thread::getThreadState() const
{
	return (State)c_thread_state.value();
}

int Thread::exec()
{
	c_thread_state = State_RUNNING;

	c_thread_retval = run();

	c_thread_state = State_TERMINATED;
#ifndef WIN32
	c_event_term.fire();
#endif

	int retval = c_thread_retval;

	// do not access any class members after calling terminated(). maybe terminated()
	// deletes this object!
	terminated();

	return retval;
}


void Thread::terminated()
{
}

} // namespace
