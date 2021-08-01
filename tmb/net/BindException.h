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



#ifndef _TMB_NET_BINDEXCEPTION_H_
#define _TMB_NET_BINDEXCEPTION_H_

#include <tmb/net/SocketException.h>

namespace tmb {

using tmb::String;

/**
 * BindException
 */
class BindException : public SocketException
{
	public: explicit BindException() throw() : SocketException() {}
			/// Constructor
	public: explicit BindException(int a_errno) throw() : SocketException(a_errno) {}
			/// Constructor
	public: explicit BindException(const String& a_msg) throw() : SocketException(a_msg) {}
			/// Constructor
	public: explicit BindException(const String& a_msg, int a_errno) throw() : SocketException(a_msg, a_errno) {}
			/// Copy constructor
	public: BindException(const BindException& a_ex) throw() : SocketException(a_ex) {}
			/// Assignment
	public: virtual const BindException& operator=(const BindException& a_ex) throw() { SocketException::operator=(a_ex); return *this; }
			/// Destructor
	public: virtual ~BindException() throw() {}
};

} // namespace

#endif
