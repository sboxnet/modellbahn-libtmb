/********************************************************************
 * cpp/rmi/RMIClient.cpp
 *
 * Copyright (C) 2004 IZB SOFT, Informatik Zentrum Bayern, GmbH
 * All rights reserved.
 *
 * @author  Thomas Maier, is00674
 * @date    2004
 *******************************************************************/





#include <cpp/rmi/RMIClient.h>

#include <cpp/lang/ByteArray.h>
#include <cpp/lang/SimpleDataTypeObject.h>

using namespace cpp::lang;
using namespace cpp::io;
using namespace cpp::net;

namespace cpp {
	namespace rmi {


RMIClient::RMIClient(Socket* a_socket) throw()
				: RMIBase(a_socket),
				  RMIObjectContainer()
{
}

RMIClient::~RMIClient() throw()
{
}


RMIObject* RMIClient::createObject(const String& a_name) throw(Exception)
{
	icheckSocket();

	InvokeArgs args(AutoDelete_ALL);
	args.add(new String(a_name));
	Object* retobj = 0;
	rmiInvoke(Cmd_CREATEOBJECT, 0, "createObject", args, "i", &retobj);
	if ( retobj )
	{
		RMIObjectID id = 0;
		if ( retobj->objIsClass(typeid(IntegerObject)) )
		{
			id = (dynamic_cast<IntegerObject*>(retobj))->value();
		}
		delete retobj;
		if ( id == 0 )
			throw RMIException(String("RMIClient::createObject(): invalid rmi object id returned"));
		
		RMIObject* obj = newObject(a_name);
		
		setObjectIDOf(obj, id);

		c_objects.put(id, obj);

		return obj;
	}
	else
		throw RMIException(String("RMIClient::createObject(): failed to create object from name: ")
							<< a_name);
}

RMIObject* RMIClient::newObject(const String& a_name) throw(Exception)
{
	return new RMIObject(a_name, *this);
}


void RMIClient::releaseObject(RMIObject* a_obj) throw(Exception)
{
	if ( !a_obj )
		throw InvalidOperationException();

	icheckSocket();

	RMIObjectID objid = a_obj->getRMIObjectID();

	InvokeArgs args(AutoDelete_ALL);
	args.add(new IntegerObject(objid));
	rmiInvoke(Cmd_RELEASEOBJECT, 0, "releaseObject", args, "i", 0);

	c_objects.remove(objid);
}

void RMIClient::rmiInvoke(RMIBase::Cmd a_cmd, RMIClient::RMIObjectID a_id, const String& a_method,
								const InvokeArgs& a_args,
								const String& a_argio,
								Object** a_pretval) throw(Exception)
{
	icheckSocket();

	{
		int i;

		int datasize = 4						// a_id
					 + (4 + a_method.length())	// a_method
					 + 4;						// numargs

		int numargs = a_args.size();

		// zuerst Packetlänge ermitteln und InvokeArgs überprüfen
		for ( i = 0; i < numargs; i++ )
		{
			Object* argobj = a_args[i];
			if ( argobj == 0 )
				throw InvalidOperationException(String("RMIClient::invoke(): invoke argument ")
												<< i << " not specified");
			datasize += 4; // Argument Flags

			int size = marshalSizeOf(argobj);
			if ( size == 0 )
			{
				throw InvalidOperationException(String("RMIClient::invoke(): unsupported invoke argument type: ")
												<< argobj->objClassName());
			}

			datasize += size;
		}

		ByteArray packet;

		Byte* p = setupPacket(packet, a_cmd, datasize, 0); 

		// a_id
		pack(p, (UInt32)a_id);
		// method
		pack(p, a_method);
		// numargs
		pack(p, (UInt32)numargs);

		int iodeflen = a_argio.length();
		for ( i = 0; i < numargs; i++ )
		{
			Object* argobj = a_args[i];

			int ioflg = InvokeArgFlag_IN;
			if ( i < iodeflen )
			{
				if ( a_argio[i] == 'o' )
					ioflg = InvokeArgFlag_OUT;
				else if ( a_argio[i] == 'x' )
					ioflg = InvokeArgFlag_INOUT;
			}

			pack(p, (UInt32)ioflg);

			marshal(p, argobj);
		}

		sendPacket(packet);
	}
	
	// return...
	{
		int r;

		// check magic
		char magicbuf[4];
		r = c_socket->read(magicbuf, 4, Socket::ReadFlag_FORCECOMPLETESIZE);
		if ( r != 4 )
			throw IOException("RMIClient::rmiInvoke(): unable to read data");
		if ( !c_magic_beg.equals(magicbuf, 4) )
			throw ParseException("begin magic not found");

		// read packet length
		UInt32 pkglen = 0;
		r = c_socket->read((char*)&pkglen, 4, Socket::ReadFlag_FORCECOMPLETESIZE);
		if ( r != 4 )
			throw IOException("RMIClient::rmiInvoke(): unable to read data");
		pkglen = Socket::ntoh(pkglen);
		if ( pkglen < 12 || pkglen > 0x10000000 )
			throw ParseException("invalid packet size");

		// read packet
		ByteArray packet;
		packet.setSize(pkglen);
		r = c_socket->read((char*)packet.data(), packet.size(), Socket::ReadFlag_FORCECOMPLETESIZE);
		if ( r != packet.size() )
			throw IOException("RMIClient::rmiInvoke(): unable to read data");

		// parse packet
		Byte* p = (Byte*)packet.data();
		Byte* pend = p + packet.size() - 4;

		if ( !c_magic_end.equals(pend, 4) )
			throw ParseException("end magic not found");

		// protokoll version
		UInt32 protoversion;
		unpack(p, pend, protoversion);

		// cmd
		UInt32 cmd;
		unpack(p, pend, cmd);

		switch(cmd)
		{
			case Cmd_RETURN:
				{
					UInt32 numoutargs;
					unpack(p, pend, numoutargs);

					Object* retobj = unmarshal(p, pend);

					try
					{
						for ( int i = 0; i < (int)numoutargs; i++ )
						{
							UInt32 argidx;
							unpack(p, pend, argidx);
							Object* obj = unmarshal(p, pend);
							if ( obj && argidx < (UInt32)a_args.size())
							{
								Object* argobj = a_args[argidx];

#define _checkClass(_c) (argobj->oIsKindOf(_c) && obj->oIsClass(_c))
#define _copyClass(_c)  *(dynamic_cast<_c *>(argobj)) = *(dynamic_cast<_c *>(obj))
								if ( _checkClass(ByteObject) )
								{
									_copyClass(ByteObject);
								}
								else if ( _checkClass(BoolObject) )
								{
									_copyClass(BoolObject);
								}
								else if ( _checkClass(ShortObject) )
								{
									_copyClass(ShortObject);
								}
								else if ( _checkClass(IntegerObject) )
								{
									_copyClass(IntegerObject);
								}
								else if ( _checkClass(FloatObject) )
								{
									_copyClass(FloatObject);
								}
								else if ( _checkClass(DoubleObject) )
								{
									_copyClass(DoubleObject);
								}
								else if ( _checkClass(String) )
								{
									_copyClass(String);
								}
								else
								{
									delete obj;
									throw RMIException(String("incompatible [out] parameter: index ")
																<< argidx);
								}
#undef _checkClass
#undef _copyClass
							}
							delete obj;
						}
					}
					catch(const Exception&)
					{
						delete retobj;
						throw;
					}

					if ( a_pretval )
						*a_pretval = retobj;
					else
						delete retobj;
				}
				break;

			case Cmd_ERROR:
				{
					UInt32 errnum;
					unpack(p, pend, errnum);

					String msg;
					unpack(p, pend, msg);

					throw RMIException(msg, errnum);
				}
				break;

			case Cmd_INVOKEEXCEPTION:
				{
					UInt32 errnum;
					unpack(p, pend, errnum);

					String msg, source, desc;
					unpack(p, pend, msg);
					unpack(p, pend, source);
					unpack(p, pend, desc);

					throw RMIInvokeException(msg, errnum, source, desc);
				}
				break;

			default:
				throw RMIException(String("unknown return command"));
				break;
		}
	}
}

void RMIClient::invoke(RMIObject* a_obj, const String& a_method,
								const InvokeArgs& a_args,
								const String& a_argio,
								Object** a_pretval) throw(Exception)
{
	if ( !a_obj )
		throw InvalidOperationException();

	RMIObjectID objid = a_obj->getRMIObjectID();

	rmiInvoke(Cmd_INVOKE, objid, a_method, a_args, a_argio, a_pretval);
}

}} // namespace
