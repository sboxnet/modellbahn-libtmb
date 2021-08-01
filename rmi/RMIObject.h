/********************************************************************
 * cpp/rmi/RMIObject.h
 *
 * Copyright (C) 2004 IZB SOFT, Informatik Zentrum Bayern, GmbH
 * All rights reserved.
 *
 * @author  Thomas Maier, is00674
 * @date    2004
 *******************************************************************/





#ifndef _CPP_RMI_RMIOBJECT_H_
#define _CPP_RMI_RMIOBJECT_H_


#include <cpp/lang/Object.h>
#include <cpp/lang/String.h>

#include <cpp/rmi/RMIBase.h>

#ifdef CPP_SYS_WINDOWS
# pragma warning ( disable : 4275 )
#endif


namespace cpp {
	namespace rmi {

class RMIObjectContainer;

class CPP_LANG_DLLSPEC RMIObject : public cpp::lang::Object
{
	CPP_OBJECT1(cpp::rmi, RMIObject, Object);

	private: RMIBase::RMIObjectID	c_rmi_object_id;
	private: cpp::lang::String		c_rmi_object_name;
	private: RMIObjectContainer&	c_rmi_container;
	
	friend class RMIObjectContainer;


	public: explicit RMIObject(RMIObjectContainer& a_container) throw();
	public: explicit RMIObject(const String& a_name, RMIObjectContainer& a_container) throw();

	private: RMIObject(const RMIObject&);
	private: void operator=(const RMIObject&);

	public: virtual ~RMIObject() throw();

	public: virtual RMIBase::RMIObjectID getRMIObjectID() const throw();
	private: virtual void setRMIObjectID(RMIBase::RMIObjectID a_id) throw();

	public: virtual const String& getRMIObjectName() const throw();
	private: virtual void setRMIObjectName(const String& a_name) throw();

	public: virtual const RMIObjectContainer& getRMIObjectContainer() const throw();

	public: virtual void invoke(const String& a_method,
								const RMIBase::InvokeArgs& a_args,
								const String& a_argio,
								Object** a_pretval = 0) throw(Exception);

	public: virtual void release() throw(Exception);
};



}} // namespace cpp::rmi


#ifdef CPP_SYS_WINDOWS
# pragma warning ( default : 4275 )
#endif

#endif // _CPP_RMI_RMIOBJECT_H_
