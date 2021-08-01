/********************************************************************
 * cpp/rmi/RMIObjectContainer.h
 *
 * Copyright (C) 2004 IZB SOFT, Informatik Zentrum Bayern, GmbH
 * All rights reserved.
 *
 * @author  Thomas Maier, is00674
 * @date    2004
 *******************************************************************/





#ifndef _CPP_RMI_RMIOBJECTCONTAINER_H_
#define _CPP_RMI_RMIOBJECTCONTAINER_H_


#include <cpp/lang/Object.h>
#include <cpp/lang/String.h>
#include <cpp/lang/PointerHashMap.h>

#include <cpp/rmi/RMIBase.h>
#include <cpp/rmi/RMIObject.h>

#ifdef CPP_SYS_WINDOWS
# pragma warning ( disable : 4275 4251 )
#endif


namespace cpp {
	namespace rmi {

using namespace cpp::lang;



class CPP_LANG_DLLSPEC RMIObjectContainer : public Object
{
	CPP_OBJECT1(cpp::rmi, RMIObjectContainer, Object);

	protected: PointerHashMap<RMIObject> c_objects;



	public: explicit RMIObjectContainer() throw();

	private: RMIObjectContainer(const RMIObjectContainer&);
	private: void operator=(const RMIObjectContainer&);

	public: virtual ~RMIObjectContainer() throw();



	public: virtual RMIObject* createObject(const String& a_name) throw(Exception);

	// must be implemented by child class
	protected: virtual RMIObject* newObject(const String& a_name) throw(Exception);


	public: virtual void releaseObject(RMIObject* a_obj) throw(Exception);
	protected: virtual void releaseObject(RMIBase::RMIObjectID a_objid) throw(Exception);

	public: virtual void invoke(RMIObject* a_obj,
								const String& a_method,
								const RMIBase::InvokeArgs& a_args,
								const String& a_argio,
								Object** a_pretval = 0) throw(Exception);
	protected: virtual void invoke(RMIBase::RMIObjectID a_objid,
								const String& a_method,
								const RMIBase::InvokeArgs& a_args,
								const String& a_argio,
								Object** a_pretval = 0) throw(Exception);

	public: virtual RMIObject* getObject(RMIBase::RMIObjectID a_id) const throw(NotFoundException);

	protected: virtual void setObjectIDOf(RMIObject* a_obj, RMIBase::RMIObjectID a_objid) const throw();

};



}} // namespace cpp::rmi


#ifdef CPP_SYS_WINDOWS
# pragma warning ( default : 4275 4251 )
#endif

#endif // _CPP_RMI_RMIOBJECTCONTAINER_H_
