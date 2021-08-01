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


#include <tmb/lang/System.h>

#include <tmb/io/File.h>

#include <stdio.h>
#include <errno.h>
#include <stdarg.h>

#ifdef WIN32
# include <windows.h>
#else
# include <unistd.h>
# include <sys/resource.h>
# include <signal.h>
# include <sys/stat.h>
# include <time.h>
# include <string.h>
# include <stdlib.h>
#endif

namespace tmb {


String System::errnoToString(int a_errno)
{
	return String(::strerror(a_errno));
}


#ifdef WIN32
String System::winErrorToString(long a_errno)
{
	char* pmsg = 0;

	::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
					NULL,
					a_errno,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(LPTSTR)&pmsg,
					0,
					NULL);
	String msg(128);
	if ( pmsg )
	{
		msg = pmsg;
		::LocalFree(pmsg);
		if ( msg.endsWith("\r\n") )
			msg.truncate(msg.length() - 2);
		else if ( msg.endsWith("\n") )
			msg.truncate(msg.length() - 1);
	}
	else
	{
		msg << String::Format("error 0x%08lx (", (long)a_errno) << (long)a_errno << ")";
	}
	return msg;
}
#endif


String System::getLogonUserName()
{
#ifdef WIN32
	char bufusername[130];
	char* username = bufusername;
	DWORD usernamelen = sizeof(bufusername) - 2;
	username[0] = 0;
	if ( ! ::GetUserName(bufusername, &usernamelen) )
	{
		username = ::getenv("USERNAME");
	}
#else //-------------------------------------------------------------
	const char* username = ::getenv("LOGNAME");
	if ( username == NULL || username[0] == 0 )
	{
		username = ::getenv("USER");
	}
#endif
	if ( username != NULL && username[0] != 0 )
	{
		return username;
	}

	return String::null; // empty...
}

String System::getHomePath()
{
	const char* homedir;

#ifdef WIN32
	homedir = ::getenv("HOMESHARE");
	if ( homedir != NULL && homedir[0] != 0 )
		return homedir;
	homedir = ::getenv("USERPROFILE");
	if ( homedir != NULL && homedir[0] != 0 )
		return homedir;
	const char* sysdrive = ::getenv("SystemDrive");
	if ( sysdrive )
		return String(sysdrive) + "\\";
	return "c:\\";
#else
	homedir = ::getenv("HOME");
	if ( homedir != NULL && homedir[0] != 0 )
		return homedir;
	return "/";
#endif
}

String System::getTempPath()
{
	const char* tempdir = ::getenv("TEMP");
	if ( !tempdir )
		tempdir = ::getenv("TMP");
	if ( tempdir != NULL && tempdir[0] != 0 )
	{
		return tempdir;
	}

#ifdef WIN32
	const char* sysdrive = ::getenv("SystemDrive");
	if ( sysdrive )
		return String(sysdrive) + "\\temp";
	return "c:\\temp";
#else
	return "/tmp";
#endif
}


String System::getDefaultPrivateKeyFile()
{
	String keyfile(260);

	keyfile << System::getHomePath()
			<< File::getFileSeparator()
			<< ".ssh"
			<< File::getFileSeparator();

	if ( File(keyfile + "id_rsa").isFile() )
	{
		return keyfile + "id_rsa";
	}
	else if ( File(keyfile + "id_dsa").isFile() )
	{
		return keyfile + "id_dsa";
	}

	throw NotFoundException();
}


#ifndef WIN32

void System::secDisableCoreDumps()
{
	// no core dumps
	struct rlimit r = { 0, 0 };
	::setrlimit(RLIMIT_CORE, &r);
}

void System::secCloseNotStdDescriptors()
{
	// close all file descriptors > 2
	int n = ::sysconf(_SC_OPEN_MAX);
	if (n < 0)
		n = ::getdtablesize();
	while (--n > 2)
		::close(n);
}

void System::secResetSigHandlers()
{
	// set all signal handlers to their default action
	for (int i = 0; i <= NSIG; i++)
		::signal(i, SIG_DFL);
}

void System::secStdDescriptors()
{
	// connect a standard file/descriptor to /dev/null
	// if the descriptor is closed
	FILE* fps[3] = { stdin, stdout, stderr };
	const char* modes[3] = { "rb", "wb", "wb" };
	struct stat buf;
    FILE *f = NULL;
	for (int i = 0; i < 3; i++)
	{
		if (fstat(i, &buf) == 0) // descriptor open?
			continue;
        f = freopen("/dev/null", modes[i], fps[i]);
    }
}

void System::secEnvironment()
{
	// secure environment
	::unsetenv("IFS");
}

void System::secure()
{
	secDisableCoreDumps();
	secResetSigHandlers();

	::umask(0077);

	secEnvironment();
	secCloseNotStdDescriptors();
	secStdDescriptors();
}

#endif // not WIN32


// log functions from tmb.h

static inline void vlogmsg(FILE* a_stream, const char* a_prefix, const char* a_file,
					 		int a_line, const char* a_fmt, va_list a_va)
{
	::fprintf(a_stream, "%s:%s:%d: ", a_prefix, a_file, a_line);
	::vfprintf(a_stream, a_fmt, a_va);
	::fflush(a_stream);
}

void logFatal(const char* a_file, int a_line, const char* a_fmt, ...)
{
	va_list va;
	va_start(va, a_fmt);
    vlogmsg(stderr, "FATAL", a_file, a_line, a_fmt, va);
	va_end(va);
}

void logError(const char* a_file, int a_line, const char* a_fmt, ...)
{
	va_list va;
	va_start(va, a_fmt);
    vlogmsg(stderr, "ERROR", a_file, a_line, a_fmt, va);
	va_end(va);
}

void logWarn(const char* a_file, int a_line, const char* a_fmt, ...)
{
	va_list va;
	va_start(va, a_fmt);
    vlogmsg(stderr, "WARNING", a_file, a_line, a_fmt, va);
	va_end(va);
}

void logInfo(const char* a_fmt, ...)
{
	va_list va;
    va_start(va, a_fmt);
	::vfprintf(stdout, a_fmt, va);
	va_end(va);
	::fflush(stdout);
}

void logDebug(const char* a_file, int a_line, uint32 a_mask, const char* a_fmt, ...)
{
	va_list va;
	va_start(va, a_fmt);
    vlogmsg(stderr, "DEBUG", a_file, a_line, a_fmt, va);
	va_end(va);
}



void System::wait_s(ulong a_sec) {
	struct timespec ts;
	ts.tv_sec = a_sec;
	ts.tv_nsec = 0;
	while (::nanosleep(&ts, &ts) == -1 && errno == EINTR) {
	}
}

void System::wait_ms(ulong a_msec) {
	struct timespec ts;
	ts.tv_sec = a_msec / 1000ul;
	ts.tv_nsec = (a_msec % 1000ul) * 1000000ul;
	while (::nanosleep(&ts, &ts) == -1 && errno == EINTR) {
	}
}

void System::wait_us(ulong a_usec) {
	struct timespec ts;
	ts.tv_sec = a_usec / 1000000ul;
	ts.tv_nsec = (a_usec % 1000000ul) * 1000ul;
	while (::nanosleep(&ts, &ts) == -1 && errno == EINTR) {
	}
}


} // namespace
