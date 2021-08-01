/********************************************************************
 * cpp/rmi/RMIObjectContainer.cpp
 *
 * Copyright (C) 2004 IZB SOFT, Informatik Zentrum Bayern, GmbH
 * All rights reserved.
 *
 * @author  Thomas Maier, is00674
 * @date    2004
 *******************************************************************/





#include <cpp/rmi/RMIObjectContainer.h>

using namespace cpp::lang;
using namespace cpp::io;
using namespace cpp::net;


namespace cpp {
	namespace rmi {



RMIObjectContainer::RMIObjectContainer() throw()
				: Object(),
				  c_objects(AutoDelete_ALL)
{
}

RMIObjectContainer::~RMIObjectContainer() throw()
{
}

// default implementation
RMIObject* RMIObjectContainer::createObject(const String& a_name) throw(Exception)
{
	RMIObject* obj = newObject(a_name);
	if ( !obj )
		throw RMIException(String("RMIObjectContainer::createObject(): unable to create new object ")
							<< a_name);

	// find object id...
	UInt32 obj_id = (UInt32)obj;
	UInt32 base_obj_id = obj_id;
	while ( c_objects.containsKey(obj_id) )
	{
		obj_id++;
		if ( obj_id == base_obj_id )
		{
			delete obj;
			throw RMIException("RMIObjectContainer::createObject(): no more object ID\'s available");
		}
	}

	obj->setRMIObjectID(obj_id);
	
	if ( obj->getRMIObjectName().length() == 0 )
		obj->setRMIObjectName(a_name);

	c_objects.put(obj_id, obj);

	return obj;
}

RMIObject* RMIObjectContainer::newObject(const String& a_name) throw(Exception)
{
	// method must be overwritten by subclasses...
	throw InvalidOperationException();
}


// default implementation
void RMIObjectContainer::releaseObject(RMIObject* a_obj) throw(Exception)
{
	if ( !a_obj )
		throw InvalidOperationException();
	
	releaseObject(a_obj->getRMIObjectID());
}
void RMIObjectContainer::releaseObject(RMIBase::RMIObjectID a_objid) throw(Exception)
{
	if ( !c_objects.remove(a_objid) ) // AutoDelete_ALL on c_objects will also delete the object!
	{
		throw RMIException(String("RMIObjectContainer::releaseObject(): rmi object not found with ID=")
								<< a_objid);
	}
}
// default implementation
void RMIObjectContainer::invoke(RMIObject* a_obj,
								const String& a_method,
								const RMIBase::InvokeArgs& a_args,
								const String& a_argio,
								Object** a_pretval) throw(Exception)
{
	if ( !a_obj )
		throw InvalidOperationException();

	if ( !objIsSame(a_obj->getRMIObjectContainer()) )
	{
		throw RMIException(String("RMIObjectContainer::invoke(): rmi object not found in container with ID=")
								<< a_obj->getRMIObjectID());
	}

	invoke(a_obj->getRMIObjectID(), a_method, a_args, a_argio, a_pretval);
}
void RMIObjectContainer::invoke(RMIBase::RMIObjectID a_objid,
								const String& a_method,
								const RMIBase::InvokeArgs& a_args,
								const String& a_argio,
								Object** a_pretval) throw(Exception)
{
	RMIObject* obj = 0;
	try
	{
		obj = c_objects.get(a_objid);
		if ( !obj )
			throw NotFoundException();
	}
	catch(const NotFoundException&) 
	{
		throw RMIException(String("RMIObjectContainer::invoke(): rmi object not found in container with ID=")
								<< a_objid);
	}

	obj->invoke(a_method, a_args, a_argio, a_pretval);
}


RMIObject* RMIObjectContainer::getObject(RMIBase::RMIObjectID a_id) const throw(NotFoundException)
{
	return c_objects.get(a_id);
}

void RMIObjectContainer::setObjectIDOf(RMIObject* a_obj, RMIBase::RMIObjectID a_objid) const throw()
{
	if ( a_obj )
		a_obj->setRMIObjectID(a_objid);
}


}} // namespace
