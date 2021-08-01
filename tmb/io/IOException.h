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


#ifndef _TMB_IO_IOEXCEPTION_H_
#define _TMB_IO_IOEXCEPTION_H_

#include <tmb/lang/Exception.h>

namespace tmb {

/**
 * IOException
 */
class IOException : public ErrorException
{
			/// Default constructor
	public: explicit IOException(int a_errno = 1) throw() : ErrorException(a_errno) {}
			/// Constructor
	public: explicit IOException(const String& a_msg, int a_errno = 1) throw() : ErrorException(a_msg, a_errno) {}
			/// Copy constructor
	public: IOException(const IOException& a_ex) throw() : ErrorException(a_ex) {}
			/// Assignment
	public: virtual const IOException& operator=(const IOException& a_ex) throw() { ErrorException::operator=(a_ex); return *this; }
			/// Destructor
	public: virtual ~IOException() throw() {}
};

} // namespace

#endif
