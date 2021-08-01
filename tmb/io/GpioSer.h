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


#ifndef _TMB_IO_GPIOSER_H_
#define _TMB_IO_GPIOSER_H_


#include <tmb/io/IOException.h>
#include <tmb/lang/String.h>
#include <tmb/lang/Array.h>
#include <tmb/io/RS232Device.h>


namespace tmb {


/**
 * class GpioSer
 */
class GpioSer
{
	public:
		enum FLAGS {
			FLG_AVRMODE		= 0x0001,
			FLG_AVRPROGMODE	= 0x0002,
			FLG_AVRWAIT0	= 0x0004,
			FLG_AVRWAIT1	= 0x0008,
		};
		enum AvrWait {
			AVRWAIT_MASK	= 0x0c,
			AVRWAIT_POLL	= 0x00,
			AVRWAIT_8MS		= 0x04,
			AVRWAIT_16MS	= 0x08,
			AVRWAIT_64MS	= 0x0c,
		};
		enum ERRORFLAGS {
			ERRFLG_AVRCMD		= (1<<7),
			ERRFLG_FRAMEERR		= (1<<4),
			ERRFLG_DATAOV		= (1<<3),
			ERRFLG_PARITYERR	= (1<<2),
			ERRFLG_UNKNOWNCMD	= (1<<0),
		};
		enum CMDS {
			CMD_CLR_ERROR_FLAGS = 0x00,
			CMD_GET_ERROR_FLAGS	= 0x01,
			CMD_GET_VERSION		= 0x02,
			CMD_GET_VERSION_STR	= 0x03,
			CMD_GET_FLAGS		= 0x04,
			CMD_GET_RECVBUFSIZE	= 0x05,
			CMD_ENTER_AVRMODE	= 0x08,
			CMD_LEAVE_AVRMODE	= 0x09,
			CMD_READ_IO_REG		= 0x0a,
			CMD_PUT_IO_REG		= 0x0b,
			CMD_SOFT_RESET		= 0x0e,
			
			CMD_AVR_PROGMODE	= 0x10,
			CMD_AVR_RAWCMD		= 0x11,
			CMD_AVR_READ_PRGMEM	= 0x12,
			CMD_AVR_READ_EEPROM	= 0x13,
			CMD_AVR_WRITE_PRGMEM_PAGE	= 0x14,
			CMD_AVR_WRITE_EEPROM_PAGE	= 0x15,
			CMD_AVR_WRITE_EEPROM_BYTE	= 0x16,
			CMD_AVR_LOAD_EXT_ADDR	= 0x17,
			CMD_AVR_CHIP_ERASE		= 0x18,
			CMD_AVR_PROGRAMENABLE	= 0x19,
			CMD_AVR_READ_SIGNATURE	= 0x1a,
			CMD_AVR_READ_LOCKBITS	= 0x1b,
			CMD_AVR_WRITE_LOCKBITS	= 0x1c,
			CMD_AVR_READ_FUSEBITS	= 0x1d,
			CMD_AVR_WRITE_FUSEBITS	= 0x1e,
			CMD_AVR_READ_CALIBR		= 0x1f,
			
			CMD_GET_PIN_0		= 0x30,
			CMD_GET_PIN_1		= 0x31,
			CMD_GET_PIN			= 0x32,		
			CMD_GET_PORT_0		= 0x33,
			CMD_GET_PORT_1		= 0x34,
			CMD_GET_PORT		= 0x35,
			CMD_GET_DDR			= 0x36,
			
			CMD_PUT_PORT_0		= 0x38,
			CMD_PUT_PORT_1		= 0x39,
			CMD_PUT_PORT		= 0x3a,
			CMD_PUT_DDR			= 0x3b,
			CMD_SETB_PORT		= 0x3c,
			CMD_CLRB_PORT		= 0x3d,
			CMD_SETB_DDR		= 0x3e,	
			CMD_CLRB_DDR		= 0x3f,
			
			CMD_PUT4_0			= 0x40,
			CMD_PUT4_1			= 0x50,
			CMD_PUT4_2			= 0x60,
			CMD_PUT4_3			= 0x70,
			CMD_SETB4_0			= 0x80,
			CMD_SETB4_1			= 0x90,
			CMD_SETB4_2			= 0xa0,
			CMD_SETB4_3			= 0xb0,
			CMD_CLRB4_0			= 0xc0,
			CMD_CLRB4_1			= 0xd0,
			CMD_CLRB4_2			= 0xe0,
			CMD_CLRB4_3			= 0xf0,
		};
		enum RC {
			RC_OK		= 0x5a,
			RC_POLL_TIMEOUT	= 0x01,
			RC_NEXT		= 0x02,
			RC_NOSYNC	= 0x04,
			RC_INVALID	= 0x08,
		};

	
		class AvrException : public ErrorException {
			public: explicit AvrException(const String& a_msg, int a_errno) : ErrorException(a_msg, a_errno) {}
			public: AvrException(const AvrException& a_ex) : ErrorException(a_ex) {}
			public: virtual const AvrException& operator=(const AvrException& a_ex) { ErrorException::operator=(a_ex); return *this; }
			public: virtual ~AvrException() {}
			public: String getErrnoStr() const;
			public: String msgErrno() const;
		};
		
		class IProgress {
			public: IProgress() {}
			public: virtual ~IProgress() {}
			public: virtual void next(uint a_addr, uint a_value) = 0;
		};
	
	protected:
		RS232Device*	_rs232;
		uint			_num_bits;
		ulong			_timeout;
		uint			_version;
		uint			_recvbufsize;
	
	public:
		GpioSer(const String& a_ttydev);
		GpioSer(const String& a_ttydev, int a_timeout);
		virtual ~GpioSer();
		
	private: GpioSer(const GpioSer&);
	private: void operator=(const GpioSer&);
	
	private: void init(const String& a_ttydev, int a_timeout);
	
	public:
		int getNumBits() { return _num_bits; }
		uint getVersion() { return _version; }
		uint getTimeout() { return _timeout; }
		void setTimeout(int a_timeout);
		
		void open();
		void close();
		
		void rawWrite(byte* a_buf, int a_size);
		void rawWrite(byte a_byte);
		void rawRead(byte* a_buf, int a_size);
		void rawRead(byte& a_byte);
		int rawReadAvail();
	
		void cmdClrErrorFlags();
		uint cmdGetErrorFlags();
		uint cmdGetVersion();
		String cmdGetVersionStr();
		uint cmdGetFlags();
		uint cmdGetRecvBufSize();
		void cmdSoftReset();
		
		void cmdEnterAvrMode();
		void cmdLeaveAvrMode();
		// FLG_B_AVRPROGMODE:	if set -> programming mode on
		// FLG_B_AVRWAITx:		programming wait mode
		// sclk-period:		serial clock period = (sclk-period + 1) * 10us													
		bool cmdAvrProgMode(bool a_on, AvrWait a_wait, uint a_sclkfreq = 0);
		uint getAvrMaxSclkFreq();
		uint getAvrMinSclkFreq();
		uint getAvrSclkFreqFrom(uint a_sclkfreq);
		uint32 cmdAvrRawCmd(uint32 a_spi_cmd);
		bool cmdAvrProgramEnable();
		void cmdAvrReadProgMem(uint a_addr, uint a_size, Array<uint16>& a_mem, IProgress* a_pg = 0);
		void cmdAvrReadEeprom(uint a_addr, uint a_size, Array<byte>& a_mem, IProgress* a_pg = 0);
		void cmdAvrWriteProgMemPage(uint a_addr, uint a_pageexp, const Array<uint16>& a_mem, uint a_offs = 0, IProgress* a_pg = 0);
		void cmdAvrWriteEepromPage(uint a_addr, uint a_pageexp, const Array<byte>& a_mem, uint a_offs = 0, IProgress* a_pg = 0);
		void cmdAvrWriteEepromByte(uint a_addr, byte a_data, IProgress* a_pg = 0);
		void cmdAvrLoadExtAddrByte(byte a_extaddr);
		void cmdAvrChipErase();
		uint32 cmdAvrReadSignature();
		byte cmdAvrReadLockBits();
		void cmdAvrWriteLockBits(byte a_bits);
		byte cmdAvrReadFuseBits(byte a_fusenr);
		void cmdAvrWriteFuseBits(byte a_fusenr, byte a_fuse);
		byte cmdAvrReadCalibrationByte(byte a_addr);
		
		
		uint cmdGetPin();
		byte cmdGetPinByte(uint bnr);
		uint cmdGetOut();
		byte cmdGetOutByte(uint bnr);
		uint cmdGetDDR();
		byte cmdGetDDRByte(uint bnr);
		
		void cmdPutOut(uint v);
		void cmdPutOutByte(uint bnr, byte v);
		void cmdPutOut4Bits(uint hnr, byte v);
		void cmdPutDDR(uint v);
		
		uint cmdGetPinBit(uint b);
		uint cmdGetOutBit(uint b);
		uint cmdGetDDRBit(uint b);
		
		void cmdPutOutBit(uint b, uint v);
		void cmdPutDDRBit(uint b, uint v);
		
		void cmdSetOut(uint v);
		void cmdClrOut(uint v);
		void cmdSetOutBit(uint b);
		void cmdClrOutBit(uint b);
		
		void cmdSetDDR(uint v);
		void cmdClrDDR(uint v);
		void cmdSetDDRBit(uint b);
		void cmdClrDDRBit(uint b);
		
		byte cmdReadIoReg(byte addr);
		void cmdPutIoReg(byte addr, byte data, byte mask = 0xff);

	private:
		uint do_cmd_get(byte cmd);
		byte do_cmd_get_byte(uint bnr, byte cmd[]);
		uint calcAvrSclkPeriod(uint a_sclkfreq);
};

} // namespace

#endif //_TMB_IO_GPIOSER_H_
