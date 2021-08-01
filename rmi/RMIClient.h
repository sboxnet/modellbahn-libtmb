/********************************************************************
 * cpp/rmi/RMIClient.h
 *
 * Copyright (C) 2004 IZB SOFT, Informatik Zentrum Bayern, GmbH
 * All rights reserved.
 *
 * @author  Thomas Maier, is00674
 * @date    2004
 *******************************************************************/





#ifndef _CPP_RMI_RMICLIENT_H_
#define _CPP_RMI_RMICLIENT_H_


#include <cpp/net/Socket.h>

#include <cpp/rmi/RMIException.h>
#include <cpp/rmi/RMIBase.h>
#include <cpp/rmi/RMIObjectContainer.h>

#ifdef CPP_SYS_WINDOWS
# pragma warning ( disable : 4275 )
#endif


namespace cpp {
	namespace rmi {

using namespace cpp::lang;
using namespace cpp::net;



class CPP_LANG_DLLSPEC RMIClient : public RMIBase, public RMIObjectContainer
{
	CPP_OBJECT2(cpp::rmi, RMIClient, RMIBase, RMIObjectContainer);

	public: explicit RMIClient(Socket* a_socket = 0) throw();

	private: RMIClient(const RMIClient&);
	private: void operator=(const RMIClient&);

	public: virtual ~RMIClient() throw();

	public: virtual RMIObject* createObject(const String& a_name) throw(Exception);

	protected: virtual RMIObject* newObject(const String& a_name) throw(Exception);


	public: virtual void releaseObject(RMIObject* a_obj) throw(Exception);

	protected: virtual void rmiInvoke(RMIBase::Cmd a_cmd, RMIObjectID a_id, const String& a_method,
								const InvokeArgs& a_args,
								const String& a_argio,
								Object** a_pretval = 0) throw(Exception);

	public: virtual void invoke(RMIObject* a_obj, const String& a_method,
								const InvokeArgs& a_args,
								const String& a_argio,
								Object** a_pretval = 0) throw(Exception);
};


typedef cpp::lang::ObjectPtr<RMIClient> RMIClientPtr;

}} // namespace cpp::rmi


#ifdef CPP_SYS_WINDOWS
# pragma warning ( default : 4275 )
#endif

#endif // _CPP_RMI_RMICLIENT_H_
