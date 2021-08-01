/********************************************************************
 * cpp/rmi/RMIException.h
 *
 * Copyright (C) 2004 IZB SOFT, Informatik Zentrum Bayern, GmbH
 * All rights reserved.
 *
 * @author  Thomas Maier, is00674
 * @date    2004
 *******************************************************************/





#ifndef _CPP_RMI_RMIEXCEPTION_H_
#define _CPP_RMI_RMIEXCEPTION_H_

#include <cpp/lang/Exception.h>

#ifdef CPP_SYS_WINDOWS
# pragma warning ( disable : 4275 )
#endif

namespace cpp {
	namespace rmi {

using namespace cpp::lang;

class CPP_LANG_DLLSPEC RMIException : public ErrorException
{
	CPP_OBJECT1(cpp::lang, RMIException, ErrorException);

			/// Default constructor
	public: explicit RMIException() throw() : ErrorException() {}
			/// Constructor
	public: explicit RMIException(int a_errno) throw() : ErrorException(a_errno) {}
			/// Constructor
	public: explicit RMIException(const String& a_msg) throw() : ErrorException(a_msg) {}
			/// Constructor
	public: explicit RMIException(const String& a_msg, int a_errno) throw() : ErrorException(a_msg, a_errno) {}
			/// Copy constructor 
	public: RMIException(const RMIException& a_ex) throw() : ErrorException(a_ex) {}
			/// Assignment
	public: virtual const RMIException& operator=(const RMIException& a_ex) throw() { ErrorException::operator=(a_ex); return *this; }
			/// Destructor 
	public: virtual ~RMIException() throw() {}
};

class CPP_LANG_DLLSPEC RMIInvokeException : public RMIException
{
	CPP_OBJECT1(cpp::lang, RMIInvokeException, RMIException);

	protected: String c_source;
	protected: String c_desc;

	public: explicit RMIInvokeException(const String& a_msg, int a_errno,
										const String& a_source, const String& a_desc) throw()
								: RMIException(a_msg, a_errno),
								  c_source(a_source),
								  c_desc(a_desc)
			{
			}

			/// Copy constructor 
	public: RMIInvokeException(const RMIInvokeException& a_ex) throw()
								: RMIException(a_ex),
								  c_source(a_ex.c_source),
								  c_desc(a_ex.c_desc)
			{
			}

			/// Assignment
	public: virtual const RMIInvokeException& operator=(const RMIInvokeException& a_ex) throw()
			{
				RMIException::operator=(a_ex);
				c_source = a_ex.c_source;
				c_desc = a_ex.c_desc;
				return *this;
			}

			/// Destructor 
	public: virtual ~RMIInvokeException() throw() {}

	public: virtual const String& getSource() const throw() { return c_source; }
	public: virtual const String& getDescription() const throw() { return c_desc; }
	public: virtual String getMessage() const throw()
			{
				return RMIException::getMessage() + ", " + c_source + ", " + c_desc;
			}
};

}} // namespace cpp::rmi

#ifdef CPP_SYS_WINDOWS
# pragma warning ( default : 4275 )
#endif

#endif // _CPP_RMI_RMIEXCEPTION_H_
