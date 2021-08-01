/********************************************************************
 * cpp/rmi/RMIBase.cpp
 *
 * Copyright (C) 2004 IZB SOFT, Informatik Zentrum Bayern, GmbH
 * All rights reserved.
 *
 * @author  Thomas Maier, is00674
 * @date    2004
 *******************************************************************/





#include <cpp/rmi/RMIBase.h>

#include <cpp/lang/SimpleDataTypeObject.h>

using namespace cpp::lang;
using namespace cpp::io;
using namespace cpp::net;

#include <string.h>

namespace cpp {
	namespace rmi {

RMIBase::RMIBase(Socket* a_socket) throw()
				: Object(),
				  c_socket(a_socket),
				  c_magic_beg("RMIB"),
				  c_magic_end("RMIE")
{
}

RMIBase::~RMIBase() throw()
{
}

inline void RMIBase::pack(Byte*& a_p, Byte a_data) const throw()
{
	*a_p++ = a_data;
}

inline void RMIBase::pack(Byte*& a_p, UInt16 a_data) const throw()
{
	UInt16 d = Socket::hton(a_data);
	::memcpy(a_p, &d, 2);
	a_p += 2;
}

inline void RMIBase::pack(Byte*& a_p, UInt32 a_data) const throw()
{
	UInt32 d = Socket::hton(a_data);
	::memcpy(a_p, &d, 4);
	a_p += 4;
}

inline void RMIBase::pack(Byte*& a_p, const String& a_data) const throw()
{
	int len = a_data.length();
	pack(a_p, (UInt32)len);
	if ( len )
	{
		a_data.copy(a_p, len);
		a_p += len;
	}
}


inline void RMIBase::unpack(Byte*& a_p, Byte* a_end, Byte& a_data) const throw(ParseException)
{
	if ( a_p + 1 > a_end )
		throw ParseException();
	a_data = *a_p++;
}

inline void RMIBase::unpack(Byte*& a_p, Byte* a_end, UInt16& a_data) const throw(ParseException)
{
	if ( a_p + 2 > a_end )
		throw ParseException();
	::memcpy(&a_data, a_p, 2);
	a_p += 2;
	a_data = Socket::ntoh(a_data);
}

inline void RMIBase::unpack(Byte*& a_p, Byte* a_end, UInt32& a_data) const throw(ParseException)
{
	if ( a_p + 4 > a_end )
		throw ParseException();
	::memcpy(&a_data, a_p, 4);
	a_p += 4;
	a_data = Socket::ntoh(a_data);
}

inline void RMIBase::unpack(Byte*& a_p, Byte* a_end, String& a_data) const throw(ParseException)
{
	UInt32 len;
	unpack(a_p, a_end, len);
	if ( a_p + len > a_end )
		throw ParseException();
	if ( len )
	{
		String s(len, 0);
		::memcpy((void*)(s.c_str()), a_p, len);
		a_p += len;
		a_data = s;
	}
	else
	{
		a_data.clear();
	}
}

inline int RMIBase::packSizeOf(const String& a_str) const throw()
{
	return (4 + a_str.length());
}



inline void RMIBase::marshal(Byte*& a_p, Byte a_data) const throw()
{
	pack(a_p, DataType_UINT8);
	*a_p++ = a_data;
}

inline void RMIBase::marshal(Byte*& a_p, Int16 a_data) const throw()
{
	pack(a_p, DataType_INT16);
	pack(a_p, (UInt16)a_data);
}

inline void RMIBase::marshal(Byte*& a_p, UInt16 a_data) const throw()
{
	pack(a_p, DataType_UINT16);
	pack(a_p, a_data);
}

inline void RMIBase::marshal(Byte*& a_p, Int32 a_data) const throw()
{
	pack(a_p, DataType_INT32);
	pack(a_p, (UInt32)a_data);
}

inline void RMIBase::marshal(Byte*& a_p, UInt32 a_data) const throw()
{
	pack(a_p, DataType_UINT32);
	pack(a_p, a_data);
}

inline void RMIBase::marshal(Byte*& a_p, bool a_data) const throw()
{
	pack(a_p, DataType_BOOL);
	pack(a_p, (Byte)(a_data ? 1 : 0));
}

inline void RMIBase::marshal(Byte*& a_p, float a_data) const throw()
{
	pack(a_p, DataType_FLOAT);
	pack(a_p, *(UInt32*)(&a_data));
}

inline void RMIBase::marshal(Byte*& a_p, double a_data) const throw()
{
	pack(a_p, DataType_DOUBLE);
	pack(a_p, ((UInt32*)(&a_data))[0]);
	pack(a_p, ((UInt32*)(&a_data))[1]);
}

inline void RMIBase::marshal(Byte*& a_p, const String& a_data) const throw()
{
	pack(a_p, DataType_STRING);
	pack(a_p, a_data);
}

void RMIBase::marshal(Byte*& a_p, const Object* a_data) const throw(InvalidOperationException)
{
	if ( !a_data )
	{
		pack(a_p, DataType_VOID);
	}
	else if ( a_data->oIsKindOf(ByteObject) )
	{
		marshal(a_p, (Byte)(dynamic_cast<const ByteObject*>(a_data))->value());
	}
	else if ( a_data->oIsKindOf(BoolObject) )
	{
		marshal(a_p, (bool)(dynamic_cast<const BoolObject*>(a_data))->value());
	}
	else if ( a_data->oIsKindOf(ShortObject) )
	{
		marshal(a_p, (Int16)(dynamic_cast<const ShortObject*>(a_data))->value());
	}
	else if ( a_data->oIsKindOf(IntegerObject) )
	{
		marshal(a_p, (Int32)(dynamic_cast<const IntegerObject*>(a_data))->value());
	}
	else if ( a_data->oIsKindOf(FloatObject) )
	{
		marshal(a_p, (float)(dynamic_cast<const FloatObject*>(a_data))->value());
	}
	else if ( a_data->oIsKindOf(DoubleObject) )
	{
		marshal(a_p, (double)(dynamic_cast<const DoubleObject*>(a_data))->value());
	}
	else if ( a_data->oIsKindOf(String) )
	{
		marshal(a_p, *(dynamic_cast<const String*>(a_data)));
	}
	else
	{
		throw InvalidOperationException(String("RMIBase::marshal(): can not marshal class: ")
											<< a_data->objClassName());
	}
}

Object* RMIBase::unmarshal(Byte*& a_p, Byte* a_end) const throw(ParseException)
{
	UInt32 datatype;
	unpack(a_p, a_end, datatype);

	Object* obj = 0;

	switch(datatype)
	{
		case DataType_INT8:
		case DataType_UINT8:
			{
				Byte d;
				unpack(a_p, a_end, d);
				obj = new ByteObject(d);
			}
			break;

		case DataType_INT16:
		case DataType_UINT16:
			{
				UInt16 d;
				unpack(a_p, a_end, d);
				obj = new ShortObject((Int16)d);
			}
			break;

		case DataType_INT32:
		case DataType_UINT32:
			{
				UInt32 d;
				unpack(a_p, a_end, d);
				obj = new IntegerObject((Int32)d);
			}
			break;

		case DataType_STRING:
			{
				String s;
				unpack(a_p, a_end, s);
				obj = new String(s);
			}
			break;

		case DataType_FLOAT:
			{
				UInt32 d;
				unpack(a_p, a_end, d);
				obj = new FloatObject(*(float*)d);
			}
			break;

		case DataType_DOUBLE:
			{
				UInt32 d1, d2;
				unpack(a_p, a_end, d1);
				unpack(a_p, a_end, d2);
				double d;
				((UInt32*)&d)[0] = d1;
				((UInt32*)&d)[1] = d2;
				obj = new DoubleObject(d);
			}
			break;

		case DataType_BOOL:
			{
				Byte d;
				unpack(a_p, a_end, d);
				obj = new BoolObject((d ? true : false));
			}
			break;

		case DataType_VOID:
			break;

		default:
			throw ParseException("RMIBase::unmarshal(): unknown data type");
	}

	return obj;
}

int RMIBase::marshalSizeOf(const Object* a_data) const throw()
{
	if ( !a_data )
	{
		return 4;
	}
	if ( a_data->oIsKindOf(ByteObject) )
	{
		return (4 + 1);
	}
	else if ( a_data->oIsKindOf(BoolObject) )
	{
		return (4 + 1);
	}
	else if ( a_data->oIsKindOf(ShortObject) )
	{
		return (4 + 2);
	}
	else if ( a_data->oIsKindOf(IntegerObject) )
	{
		return (4 + 4);
	}
	else if ( a_data->oIsKindOf(FloatObject) )
	{
		return (4 + 4);
	}
	else if ( a_data->oIsKindOf(DoubleObject) )
	{
		return (4 + 8);
	}
	else if ( a_data->oIsKindOf(String) )
	{
		return (4 + 4 + (dynamic_cast<const String*>(a_data))->length());
	}
	else
	{
		return 0;
	}
}


void RMIBase::icheckSocket() throw(ConnectException)
{
	if ( !c_socket )
		throw ConnectException("no connection available");
}


Byte* RMIBase::setupPacket(ByteArray& a_packet, RMIBase::Cmd a_cmd, int a_datasize, Byte** a_pend) throw()
{
	UInt32 datasize = 4		// Protokoll_VERSION
					+ 4		// Cmd_*
					+ a_datasize
					+ 4;	// 'RMIE'

	UInt32 pkglen = 4		// magic 'RMIB'
				  + 4		// datasize
				  + datasize;

	a_packet.setSize(pkglen, pkglen);

	Byte* p = (Byte*)a_packet.data();
	Byte* pend = p + pkglen;

	// magic
	c_magic_beg.copy(p, 4);
	p += 4;
	// data size
	pack(p, (UInt32)datasize);
	// Protokoll_VERSION
	pack(p, (UInt32)Protokoll_VERSION);
	// Cmd_*
	pack(p, (UInt32)a_cmd);

	c_magic_end.copy(pend - 4, 4);

	if ( a_pend )
		*a_pend = pend;

	return p;
}

void RMIBase::sendPacket(ByteArray& a_packet) throw(Exception)
{
	icheckSocket();

	c_socket->write((const char*)a_packet.data(),
					a_packet.size(),
					Socket::WriteFlag_FORCECOMPLETESIZE);
}


Socket* RMIBase::getSocket() const throw()
{
	return c_socket;
}

void RMIBase::setSocket(Socket* a_socket) throw()
{
	c_socket = a_socket;
}


}} // namespace
