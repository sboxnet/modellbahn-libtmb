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


#ifndef _TMB_SSL_SSLSIGNATURE_H_
#define _TMB_SSL_SSLSIGNATURE_H_


#include <tmb/lang/String.h>
#include <tmb/ssl/SSLException.h>


namespace tmb {

class SSLSignature
{
	protected:
		unsigned int	c_size;
		char*			c_data;

	public: explicit SSLSignature();
	public: explicit SSLSignature(unsigned int a_size);

	private: SSLSignature(const SSLSignature&);
	private: void operator=(const SSLSignature&);

	public: virtual ~SSLSignature();

	public: unsigned int size() const { return c_size; }
	public: void setSize(unsigned int a_size);

	public: const void* data() const { return (const void*)c_data; }

};

} // namespace


#endif
