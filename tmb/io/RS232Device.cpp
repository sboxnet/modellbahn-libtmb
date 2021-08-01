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

#include <tmb/io/RS232Device.h>
#include <tmb/io/Select.h>

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>

#define USE_LINUX_SERIAL
#ifdef USE_LINUX_SERIAL
#include <linux/serial.h>
#endif

namespace tmb {

RS232Device::RS232Device(uint a_flags, const String& a_devfile)
		: _flags(a_flags), _devfile(a_devfile), _fd(-1) {
}

RS232Device::~RS232Device() {
	try { close(); } catch(...) {}
}

void RS232Device::open() {
	struct termios tp;
	::cfmakeraw(&tp);
	tp.c_iflag &= ~(ISTRIP|INLCR|IGNCR|ICRNL|IUCLC|IXON|IXANY|IXOFF|IUTF8);
	tp.c_cflag &= ~(CSIZE|CSTOPB|PARODD|PARENB);
	tp.c_cflag |= (CREAD|CLOCAL);
	switch(_flags & Flag_DATABITS_MASK) {
		case Flag_DATABITS_5:	tp.c_cflag |= CS5; break;
		case Flag_DATABITS_6:	tp.c_cflag |= CS6; break;
		case Flag_DATABITS_7:	tp.c_cflag |= CS7; break;
		case Flag_DATABITS_8:	tp.c_cflag |= CS8; break;
		default: throw IOException("invalid databits flag", EINVAL);
	}
	switch(_flags & Flag_BAUD_MASK) {
		case Flag_BAUD_1200:	::cfsetspeed(&tp, B1200); break;
		case Flag_BAUD_2400:	::cfsetspeed(&tp, B2400); break;
		case Flag_BAUD_4800:	::cfsetspeed(&tp, B4800); break;
		case Flag_BAUD_9600:	::cfsetspeed(&tp, B9600); break;
		case Flag_BAUD_19200:	::cfsetspeed(&tp, B19200); break;
		case Flag_BAUD_38400:	::cfsetspeed(&tp, B38400); break;
		default: throw IOException("invalid baud rate", EINVAL);
	}
	if (_flags.has(Flag_2_STOPBITS))
		tp.c_cflag |= CSTOPB;
	if (_flags.has(Flag_EVEN_PARITY))
		tp.c_cflag |= PARENB;
	else if (_flags.has(Flag_ODD_PARITY))
		tp.c_cflag |= (PARENB|PARODD);
	if (_flags.has(Flag_RTSCTS))
		tp.c_cflag |= CRTSCTS;

	
	int openflags;
	switch(_flags & Flag_OPEN_MASK) {
		case Flag_OPEN_RDONLY:	openflags = O_RDONLY; break;
		case Flag_OPEN_WRONLY:	openflags = O_WRONLY; break;
		case Flag_OPEN_RDWR:	openflags = O_RDWR; break;
		default: throw IOException("invalid open flags", EINVAL);
	}
	
	if (_fd >= 0)
		throw IOException("device is already open");
	
	_fd = ::open(_devfile, openflags|O_NDELAY|O_NOCTTY);
	if (_fd == -1) {
		throw IOException("can not open device " + _devfile, errno);
	}	
	struct stat stats;
	// check if character device
	if (::fstat(_fd, &stats) != 0)
		throw IOException("unable to stat file " + _devfile, errno);
	int devmajor = (stats.st_rdev >> 8) & 0x0ff;
	int devminor = (stats.st_rdev & 0x0ff);
	if (!S_ISCHR(stats.st_mode) || devmajor != 4 || devminor < 64)
		throw IOException("this is not a serial line on device file " + _devfile);
	
	if (::ioctl(_fd, TIOCEXCL, 0) != 0)	// enter exclusive mode
		throw IOException("failed to set exclusive mode on " + _devfile, errno);
			
	if (::tcsetattr(_fd, TCSANOW, &tp) == -1) {
		int e = errno;
		::close(_fd);
		throw IOException("failed to set device attributes", e);
	}
	::tcflush(_fd, TCIOFLUSH);
}

void RS232Device::close() {
	if (_fd >= 0)
		::close(_fd);
	_fd = -1;
}

void RS232Device::ioctl(int a_cmd, void* a_data) {
	if (_fd < 0)
		throw IOException(EBADF);
	if (::ioctl(_fd, a_cmd, a_data) == -1)
		throw IOException("ioctl() failed on " + _devfile, errno);
}
void RS232Device::ioctl(int a_cmd, int a_data) {
	if (_fd < 0)
		throw IOException(EBADF);
	if (::ioctl(_fd, a_cmd, a_data) == -1)
		throw IOException("ioctl() failed on " + _devfile, errno);
}

void RS232Device::setLowLatency() {
#ifdef USE_LINUX_SERIAL
	struct serial_struct ser;
	ioctl(TIOCGSERIAL, &ser);
	ser.flags |= ASYNC_LOW_LATENCY;
	ioctl(TIOCSSERIAL, &ser);
#endif
}


int RS232Device::write(byte* a_buf, int a_size, ulong a_timeout_ms) {
	if (_fd < 0)
		throw IOException(EBADF);
	int written = 0;
	while (written < a_size) {
		if (a_timeout_ms != NO_WAIT) {
			Select sel;
			sel.addWR(_fd);
			if (a_timeout_ms == INF_WAIT)
				sel.select();
			else
				sel.select(0, a_timeout_ms * 1000);
		}
		int r = ::write(_fd, a_buf + written, a_size - written);
		if (r == -1) {
			if (errno == EAGAIN)
				throw TryAgainException("RS232Device::write(): try again, timeout");
			else
				throw IOException("write() failed", errno);
		}
		written += r;
	}
	return written;
}

int RS232Device::write(byte a_byte, ulong a_timeout_ms) {
	return write(&a_byte, 1, a_timeout_ms);
}

int RS232Device::read(byte* a_buf, int a_size, ulong a_timeout_ms) {
	if (_fd < 0)
		throw IOException(EBADF);
	int got = 0;
	while (got < a_size) {
		if (a_timeout_ms != NO_WAIT) {
			Select sel;
			sel.addRD(_fd);
			if (a_timeout_ms == INF_WAIT)
				sel.select();
			else
				sel.select(0, a_timeout_ms * 1000);
		}
		int r = ::read(_fd, a_buf + got, a_size - got);
		if (r == -1) {
			if (errno == EAGAIN)
				throw TryAgainException("RS232Device::read(): try again, timeout");
			else
				throw IOException("read() failed", errno);
		}
		got += r;
	}
	return got;
}

int RS232Device::read(byte& a_byte, ulong a_timeout_ms) {
	return read(&a_byte, 1, a_timeout_ms);
}

int RS232Device::bytesInReadQueue() {
	int n;
	ioctl(TIOCINQ, &n);
	return n;
}

int RS232Device::bytesInWriteQueue() {
	int n;
	ioctl(TIOCOUTQ, &n);
	return n;
}

void RS232Device::drainWriteQueue() {
	if (_fd < 0)
		throw IOException(EBADF);
	::tcdrain(_fd);
}

void RS232Device::flush() {
	ioctl(TCFLSH, TCIOFLUSH);
}
void RS232Device::flushRead() {
	ioctl(TCFLSH, TCIFLUSH);
}
void RS232Device::flushWrite(){
	ioctl(TCFLSH, TCOFLUSH);
}

}
