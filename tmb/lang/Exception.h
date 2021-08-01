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


#ifndef _TMB_LANG_EXCEPTION_H_
# define _TMB_LANG_EXCEPTION_H_

#include <tmb/lang/String.h>

namespace tmb {

/**
 * The base class for all exceptions. Holds a message text.
 */
class Exception
{
	protected:
			String _msg;

	public: explicit Exception() {}
	public: explicit Exception(const String& a_msg) : _msg(a_msg) {}
	public: Exception(const Exception& a_ex) : _msg(a_ex._msg) {}
   	public: virtual const Exception& operator=(const Exception& a_ex) { _msg = a_ex._msg; return *this; }
	public: virtual ~Exception() {}

	public: const String& getMessage() const { return _msg; }
	public: const String& msg() const { return _msg; }
};


/**
 * NumberFormatException
 */
class NumberFormatException : public Exception
{
			/// Default constructor
	public: explicit NumberFormatException() : Exception() {}
			/// Constructor
	public: explicit NumberFormatException(const String& a_msg) : Exception(a_msg) {}
			/// Copy constructor
	public: NumberFormatException(const NumberFormatException& a_ex) : Exception(a_ex) {}
			/// Assignment
	public: virtual const NumberFormatException& operator=(const NumberFormatException& a_ex) throw() { Exception::operator=(a_ex); return *this; }
			/// Destructor
	public: virtual ~NumberFormatException() {}
};

/**
 * IndexOutOfBoundsException
 */
class IndexOutOfBoundsException : public Exception
{
			/// Default constructor
	public: explicit IndexOutOfBoundsException() : Exception() {}
			/// Constructor
	public: explicit IndexOutOfBoundsException(const String& a_msg) : Exception(a_msg) {}
			/// Copy constructor
	public: IndexOutOfBoundsException(const IndexOutOfBoundsException& a_ex) : Exception(a_ex) {}
			/// Assignment
	public: virtual const IndexOutOfBoundsException& operator=(const IndexOutOfBoundsException& a_ex) { Exception::operator=(a_ex); return *this; }
			/// Destructor
	public: virtual ~IndexOutOfBoundsException() {}
};

/**
 * InvalidOperationException
 */
class InvalidOperationException : public Exception
{
			/// Default constructor
	public: explicit InvalidOperationException() : Exception() {}
			/// Constructor
	public: explicit InvalidOperationException(const String& a_msg) : Exception(a_msg) {}
			/// Copy constructor
	public: InvalidOperationException(const InvalidOperationException& a_ex) : Exception(a_ex) {}
			/// Assignment
	public: virtual const InvalidOperationException& operator=(const InvalidOperationException& a_ex) { Exception::operator=(a_ex); return *this; }
			/// Destructor
	public: virtual ~InvalidOperationException() {}
};

/**
 * InvalidParameterException
 */
class InvalidParameterException : public Exception
{
			/// Default constructor
	public: explicit InvalidParameterException() : Exception() {}
			/// Constructor
	public: explicit InvalidParameterException(const String& a_msg) : Exception(a_msg) {}
			/// Copy constructor
	public: InvalidParameterException(const InvalidParameterException& a_ex) : Exception(a_ex) {}
			/// Assignment
	public: virtual const InvalidParameterException& operator=(const InvalidParameterException& a_ex) { Exception::operator=(a_ex); return *this; }
			/// Destructor
	public: virtual ~InvalidParameterException() {}
};


/**
 * NotFoundException
 */
class NotFoundException : public Exception
{
			/// Default constructor
	public: explicit NotFoundException() : Exception() {}
			/// Constructor
	public: explicit NotFoundException(const String& a_msg) : Exception(a_msg) {}
			/// Copy constructor
	public: NotFoundException(const NotFoundException& a_ex) : Exception(a_ex) {}
			/// Assignment
	public: virtual const NotFoundException& operator=(const NotFoundException& a_ex) { Exception::operator=(a_ex); return *this; }
			/// Destructor
	public: virtual ~NotFoundException() {}
};

/**
 * ParseException
 */
class ParseException : public Exception
{
			/// Default constructor
	public: explicit ParseException() : Exception() {}
			/// Constructor
	public: explicit ParseException(const String& a_msg) : Exception(a_msg) {}
			/// Copy constructor
	public: ParseException(const ParseException& a_ex)  : Exception(a_ex) {}
			/// Assignment
	public: virtual const ParseException& operator=(const ParseException& a_ex) { Exception::operator=(a_ex); return *this; }
			/// Destructor
	public: virtual ~ParseException() {}
};


/**
 * InterruptedException
 */
class InterruptedException : public Exception
{
			/// Default constructor
	public: explicit InterruptedException() : Exception() {}
			/// Constructor
	public: explicit InterruptedException(const String& a_msg) : Exception(a_msg) {}
			/// Copy constructor
	public: InterruptedException(const InterruptedException& a_ex) : Exception(a_ex) {}
			/// Assignment
	public: virtual const InterruptedException& operator=(const InterruptedException& a_ex) { Exception::operator=(a_ex); return *this; }
			/// Destructor
	public: virtual ~InterruptedException() {}
};

/**
 * CancelException
 */
class CancelException : public Exception
{
		/// Default constructor
	public: explicit CancelException() : Exception() {}
			/// Constructor
	public: explicit CancelException(const String& a_msg) : Exception(a_msg) {}
			/// Copy constructor
	public: CancelException(const CancelException& a_ex) : Exception(a_ex) {}
			/// Assignment
	public: virtual const CancelException& operator=(const CancelException& a_ex) { Exception::operator=(a_ex); return *this; }
			/// Destructor
	public: virtual ~CancelException() {}
};

/**
 * TryAgainException
 */
class TryAgainException : public Exception
{
			/// Default constructor
	public: explicit TryAgainException() : Exception() {}
			/// Constructor
	public: explicit TryAgainException(const String& a_msg) : Exception(a_msg) {}
			/// Copy constructor
	public: TryAgainException(const TryAgainException& a_ex) : Exception(a_ex) {}
			/// Assignment
	public: virtual const TryAgainException& operator=(const TryAgainException& a_ex) { Exception::operator=(a_ex); return *this; }
			/// Destructor
	public: virtual ~TryAgainException() {}
};




/**
 * ErrorException
 */
class ErrorException : public Exception
{
	protected: int _errno;

			/// Default constructor
	public: explicit ErrorException(int a_errno = 1) : Exception(), _errno(a_errno) {}
			/// Constructor
	public: explicit ErrorException(const String& a_msg, int a_errno = 1) : Exception(a_msg), _errno(a_errno) {}
			/// Copy constructor
	public: ErrorException(const ErrorException& a_ex) : Exception(a_ex), _errno(a_ex._errno) {}
			/// Assignment
	public: virtual const ErrorException& operator=(const ErrorException& a_ex)
			{
				Exception::operator=(a_ex);
				_errno = a_ex._errno;
				return *this;
			}

			/// Destructor
	public: virtual ~ErrorException() {}

	public: int getErrno() const { return _errno; }
	public: String getErrnoStr() const;
	public: String msgErrno() const;
};


class OutOfMemoryException : public Exception
{
			/// Default constructor
	public: explicit OutOfMemoryException() : Exception() {}
			/// Constructor
	public: explicit OutOfMemoryException(const String& a_msg) : Exception(a_msg) {}
			/// Copy constructor
	public: OutOfMemoryException(const OutOfMemoryException& a_ex) : Exception(a_ex) {}
			/// Assignment
	public: virtual const OutOfMemoryException& operator=(const OutOfMemoryException& a_ex) { Exception::operator=(a_ex); return *this; }
			/// Destructor
	public: virtual ~OutOfMemoryException() {}
};

class AccessDeniedException : public ErrorException
{
			/// Default constructor
	public: explicit AccessDeniedException(int a_errno = 1) : ErrorException(a_errno) {}
			/// Constructor
	public: explicit AccessDeniedException(const String& a_msg, int a_errno = 1) : ErrorException(a_msg, a_errno) {}
			/// Copy constructor
	public: AccessDeniedException(const AccessDeniedException& a_ex) : ErrorException(a_ex) {}
			/// Assignment
	public: virtual const AccessDeniedException& operator=(const AccessDeniedException& a_ex)
			{
				ErrorException::operator=(a_ex);
				return *this;
			}

			/// Destructor
	public: virtual ~AccessDeniedException() {}
};


} // namespace


#endif
