/********************************************************************
 * cpp/rmi/RMIServer.h
 *
 * Copyright (C) 2004 IZB SOFT, Informatik Zentrum Bayern, GmbH
 * All rights reserved.
 *
 * @author  Thomas Maier, is00674
 * @date    2004
 *******************************************************************/





#ifndef _CPP_RMI_RMISERVER_H_
#define _CPP_RMI_RMISERVER_H_


#include <cpp/lang/Object.h>
#include <cpp/lang/String.h>

#include <cpp/rmi/RMIObjectContainer.h>
#include <cpp/rmi/RMIBase.h>

#include <cpp/net/Socket.h>


#ifdef CPP_SYS_WINDOWS
# pragma warning ( disable : 4275 )
#endif


namespace cpp {
	namespace rmi {

using namespace cpp::lang;
using namespace cpp::net;


class CPP_LANG_DLLSPEC RMIServer : public RMIBase, public RMIObjectContainer
{
	CPP_OBJECT2(cpp::rmi, RMIServer, RMIBase, RMIObjectContainer);


	public: explicit RMIServer(Socket* a_socket = 0) throw();

	private: RMIServer(const RMIServer&);
	private: void operator=(const RMIServer&);

	public: virtual ~RMIServer() throw();


	public: virtual void processNextRequest() throw(Exception);


	protected: virtual void returnError(int a_errnum, const String& a_msg) throw(Exception);

	protected: virtual void returnInvokeException(int a_errnum, const String& a_msg,
												const String& a_source,
												const String& a_desc) throw(Exception);

	protected: virtual void returnValue(Object* a_retobj, const RMIBase::InvokeArgs* a_args = 0,
										const String* a_argio = 0) throw(Exception);
};


typedef cpp::lang::ObjectPtr<RMIServer> RMIServerPtr;

}} // namespace cpp::rmi


#ifdef CPP_SYS_WINDOWS
# pragma warning ( default : 4275 )
#endif

#endif // _CPP_RMI_RMISERVER_H_
