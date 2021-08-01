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


#ifndef _TMB_LANG_SYSTEM_H_
#define _TMB_LANG_SYSTEM_H_


#include <tmb/lang/String.h>
#include <tmb/lang/Exception.h>

namespace tmb {

class System
{
	private: System();
	private: ~System();
	private: System(const System&);
	private: void operator=(const System&);

	public: static String errnoToString(int a_errno);

#ifdef WIN32
	public: static String winErrorToString(long a_errno);
#endif

	public: static String getLogonUserName();

	public: static String getHomePath();
	public: static String getTempPath();

	public: static String getDefaultPrivateKeyFile();


#ifndef WIN32
	public: static void secDisableCoreDumps();
	public: static void secCloseNotStdDescriptors();
	public: static void secResetSigHandlers();
	public: static void secStdDescriptors();
	public: static void secEnvironment();
	public: static void secure();
	
	public: static void wait_s(ulong a_sec);
	public: static void wait_ms(ulong a_msec);
	public: static void wait_us(ulong a_usec);
#endif
};


} // namespace

#endif
