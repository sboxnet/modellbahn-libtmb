/***************************************************************************
 *   Copyright (C) 2009
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


#include <tmb/io/GpioSer.h>

namespace tmb {

enum { NUM_BITS = 14,
	   NUM_BYTES = 2,
	   AVRPROGCHUNK_MAX = 32,
	   AVRSCLKPERIOD_MIN = 10, // usec
	   AVRSCLKFREQ_MAX   = 100000, // Hz
	   AVRSCLKFREQ_MIN	 = AVRSCLKFREQ_MAX / 256
};

static inline uint Min(uint a, uint b) {
	return (a < b ? a : b);
}

String GpioSer::AvrException::getErrnoStr() const {
	switch(_errno) {
		case GpioSer::RC_OK:			return String("RC_OK");
		case GpioSer::RC_POLL_TIMEOUT:	return String("RC_POLL_TIMEOUT");
		case GpioSer::RC_NEXT:			return String("RC_NEXT");
		case GpioSer::RC_NOSYNC:		return String("RC_NOSYNC");
		case GpioSer::RC_INVALID:		return String("RC_INVALID");
		default:						return String("unknown retcode");
	}
}

String GpioSer::AvrException::msgErrno() const {
	String e = getErrnoStr();
	String s(msg().length() + e.length() + 10);
	s << msg() << ": " << e;
	return s;
}



GpioSer::GpioSer(const String& a_ttydev) {
	init(a_ttydev, -1);
}
GpioSer::GpioSer(const String& a_ttydev, int a_timeout) {
	init(a_ttydev, a_timeout);
}

GpioSer::~GpioSer() {
	delete _rs232;
}

void GpioSer::init(const String& a_ttydev, int a_timeout) {
	_rs232 = new RS232Device(RS232Device::Flag_DATABITS_8
							|RS232Device::Flag_EVEN_PARITY
							|RS232Device::Flag_1_STOPBITS
							|RS232Device::Flag_BAUD_38400
							|RS232Device::Flag_OPEN_RDWR, a_ttydev);
	_num_bits = NUM_BITS;
	_version = 0;
	_recvbufsize = 1;
	
	setTimeout(a_timeout);
}

void GpioSer::setTimeout(int a_timeout) {
	if (a_timeout < 0)
		_timeout = 100;
	else
		_timeout = a_timeout;
}


void GpioSer::open() {
	_rs232->open();
	try {
		_rs232->setLowLatency();
	} catch(const Exception&) {
		_rs232->close();
		throw;
	}
	
	try {
		_version = cmdGetVersion();
	} catch(const TryAgainException&) {
		_version = 0;
	}
	if (_version == 0) {
		_rs232->close();
		throw Exception("no version returned by gpio-ser device");
	}
	
	try {
		_recvbufsize = cmdGetRecvBufSize();
		if (_recvbufsize == 0)
			_recvbufsize = 1;
	} catch(const Exception&) {
		_recvbufsize = 1;
	}
	
	cmdClrErrorFlags();
}

void GpioSer::close() {
	_rs232->close();
}

void GpioSer::rawWrite(byte* a_buf, int a_size) {
	_rs232->write(a_buf, a_size, _timeout);
}
void GpioSer::rawWrite(byte a_byte) {
	_rs232->write(a_byte, _timeout);
}
void GpioSer::rawRead(byte* a_buf, int a_size) {
	_rs232->read(a_buf, a_size, _timeout);
}
void GpioSer::rawRead(byte& a_byte) {
	_rs232->read(a_byte, _timeout);
}
int GpioSer::rawReadAvail() {
	return _rs232->bytesInReadQueue();
}


void GpioSer::cmdClrErrorFlags() {
	_rs232->write(CMD_CLR_ERROR_FLAGS, _timeout);
}

uint GpioSer::cmdGetErrorFlags() {
	_rs232->write(CMD_GET_ERROR_FLAGS, _timeout);
	byte b;
	_rs232->read(b, _timeout);
	return b;
}

uint GpioSer::cmdGetVersion() {
	_rs232->write(CMD_GET_VERSION, _timeout);
	byte b;
	_rs232->read(b, _timeout);
	return b;
}

String GpioSer::cmdGetVersionStr() {
	_rs232->write(CMD_GET_VERSION_STR, _timeout);
	String vs(256);
	int cnt = 0;
	for(;;) {
		byte b;
		_rs232->read(b, _timeout);
		if (b == 0)
			break;
		vs << (char)b;
		cnt++;
		if (cnt > 256)
			throw Exception("version string returned by gpio-ser device is too long");
	}
	return vs;
}

uint GpioSer::cmdGetFlags() {
	_rs232->write(CMD_GET_FLAGS, _timeout);
	byte b;
	_rs232->read(b, _timeout);
	return b;
}

uint GpioSer::cmdGetRecvBufSize() {
	_rs232->write(CMD_GET_RECVBUFSIZE, _timeout);
	byte b;
	_rs232->read(b, _timeout);
	return b;
}

void GpioSer::cmdSoftReset() {
	_rs232->write(CMD_SOFT_RESET, _timeout);
}


void GpioSer::cmdEnterAvrMode() {
	_rs232->write(CMD_ENTER_AVRMODE, _timeout);
	byte b;
	_rs232->read(b, _timeout);
	if (b != RC_OK)
		throw Exception("error on enter AVR mode");
}

void GpioSer::cmdLeaveAvrMode() {
	_rs232->write(CMD_LEAVE_AVRMODE, _timeout);
	byte b;
	_rs232->read(b, _timeout);
	if (b != RC_OK)
		throw Exception("error on leaving AVR mode");
}

uint GpioSer::calcAvrSclkPeriod(uint a_sclkfreq) {
	// SCLK frequency: max 100kHz
	uint sclk_period;
	if (a_sclkfreq == 0 || a_sclkfreq >= AVRSCLKFREQ_MAX) {
		sclk_period = 0; // == 10us   , see: gpio-ser command cmdAvrProgMode()
	} else {
		sclk_period = ((AVRSCLKFREQ_MAX + a_sclkfreq - 1) / a_sclkfreq) - 1; // round up to next higher period or next lower frequency: 50kHz, 33kHz, 25kHz, ...
		if (sclk_period > 255)
			sclk_period = 255;	// max: 2.55ms or 390.6Hz
	}
	return sclk_period;
}

uint GpioSer::getAvrSclkFreqFrom(uint a_sclkfreq) {
	return AVRSCLKFREQ_MAX / (calcAvrSclkPeriod(a_sclkfreq) + 1);
}

bool GpioSer::cmdAvrProgMode(bool a_on, AvrWait a_wait, uint a_sclkfreq) {
	byte sclk_period = (byte)calcAvrSclkPeriod(a_sclkfreq);
	byte flags = 0;
	if (a_on) {
		flags |= FLG_AVRPROGMODE;
		flags |= (a_wait & AVRWAIT_MASK);
	}
	byte cmds[3] = { CMD_AVR_PROGMODE, flags, sclk_period };
	_rs232->write(cmds, 3, _timeout);
	byte b;
	_rs232->read(b, _timeout);
	if ((b & FLG_AVRPROGMODE) == (flags & FLG_AVRPROGMODE))
		return true;
	return false;
}

uint GpioSer::getAvrMaxSclkFreq() {
	return AVRSCLKFREQ_MAX;
}
uint GpioSer::getAvrMinSclkFreq() {
	return AVRSCLKFREQ_MIN;
}

uint32 GpioSer::cmdAvrRawCmd(uint32 a_spi_cmd) {
	byte cmds[5] = { CMD_AVR_RAWCMD, (byte)(a_spi_cmd >> 24), (byte)(a_spi_cmd >> 16), (byte)(a_spi_cmd >> 8), (byte)a_spi_cmd };
	_rs232->write(cmds, 5, _timeout);
	byte r[3];
	_rs232->read(r, 3, _timeout);
	return ((uint32)r[0] << 16)|((uint32)r[1] << 8)|r[2];
}

static inline void AVR_RC(byte rc) {
	switch(rc) {
		case GpioSer::RC_OK:			break;
		case GpioSer::RC_POLL_TIMEOUT:	throw GpioSer::AvrException("AVR poll timeout", rc);
		case GpioSer::RC_NOSYNC:		throw GpioSer::AvrException("AVR no sync, command failure", rc);
		case GpioSer::RC_NEXT:			throw GpioSer::AvrException("AVR RC_NEXT as retcode", rc);
		case GpioSer::RC_INVALID:		throw GpioSer::AvrException("AVR invalid parameter", rc);
		default:						throw GpioSer::AvrException("AVR unknown retcode", rc);
	}
}

bool GpioSer::cmdAvrProgramEnable() {
	_rs232->write(CMD_AVR_PROGRAMENABLE, _timeout);
	byte b;
	_rs232->read(b, _timeout);
	switch(b) {
		case GpioSer::RC_OK:		return true;
		case GpioSer::RC_NOSYNC:	return false;
		
		default:					AVR_RC(b);
									return false;
	}
}

void GpioSer::cmdAvrReadProgMem(uint a_addr, uint a_size, Array<uint16>& a_mem, IProgress* a_pg) {
	if (a_addr >= 0x10000 || a_size > 0x10000 || a_addr + a_size > 0x10000)
		throw AvrException("GpioSer::cmdAvrReadProgMem(): invalid parameter: address or size beyond 0x10000", RC_INVALID);
	a_mem.setSize(a_size);
	if (a_size == 0)
		return;
	byte cmds[5] = { CMD_AVR_READ_PRGMEM, (byte)(a_addr >> 8), (byte)a_addr, (byte)(a_size >> 8), (byte)a_size };
	_rs232->write(cmds, 5, _timeout);
	for (uint i = 0; i < a_size; i++) {
		byte buf[2];
		_rs232->read(buf, 2, _timeout);
		uint16 w = ((uint16)buf[1] << 8)|buf[0];
		if (a_pg)
			a_pg->next(a_addr+i, w);
		a_mem[i] = w;
	}
	byte rc;
	_rs232->read(rc, _timeout);
	AVR_RC(rc);
}

void GpioSer::cmdAvrReadEeprom(uint a_addr, uint a_size, Array<byte>& a_mem, IProgress* a_pg) {
	if (a_addr >= 0x10000 || a_size > 0x10000 || a_addr + a_size > 0x10000)
		throw AvrException("GpioSer::cmdAvrReadEeprom(): invalid parameter: address or size beyond 0x10000", RC_INVALID);
	a_mem.setSize(a_size);
	if (a_size == 0)
		return;
	byte cmds[5] = { CMD_AVR_READ_EEPROM, (byte)(a_addr >> 8), (byte)a_addr, (byte)(a_size >> 8), (byte)a_size };
	_rs232->write(cmds, 5, _timeout);
	for (uint i = 0; i < a_size; i++) {
		byte b;
		_rs232->read(b, _timeout);
		if (a_pg)
			a_pg->next(a_addr+i, b);
		a_mem[i] = b;
	}
	byte rc;
	_rs232->read(rc, _timeout);
	AVR_RC(rc);
}

void GpioSer::cmdAvrWriteProgMemPage(uint a_addr, uint a_pageexp, const Array<uint16>& a_mem, uint a_offs, IProgress* a_pg) {
	if (a_addr >= 0x10000)
		throw AvrException("GpioSer::cmdAvrWriteProgMemPage(): invalid parameter: address >= 0x10000", RC_INVALID);
	if (a_pageexp > 15)
		throw AvrException("GpioSer::cmdAvrWriteProgMemPage(): invalid parameter: page exponent > 15", RC_INVALID);
	uint pagesize = (1 << a_pageexp);
	if ((uint)a_mem.size() < a_offs + pagesize)
		throw AvrException("GpioSer::cmdAvrWriteProgMemPage(): invalid parameter: mem buffer size < buffer offset + flash page size", RC_INVALID);
	uint addr = a_addr & ~(pagesize - 1);
	byte cmds[4] = { CMD_AVR_WRITE_PRGMEM_PAGE, (byte)(addr >> 8), (byte)addr, (byte)a_pageexp };
	_rs232->write(cmds, 4, _timeout);
	uint bufsize = Min(_recvbufsize, AVRPROGCHUNK_MAX);
	for (uint i = 0, ri = 0; i < pagesize; ) {
		byte buf[bufsize];
		uint k = 0;
		while (k < bufsize && i < pagesize) {
			buf[k++] = (byte)a_mem[a_offs+i];			// low byte
			buf[k++] = (byte)(a_mem[a_offs+i] >> 8);	// high byte
			i++;
		}
		_rs232->write(buf, k, _timeout);
		
		k /= 2;
		_rs232->read(buf, k, _timeout);	// for each send word -> one return byte
		for (uint j = 0; j < k; j++) {
			if (a_pg) {
				a_pg->next(addr+ri, a_mem[a_offs+ri]);
				ri++;
			}
			if (i == pagesize && j == k-1) { // last return byte
				AVR_RC(buf[j]);
				break;
			}
			if (buf[j] != RC_NEXT)
				throw AvrException("GpioSer::cmdAvrWriteProgMemPage(): communication error: RC_NEXT expected", RC_NOSYNC);
		}
	}
}


void GpioSer::cmdAvrWriteEepromPage(uint a_addr, uint a_pageexp, const Array<byte>& a_mem, uint a_offs, IProgress* a_pg) {
	if (a_addr >= 0x10000)
		throw AvrException("GpioSer::cmdAvrWriteEepromPage(): invalid parameter: address >= 0x10000", RC_INVALID);
	if (a_pageexp > 15)
		throw AvrException("GpioSer::cmdAvrWriteEepromPage(): invalid parameter: page exponent > 15", RC_INVALID);
	uint pagesize = (1 << a_pageexp);
	if ((uint)a_mem.size() < a_offs + pagesize)
		throw AvrException("GpioSer::cmdAvrWriteEepromPage(): invalid parameter: mem buffer size < buffer offset + eeprom page size", RC_INVALID);
	uint addr = a_addr & ~(pagesize - 1);
	byte cmds[4] = { CMD_AVR_WRITE_EEPROM_PAGE, (byte)(addr >> 8), (byte)addr, (byte)a_pageexp };
	_rs232->write(cmds, 4, _timeout);
	uint bufsize = Min(_recvbufsize, AVRPROGCHUNK_MAX);
	for (uint i = 0, ri = 0; i < pagesize; ) {
		byte buf[bufsize];
		uint k = 0;
		while (k < bufsize && i < pagesize) {
			buf[k++] = a_mem[a_offs+i];
			i++;
		}
		_rs232->write(buf, k, _timeout);
		
		_rs232->read(buf, k, _timeout);	// for each send byte -> one return byte
		for (uint j = 0; j < k; j++) {
			if (a_pg) {
				a_pg->next(addr+ri, a_mem[a_offs+ri]);
				ri++;
			}
			if (i == pagesize && j == k-1) { // last return byte
				AVR_RC(buf[j]);
				break;
			}
			if (buf[j] != RC_NEXT)
				throw AvrException("GpioSer::cmdAvrWriteEepromPage(): communication error: RC_NEXT expected", RC_NOSYNC);
		}
	}
}

void GpioSer::cmdAvrWriteEepromByte(uint a_addr, byte a_data, IProgress* a_pg) {
	if (a_addr >= 0x10000)
		throw AvrException("GpioSer::cmdAvrWriteEepromByte(): invalid parameter: address >= 0x10000", RC_INVALID);
	byte cmds[4] = { CMD_AVR_WRITE_EEPROM_BYTE, (byte)(a_addr >> 8), (byte)a_addr, a_data };
	_rs232->write(cmds, 4, _timeout);
	byte rc;
	_rs232->read(rc, _timeout);
	if (a_pg)
		a_pg->next(a_addr, a_data);
	AVR_RC(rc);
}

void GpioSer::cmdAvrLoadExtAddrByte(byte a_extaddr) {
	byte cmds[2] = { CMD_AVR_LOAD_EXT_ADDR, a_extaddr };
	_rs232->write(cmds, 2, _timeout);
	byte rc;
	_rs232->read(rc, _timeout);
	AVR_RC(rc);
}

void GpioSer::cmdAvrChipErase() {
	_rs232->write(CMD_AVR_CHIP_ERASE, _timeout);
	byte rc;
	_rs232->read(rc, _timeout);
	AVR_RC(rc);
}

uint32 GpioSer::cmdAvrReadSignature() {
	_rs232->write(CMD_AVR_READ_SIGNATURE, _timeout);
	byte buf[4];
	_rs232->read(buf, 4, _timeout);
	uint32 sig = ((uint32)buf[0] << 16)|((uint32)buf[1] << 8)|((uint32)buf[2]);
	AVR_RC(buf[3]);
	return sig;
}

byte GpioSer::cmdAvrReadLockBits() {
	_rs232->write(CMD_AVR_READ_LOCKBITS, _timeout);
	byte buf[2];
	_rs232->read(buf, 2, _timeout);
	AVR_RC(buf[1]);
	return buf[0];
}

void GpioSer::cmdAvrWriteLockBits(byte a_bits) {
	byte cmds[2] = { CMD_AVR_WRITE_LOCKBITS, a_bits };
	_rs232->write(cmds, 2, _timeout);
	byte rc;
	_rs232->read(rc, _timeout);
	AVR_RC(rc);
}

byte GpioSer::cmdAvrReadFuseBits(byte a_fusenr) {
	byte cmds[2] = { CMD_AVR_READ_FUSEBITS, a_fusenr };
	_rs232->write(cmds, 2, _timeout);
	byte buf[2];
	_rs232->read(buf, 2, _timeout);
	AVR_RC(buf[1]);
	return buf[0];
}

void GpioSer::cmdAvrWriteFuseBits(byte a_fusenr, byte a_fuse) {
	byte cmds[3] = { CMD_AVR_WRITE_FUSEBITS, a_fusenr, a_fuse };
	_rs232->write(cmds, 3, _timeout);
	byte rc;
	_rs232->read(rc, _timeout);
	AVR_RC(rc);
}

byte GpioSer::cmdAvrReadCalibrationByte(byte a_addr) {
	byte cmds[2] = { CMD_AVR_READ_CALIBR, a_addr };
	_rs232->write(cmds, 2, _timeout);
	byte buf[2];
	_rs232->read(buf, 2, _timeout);
	AVR_RC(buf[1]);
	return buf[0];
}



#define CHECK_BYTE_NR(_bnr) \
			if ((_bnr) >= NUM_BYTES) \
				throw Exception("byte number out of range");
	
#define CHECK_BIT_NR(_bnr) \
			if ((_bnr) >= _num_bits) \
				throw Exception("bit number out of range");


uint GpioSer::do_cmd_get(byte cmd) {
	_rs232->write(cmd, _timeout);
	byte vals[2];
	_rs232->read(vals, 2, _timeout);
	return ((uint)vals[0] << 8) | vals[1];
}

byte GpioSer::do_cmd_get_byte(uint bnr, byte cmds[]) {
	CHECK_BYTE_NR(bnr);
	_rs232->write(cmds[bnr], _timeout);
	byte b;
	_rs232->read(b, _timeout);
	return b;
}

uint GpioSer::cmdGetPin() {
	return do_cmd_get(CMD_GET_PIN);
}

byte GpioSer::cmdGetPinByte(uint bnr) {
	byte cmds[2] = { CMD_GET_PIN_0, CMD_GET_PIN_1 };
	return do_cmd_get_byte(bnr, cmds);
}

uint GpioSer::cmdGetOut() {
	return do_cmd_get(CMD_GET_PORT);
}

byte GpioSer::cmdGetOutByte(uint bnr) {
	byte cmds[2] = { CMD_GET_PORT_0, CMD_GET_PORT_1 };
	return do_cmd_get_byte(bnr, cmds);
}

uint GpioSer::cmdGetDDR() {
	return do_cmd_get(CMD_GET_DDR);
}

byte GpioSer::cmdGetDDRByte(uint bnr) {
	CHECK_BYTE_NR(bnr);
	uint v = do_cmd_get(CMD_GET_DDR);
	return (v >> (8*bnr));
}


void GpioSer::cmdPutOut(uint v) {
	byte cmds[3] = { CMD_PUT_PORT, (byte)(v >> 8), (byte)v };
	_rs232->write(cmds, 3, _timeout);
}

void GpioSer::cmdPutOutByte(uint bnr, byte v) {
	CHECK_BYTE_NR(bnr);
	byte cmds[2] = { (bnr == 0 ? CMD_PUT_PORT_0 : CMD_PUT_PORT_1), v };
	_rs232->write(cmds, 2, _timeout);
}

void GpioSer::cmdPutOut4Bits(uint hnr, byte v) {
	static const byte cmddef[4] = { CMD_PUT4_0, CMD_PUT4_1, CMD_PUT4_2, CMD_PUT4_3 };
	if (hnr >= 2*NUM_BYTES)
		throw Exception("half byte number out of range");
	_rs232->write(cmddef[hnr] | (v & 0x0f), _timeout);
}

void GpioSer::cmdPutDDR(uint v) {
	byte cmds[3] = { CMD_PUT_DDR, (byte)(v >> 8), (byte)v };
	_rs232->write(cmds, 3, _timeout);
}



uint GpioSer::cmdGetPinBit(uint b) {
	CHECK_BIT_NR(b);
	return (cmdGetPinByte(b >> 3) & (1 << (b & 0x07))) ? 1 : 0;
}

uint GpioSer::cmdGetOutBit(uint b) {
	CHECK_BIT_NR(b);
	return (cmdGetOutByte(b >> 3) & (1 << (b & 0x07))) ? 1 : 0;
}

uint GpioSer::cmdGetDDRBit(uint b) {
	CHECK_BIT_NR(b);
	return (cmdGetDDRByte(b >> 3) & (1 << (b & 0x07))) ? 1 : 0;
}


void GpioSer::cmdPutOutBit(uint b, uint v) {
	if (v)
		cmdSetOutBit(b);
	else
		cmdClrOutBit(b);
}

void GpioSer::cmdPutDDRBit(uint b, uint v) {
	if (v)
		cmdSetDDRBit(b);
	else
		cmdClrDDRBit(b);
}

void GpioSer::cmdSetOut(uint v) {
	if ((v & 0x0fff0) == 0 && (v & 0x000f)) {
		_rs232->write(CMD_SETB4_0|(v & 0x000f), _timeout);
		
	} else if ((v & 0x0ff0f) == 0 && (v & 0x00f0)) {
		_rs232->write(CMD_SETB4_1|((v>>4) & 0x000f), _timeout);
		
	} else if ((v & 0x0f0ff) == 0 && (v & 0x0f00)) {
		_rs232->write(CMD_SETB4_2|((v>>8) & 0x000f), _timeout);
		
	} else if ((v & 0x00fff) == 0 && (v & 0xf000)) {
		_rs232->write(CMD_SETB4_3|((v>>12) & 0x000f), _timeout);
	
	} else if (v & 0x0ffff) {
		byte cmds[3] = { CMD_SETB_PORT, (byte)(v >> 8), (byte)v };
		_rs232->write(cmds, 3, _timeout);
	}
}

void GpioSer::cmdClrOut(uint v) {
	if ((v & 0x0fff0) == 0 && (v & 0x000f)) {
		_rs232->write(CMD_CLRB4_0|(v & 0x000f), _timeout);
		
	} else if ((v & 0x0ff0f) == 0 && (v & 0x00f0)) {
		_rs232->write(CMD_CLRB4_1|((v>>4) & 0x000f), _timeout);
		
	} else if ((v & 0x0f0ff) == 0 && (v & 0x0f00)) {
		_rs232->write(CMD_CLRB4_2|((v>>8) & 0x000f), _timeout);
		
	} else if ((v & 0x00fff) == 0 && (v & 0xf000)) {
		_rs232->write(CMD_CLRB4_3|((v>>12) & 0x000f), _timeout);
	
	} else if (v & 0x0ffff) {
		byte cmds[3] = { CMD_CLRB_PORT, (byte)(v >> 8), (byte)v };
		_rs232->write(cmds, 3, _timeout);
	}
}

void GpioSer::cmdSetOutBit(uint b) {
	CHECK_BIT_NR(b);
	cmdSetOut(1 << b);
}

void GpioSer::cmdClrOutBit(uint b) {
	CHECK_BIT_NR(b);
	cmdClrOut(1 << b);
}

void GpioSer::cmdSetDDR(uint v) {
	byte cmds[3] = { CMD_SETB_DDR, (byte)(v >> 8), (byte)v };
	_rs232->write(cmds, 3, _timeout);
}

void GpioSer::cmdClrDDR(uint v) {
	byte cmds[3] = { CMD_CLRB_DDR, (byte)(v >> 8), (byte)v };
	_rs232->write(cmds, 3, _timeout);
}

void GpioSer::cmdSetDDRBit(uint b) {
	CHECK_BIT_NR(b);
	cmdSetDDR(1 << b);
}

void GpioSer::cmdClrDDRBit(uint b) {
	CHECK_BIT_NR(b);
	cmdClrDDR(1 << b);
}

byte GpioSer::cmdReadIoReg(byte a_addr) {
	byte cmds[2] = { CMD_READ_IO_REG, a_addr };
	_rs232->write(cmds, 2, _timeout);
	byte b;
	_rs232->read(b, _timeout);
	return b;
}

void GpioSer::cmdPutIoReg(byte a_addr, byte a_data, byte a_mask) {
	byte cmds[4] = { CMD_PUT_IO_REG, a_addr, a_data, a_mask };
	_rs232->write(cmds, 4, _timeout);
}


} // namespace
