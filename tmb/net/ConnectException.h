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



#ifndef _TMB_NET_CONNECTEXCEPTION_H_
#define _TMB_NET_CONNECTEXCEPTION_H_

#include <tmb/net/SocketException.h>

namespace tmb {

/**
 * ConnectException
 */
class ConnectException : public SocketException
{
			/// Default constructor
	public: explicit ConnectException() throw() : SocketException() {}
			/// Constructor
	public: explicit ConnectException(int a_errno) throw() : SocketException(a_errno) {}
			/// Constructor
	public: explicit ConnectException(const String& a_msg) throw() : SocketException(a_msg) {}
			/// Constructor
	public: explicit ConnectException(const String& a_msg, int a_errno) throw() : SocketException(a_msg, a_errno) {}
			/// Copy constructor
	public: ConnectException(const ConnectException& a_ex) throw() : SocketException(a_ex) {}
			/// Assignment
	public: virtual const ConnectException& operator=(const ConnectException& a_ex) throw() { SocketException::operator=(a_ex); return *this; }
			/// Destructor
	public: virtual ~ConnectException() throw() {}
};

} // namespace

#endif
