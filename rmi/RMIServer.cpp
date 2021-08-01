/********************************************************************
 * cpp/rmi/RMIServer.cpp
 *
 * Copyright (C) 2004 IZB SOFT, Informatik Zentrum Bayern, GmbH
 * All rights reserved.
 *
 * @author  Thomas Maier, is00674
 * @date    2004
 *******************************************************************/





#include <cpp/rmi/RMIServer.h>

#include <cpp/lang/ByteArray.h>
#include <cpp/lang/SimpleDataTypeObject.h>

using namespace cpp::lang;
using namespace cpp::io;
using namespace cpp::net;


namespace cpp {
	namespace rmi {


RMIServer::RMIServer(Socket* a_socket) throw()
				: RMIBase(a_socket),
				  RMIObjectContainer()
{
}

RMIServer::~RMIServer() throw()
{
}


void RMIServer::processNextRequest() throw(Exception)
{
	icheckSocket();

	InvokeArgs invokeargs(AutoDelete_ALL);
	String invokeio(16);
	String methodname;
	UInt32 cmd;
	UInt32 objid;

	// read input...
	{
		int r;

		// check magic
		char magicbuf[4];
		r = c_socket->read(magicbuf, 4, Socket::ReadFlag_FORCECOMPLETESIZE);
		if ( r != 4 )
			throw IOException("RMIServer::processNextRequest(): unable to read data");
		if ( !c_magic_beg.equals(magicbuf, 4) )
			throw ParseException("begin magic not found");

		// read packet length
		UInt32 pkglen = 0;
		r = c_socket->read((char*)&pkglen, 4, Socket::ReadFlag_FORCECOMPLETESIZE);
		if ( r != 4 )
			throw IOException("RMIServer::processNextRequest(): unable to read data");
		pkglen = Socket::ntoh(pkglen);
		if ( pkglen < 12 || pkglen > 0x10000000 )
			throw ParseException("invalid packet size");

		// read packet
		ByteArray packet;
		packet.setSize(pkglen);
		r = c_socket->read((char*)packet.data(), packet.size(), Socket::ReadFlag_FORCECOMPLETESIZE);
		if ( r != packet.size() )
			throw IOException("RMIServer::processNextRequest(): unable to read data");

		// parse packet
		Byte* p = (Byte*)packet.data();
		Byte* pend = p + packet.size() - 4;

		if ( !c_magic_end.equals(pend, 4) )
			throw ParseException("end magic not found");


		// protokoll version
		UInt32 protoversion;
		unpack(p, pend, protoversion);

		// cmd
		unpack(p, pend, cmd);

		switch ( cmd )
		{
			case Cmd_CREATEOBJECT:
			case Cmd_RELEASEOBJECT:
			case Cmd_INVOKE:
			{
				unpack(p, pend, objid);

				unpack(p, pend, methodname);

				UInt32 numargs;
				unpack(p, pend, numargs);

				if ( numargs )
				{
					invokeargs.setMinBufSize(numargs);

					int i;
					for ( i = 0; i < (int)numargs; i++ )
					{
						UInt32 ioflg;
						unpack(p, pend, ioflg);
						if ( (ioflg & InvokeArgFlag_IN) && (ioflg & InvokeArgFlag_OUT) )
							invokeio << 'x';
						else if ( ioflg & InvokeArgFlag_OUT )
							invokeio << 'o';
						else
							invokeio << 'i';
						
						Object* obj = unmarshal(p, pend);
						if ( !obj )
							throw ParseException("unable to unmarshal object");
						invokeargs.add(obj);
					}
				}
			}
			break;

			case Cmd_RETURN:
			case Cmd_ERROR:
			case Cmd_INVOKEEXCEPTION:
				throw RMIException(String("invalid command: ") << cmd);
				break;

			default:
				throw RMIException(String("unknown command: ") << cmd);
				break;
		}
	}

	
	switch ( cmd )
	{
		case Cmd_CREATEOBJECT:
			{
				if ( invokeargs.size() != 1 || !invokeargs[0]->oIsClass(String) )
					throw RMIException(String("invalid command parameters to createObject"));

				String objname = *(dynamic_cast<String*>(invokeargs[0]));
				RMIObject* obj = 0;
				try
				{
					obj = createObject(objname);
				}
				catch(const ErrorException& ex)
				{
					returnError(ex.getErrno(), ex.getMessage());
				}
				catch(const Exception& ex)
				{
					returnError(1, ex.getMessage());
				}
				if ( obj )
				{
					IntegerObject rv(obj->getRMIObjectID());
					returnValue(&rv);
				}
			}
			break;

		case Cmd_RELEASEOBJECT:
			{
				if ( invokeargs.size() != 1 || !invokeargs[0]->oIsClass(IntegerObject) )
					throw RMIException(String("invalid command parameters to releaseObject"));

				RMIObjectID objid = (dynamic_cast<IntegerObject*>(invokeargs[0]))->value();
				bool ok = false;
				try
				{
					releaseObject(objid);
					ok = true;
				}
				catch(const ErrorException& ex)
				{
					returnError(ex.getErrno(), ex.getMessage());
				}
				catch(const Exception& ex)
				{
					returnError(1, ex.getMessage());
				}
				if ( ok )
				{
					returnValue(0);
				}
			}
			break;

		case Cmd_INVOKE:
			{
				Object* retobj = 0;
				bool ok = false;
				try
				{
					invoke(objid, methodname, invokeargs, invokeio, &retobj);
					ok = true;
				}
				catch(const RMIInvokeException& ex)
				{
					returnInvokeException(ex.getErrno(), ex.getRawMessage(),
											ex.getSource(), ex.getDescription());
				}
				catch(const ErrorException& ex)
				{
					returnError(ex.getErrno(), ex.getMessage());
				}
				catch(const Exception& ex)
				{
					returnError(1, ex.getMessage());
				}
				if ( ok )
				{
					returnValue(retobj, &invokeargs, &invokeio);
				}
			}
			break;

		default:
			break;
	}
}

void RMIServer::returnValue(Object* a_retobj, const RMIBase::InvokeArgs* a_args,
										const String* a_argio) throw(Exception)
{
	UInt32 datasize = 4		// num out args
					+ marshalSizeOf(a_retobj);
	int i;
	int numoutargs = 0;

	if ( a_args && a_argio )
	{
		for ( i = 0; i < a_argio->length() && i < a_args->size(); i++ )
		{
			if ( a_argio->get(i) == 'o' || a_argio->get(i) == 'x' )
			{
				numoutargs++;
				datasize += (4 + marshalSizeOf(a_args->get(i)));
			}
		}
	}

	ByteArray packet;

	Byte* p = setupPacket(packet, Cmd_RETURN, datasize, 0);

	// num out args
	pack(p, (UInt32)numoutargs);
	// ret object
	marshal(p, a_retobj);

	if ( a_args && a_argio )
	{
		for ( i = 0; i < a_argio->length() && i < a_args->size(); i++ )
		{
			if ( a_argio->get(i) == 'o' || a_argio->get(i) == 'x' )
			{
				pack(p, (UInt32)i);
				marshal(p, a_args->get(i));
			}
		}
	}

	sendPacket(packet);
}

void RMIServer::returnInvokeException(int a_errnum, const String& a_msg,
									const String& a_source,
									const String& a_desc) throw(Exception)
{
	int datasize = 4		// errnum
			   + (4 + a_msg.length())
			   + (4 + a_source.length())
			   + (4 + a_desc.length())
			   ;

	ByteArray packet;

	Byte* p = setupPacket(packet, Cmd_INVOKEEXCEPTION, datasize, 0);

	// errnum
	pack(p, (UInt32)a_errnum);
	
	pack(p, a_msg);
	pack(p, a_source);
	pack(p, a_desc);

	sendPacket(packet);
}

void RMIServer::returnError(int a_errnum, const String& a_msg) throw(Exception)
{
	int datasize = 4		// errnum
				+ (4 + a_msg.length())
				;

	ByteArray packet;

	Byte* p = setupPacket(packet, Cmd_ERROR, datasize, 0);

	// errnum
	pack(p, (UInt32)a_errnum);
	
	pack(p, a_msg);

	sendPacket(packet);
}


}} // namespace
