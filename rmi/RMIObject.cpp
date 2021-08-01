/********************************************************************
 * cpp/rmi/RMIObject.cpp
 *
 * Copyright (C) 2004 IZB SOFT, Informatik Zentrum Bayern, GmbH
 * All rights reserved.
 *
 * @author  Thomas Maier, is00674
 * @date    2004
 *******************************************************************/





#include <cpp/rmi/RMIObject.h>
#include <cpp/rmi/RMIObjectContainer.h>

using namespace cpp::lang;
using namespace cpp::io;
using namespace cpp::net;


namespace cpp {
	namespace rmi {

RMIObject::RMIObject(RMIObjectContainer& a_container) throw()
				: Object(),
				  c_rmi_object_id(0),
				  c_rmi_object_name(),
				  c_rmi_container(a_container)
{
}

RMIObject::RMIObject(const String& a_name, RMIObjectContainer& a_container) throw()
				: Object(),
				  c_rmi_object_id(0),
				  c_rmi_object_name(a_name),
				  c_rmi_container(a_container)
{
}

RMIObject::~RMIObject() throw()
{
}

RMIBase::RMIObjectID RMIObject::getRMIObjectID() const throw()
{
	return c_rmi_object_id;
}

void RMIObject::setRMIObjectID(RMIBase::RMIObjectID a_id) throw()
{
	c_rmi_object_id = a_id;
}

const String& RMIObject::getRMIObjectName() const throw()
{
	return c_rmi_object_name;
}

void RMIObject::setRMIObjectName(const String& a_name) throw()
{
	c_rmi_object_name = a_name;
}

const RMIObjectContainer& RMIObject::getRMIObjectContainer() const throw()
{
	return c_rmi_container;
}

void RMIObject::invoke(const String& a_method,
						const RMIBase::InvokeArgs& a_args,
						const String& a_argio,
						Object** a_pretval) throw(Exception)
{
	c_rmi_container.invoke(this, a_method, a_args, a_argio, a_pretval);
}

void RMIObject::release() throw(Exception)
{
	c_rmi_container.releaseObject(this);
}


}} // namespace
