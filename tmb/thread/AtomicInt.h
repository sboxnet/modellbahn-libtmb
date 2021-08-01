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


#ifndef _TMB_THREAD_ATOMICINT_H_
#define _TMB_THREAD_ATOMICINT_H_


#ifdef WIN32
# include <windows.h>
#else
//# include <bits/atomicity.h>
// since gcc 4.2
# include <ext/atomicity.h>
#endif


namespace tmb {

class AtomicInt
{
#ifdef WIN32
	protected: LONG c_int;

	public: void setValue(int a_int) throw()
			{
				::InterlockedExchange(&c_int, (LONG)a_int);
			}

	public: AtomicInt& operator = (int a_int) throw()
			{
				::InterlockedExchange(&c_int, (LONG)a_int);
				return *this;
			}
	public: AtomicInt& operator = (const AtomicInt& a_int) throw()
			{
				::InterlockedExchange(&c_int, (LONG)a_int.c_int);
				return *this;
			}

	public: int operator ++ (int) throw() // prefix i++
			{
				return (int)::InterlockedExchangeAdd(&c_int, 1);
			}
	public: int operator -- (int) throw() // i--
			{
				return (int)::InterlockedExchangeAdd(&c_int, -1);
			}

	public: void operator += (int a_int) throw()
			{
				::InterlockedExchangeAdd(&c_int, (LONG)a_int);
			}
	public: void operator -= (int a_int) throw()
			{
				::InterlockedExchangeAdd(&c_int, - (LONG)a_int);
			}

#else

	protected: _Atomic_word		c_int;

	public: void setValue(int a_int) throw()
			{
				c_int = a_int;
			}

	public: AtomicInt& operator = (int a_int) throw()
			{
				c_int = a_int;
				return *this;
			}
	public: AtomicInt& operator = (const AtomicInt& a_int) throw()
			{
				c_int = a_int.c_int;
				return *this;
			}

	public: int operator ++ (int) throw() // prefix i++
			{
				return (int)__gnu_cxx::__exchange_and_add(&c_int, 1);
			}
	public: int operator -- (int) throw() // prefix i--
			{
				return (int)__gnu_cxx::__exchange_and_add(&c_int, -1);
			}

	public: void operator += (int a_int) throw()
			{
				__gnu_cxx::__atomic_add(&c_int, a_int);
			}
	public: void operator -= (int a_int) throw()
			{
				__gnu_cxx::__atomic_add(&c_int, -a_int);
			}

#endif

	public: AtomicInt() throw() {}
	public: AtomicInt(int a_int) throw() : c_int(a_int) {}

	public: int value() const throw() { return c_int; }

	public: bool operator == (const AtomicInt& a_int) const throw() { return c_int == a_int.c_int; }
	public: bool operator != (const AtomicInt& a_int) const throw() { return c_int != a_int.c_int; }
	public: bool operator <  (const AtomicInt& a_int) const throw() { return c_int <  a_int.c_int; }
	public: bool operator <= (const AtomicInt& a_int) const throw() { return c_int <= a_int.c_int; }
	public: bool operator >  (const AtomicInt& a_int) const throw() { return c_int >  a_int.c_int; }
	public: bool operator >= (const AtomicInt& a_int) const throw() { return c_int >= a_int.c_int; }

	public: bool operator ! () const throw() { return !c_int; }

	public: bool operator == (int a_int) const throw() { return c_int == a_int; }
	public: bool operator != (int a_int) const throw() { return c_int != a_int; }
	public: bool operator <  (int a_int) const throw() { return c_int <  a_int; }
	public: bool operator <= (int a_int) const throw() { return c_int <= a_int; }
	public: bool operator >  (int a_int) const throw() { return c_int >  a_int; }
	public: bool operator >= (int a_int) const throw() { return c_int >= a_int; }

	public: operator int () const { return c_int; }

};

} // namespace

#endif
