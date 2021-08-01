/********************************************************************
 * cpp/rmi/RMIBase.h
 *
 * Copyright (C) 2004 IZB SOFT, Informatik Zentrum Bayern, GmbH
 * All rights reserved.
 *
 * @author  Thomas Maier, is00674
 * @date    2004
 *******************************************************************/





#ifndef _CPP_RMI_RMIBASE_H_
#define _CPP_RMI_RMIBASE_H_


#include <cpp/lang/Object.h>
#include <cpp/lang/String.h>
#include <cpp/lang/PointerArray.h>
#include <cpp/lang/ByteArray.h>
#include <cpp/net/ConnectException.h>
#include <cpp/net/Socket.h>

#include <cpp/rmi/RMIException.h>

#ifdef CPP_SYS_WINDOWS
# pragma warning ( disable : 4275 )
#endif


namespace cpp {
	namespace rmi {

using namespace cpp::lang;
using namespace cpp::net;

class CPP_LANG_DLLSPEC RMIBase : public Object
{
	CPP_OBJECT1(cpp::rmi, RMIBase, Object);

	public: typedef PointerArray<Object> InvokeArgs;

	public: enum { Protokoll_VERSION = 1 };

	protected: Socket*	c_socket;
	protected: String	c_magic_beg;
	protected: String	c_magic_end;

	protected: enum DataType {
						DataType_INT8	= 0,
						DataType_UINT8	= 1,
						DataType_INT16	= 2,
						DataType_UINT16	= 3,
						DataType_INT32	= 4,
						DataType_UINT32	= 5,
						DataType_STRING	= 6,
						DataType_FLOAT	= 7,
						DataType_DOUBLE	= 8,
						DataType_BOOL	= 9,
						DataType_VOID	= 10,
			   };

	protected: enum Cmd {
						Cmd_CREATEOBJECT	= 0,
						Cmd_RELEASEOBJECT	= 1,
						Cmd_INVOKE			= 2,
						Cmd_RETURN			= 3,
						Cmd_ERROR			= 4,
						Cmd_INVOKEEXCEPTION	= 5,
			   };

	protected: enum InvokeArgFlag {
						InvokeArgFlag_IN	= 0x0001,
						InvokeArgFlag_OUT	= 0x0002,
						InvokeArgFlag_INOUT	= 0x0003,
			   };

	public: typedef UInt32 RMIObjectID;

	protected: RMIBase(Socket* a_socket) throw();
	private: RMIBase(const RMIBase&);
	private: void operator=(const RMIBase&);
	public: virtual ~RMIBase() throw();

	
	protected: void pack(Byte*& a_p, Byte a_data) const throw();
	protected: void pack(Byte*& a_p, UInt16 a_data) const throw();
	protected: void pack(Byte*& a_p, UInt32 a_data) const throw();
	protected: void pack(Byte*& a_p, DataType a_dt) const throw() { pack(a_p, (UInt32)a_dt); }
	protected: void pack(Byte*& a_p, const String& a_data) const throw();

	protected: void unpack(Byte*& a_p, Byte* a_end, Byte& a_data) const throw(ParseException);
	protected: void unpack(Byte*& a_p, Byte* a_end, UInt16& a_data) const throw(ParseException);
	protected: void unpack(Byte*& a_p, Byte* a_end, UInt32& a_data) const throw(ParseException);
	protected: void unpack(Byte*& a_p, Byte* a_end, String& a_data) const throw(ParseException);

	protected: int packSizeOf(const String& a_str) const throw();

	protected: void marshal(Byte*& a_p, Byte a_data) const throw();
	protected: void marshal(Byte*& a_p, Int16 a_data) const throw();
	protected: void marshal(Byte*& a_p, UInt16 a_data) const throw();
	protected: void marshal(Byte*& a_p, Int32 a_data) const throw();
	protected: void marshal(Byte*& a_p, UInt32 a_data) const throw();
	protected: void marshal(Byte*& a_p, bool a_data) const throw();
	protected: void marshal(Byte*& a_p, float a_data) const throw();
	protected: void marshal(Byte*& a_p, double a_data) const throw();
	protected: void marshal(Byte*& a_p, const String& a_data) const throw();
	protected: void marshal(Byte*& a_p, const Object* a_data) const throw(InvalidOperationException);

	protected: Object* unmarshal(Byte*& a_p, Byte* a_end) const throw(ParseException);

	protected: int marshalSizeOf(const Object* a_data) const throw();


	protected: void icheckSocket() throw(cpp::net::ConnectException);

	protected: Byte* setupPacket(ByteArray& a_packet, RMIBase::Cmd a_cmd, int a_datasize, Byte** a_pend) throw();
	protected: void sendPacket(ByteArray& a_packet) throw(Exception);

	public: virtual Socket* getSocket() const throw();
	public: virtual void setSocket(Socket* a_socket) throw();

};



}} // namespace cpp::rmi


#ifdef CPP_SYS_WINDOWS
# pragma warning ( default : 4275 )
#endif

#endif // _CPP_RMI_RMIBASE_H_
