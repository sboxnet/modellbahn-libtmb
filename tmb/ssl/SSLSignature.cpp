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


#include <tmb/ssl/SSLSignature.h>


#include <openssl/ssl.h>
#include <openssl/ssl3.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#include <string.h>

namespace tmb {


SSLSignature::SSLSignature()
				: c_size(0),
				  c_data(0)
{
}

SSLSignature::SSLSignature(unsigned int a_size)
				: c_size(0),
				  c_data(0)
{
	if ( a_size > 0 )
	{
		c_data = new char [a_size];
		c_size = a_size;
	}
}


SSLSignature::~SSLSignature()
{
	if ( c_data )
	{
		delete [] c_data;
		c_data = 0;
	}
}

void SSLSignature::setSize(unsigned int a_size)
{
	if ( a_size == c_size )
		return;

	if ( c_data )
		delete [] c_data;

	c_data = 0;
	c_size = 0;

	if ( a_size > 0 )
	{
		c_data = new char [a_size];
		c_size = a_size;
	}
}

} // namespace
