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




#ifndef _TMB_SSL_SSLEXCEPTION_H_
#define _TMB_SSL_SSLEXCEPTION_H_

#include <tmb/io/IOException.h>


namespace tmb {

class SSLException : public IOException
{
	public: explicit SSLException() throw() : IOException() {}
			/// Constructor
	public: explicit SSLException(int a_errno) throw() : IOException(a_errno) {}
			/// Constructor
	public: explicit SSLException(const String& a_msg) throw() : IOException(a_msg) {}
			/// Constructor
	public: explicit SSLException(const String& a_msg, int a_errno) throw() : IOException(a_msg, a_errno) {}
			/// Copy constructor
	public: SSLException(const SSLException& a_ex) throw() : IOException(a_ex) {}
			/// Assignment
	public: virtual const SSLException& operator=(const SSLException& a_ex) throw() { IOException::operator=(a_ex); return *this; }
			/// Destructor
	public: virtual ~SSLException() throw() {}
};

} // namespace

#endif
