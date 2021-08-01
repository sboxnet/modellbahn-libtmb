/***************************************************************************
 *   Copyright (C) 2008
 *   by Thomas Maier <balagi@justmail.de>
 *
 *   Copyright: See COPYING file that comes with this distribution         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 ***************************************************************************/


#ifndef _TMB_IO_RS232DEVICE_H_
#define _TMB_IO_RS232DEVICE_H_


#include <tmb/io/IOException.h>
#include <tmb/lang/String.h>

namespace tmb {


/**
 * class RS232Device:
 * A RS232 Interface Class.
 */
class RS232Device
{
	public:
		enum {
			Flag_DATABITS_MASK	= 0x0007,
			Flag_DATABITS_5		= 0x0001,
			Flag_DATABITS_6		= 0x0002,
			Flag_DATABITS_7		= 0x0003,
			Flag_DATABITS_8		= 0x0000,
			
			Flag_1_STOPBITS		= 0x0000,
			Flag_2_STOPBITS		= 0x0008,
			Flag_NO_PARITY		= 0x0000,
			Flag_EVEN_PARITY	= 0x0010,
			Flag_ODD_PARITY		= 0x0020,
			
			Flag_RTSCTS			= 0x0040,	// use RTS and CTS signals?
			
			Flag_BAUD_MASK		= 0x0f00,
			Flag_BAUD_1200		= 0x0100,
			Flag_BAUD_2400		= 0x0200,
			Flag_BAUD_4800		= 0x0300,
			Flag_BAUD_9600		= 0x0000,
			Flag_BAUD_19200		= 0x0400,
			Flag_BAUD_38400		= 0x0500,
			
			Flag_OPEN_RDONLY	= 0x1000,	// open() modes for serial (tty) device /dev/ttyS*
			Flag_OPEN_WRONLY	= 0x2000,
			Flag_OPEN_RDWR		= 0x0000,
			Flag_OPEN_MASK		= 0x3000,
		};
		
		enum {	// wait modes for  write() and read() methods
			NO_WAIT		= 0,			// no wait
			INF_WAIT	= 0xffffffff,	// infinite wait
		};

	protected:
		Flags	_flags;
		String	_devfile;
		int		_fd;
	
	public:
		/*
		 * Constructor:
		 * @param a_flags		Flag_*
		 * @param a_devfile		the name of a /dev/ttyS* file
		 */
		RS232Device(uint a_flags, const String& a_devfile);
		virtual ~RS232Device();
		
	private: RS232Device(const RS232Device&);
	private: void operator=(const RS232Device&);


	public:
		// open the serial device, in non blocking mode (O_NDELAY)
		void open();
		// close the serial device
		void close();
		// do ioctl() on the serial device
		void ioctl(int a_cmd, void* a_data);
		void ioctl(int a_cmd, int a_data);
		
		// set low latency flag
		void setLowLatency();
		
		/* write and read to/from the serial device
		 * @param a_timeout_ms	timeout in ms or the values NO_WAIT or INF_WAIT
		 * @return number of written or read bytes. Always equal to a_size argument!
		 * @throws TryAgainException if timeout expires and no data could be written/read
		 */
		int write(byte* a_buf, int a_size, ulong a_timeout_ms = INF_WAIT);
		int write(byte a_byte, ulong a_timeout_ms = INF_WAIT);
		int read(byte* a_buf, int a_size, ulong a_timeout_ms = INF_WAIT);
		int read(byte& a_byte, ulong a_timeout_ms = INF_WAIT);
		
		// return the device file descriptor. if the device is not open, return -1 .
		int fd() const { return _fd; }
		
		// number of bytes in input buffer
		int bytesInReadQueue();
		// number of bytes in output buffer
		int bytesInWriteQueue();
		
		// drain and wait till output buffer is emtpy
		void drainWriteQueue();
		// flush (discards) input and output buffer
		void flush();
		void flushRead();
		void flushWrite();
		
		uint flags() const { return _flags; }
		void setFlags(uint f) { _flags = f; }
		const String& devFile() const { return _devfile; }
};


} // namespace


#endif
