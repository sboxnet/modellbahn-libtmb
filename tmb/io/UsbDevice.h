/***************************************************************************
 *   Copyright (C) 2013
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


#ifndef _TMB_IO_USBDEVICE_H_
#define _TMB_IO_USBDEVICE_H_


#include <tmb/io/IOException.h>
#include <tmb/lang/String.h>
#include <tmb/lang/Array.h>

namespace tmb {

/**
 * class UsbDevice:
 * A USB Interface Class.
 */
class UsbDevice
{
	public:
		enum {
			REQTYPE_TYP_STANDARD   = (0<<5),
			REQTYPE_TYP_CLASS      = (1<<5),
			REQTYPE_TYP_VENDOR     = (2<<5),
			REQTYPE_REC_DEVICE     = 0,
			REQTYPE_REC_INTERFACE  = 1,
			REQTYPE_REC_ENDPOINT   = 2,
			REQTYPE_REC_OTHER      = 3,
			
			DEFAULT_TIMEOUT = 2000,
		};

	protected:
		void*	_usbctx;
		void*	_usbdevh;
	
	public:
		UsbDevice();
		virtual ~UsbDevice();
		
	private: UsbDevice(const UsbDevice&);
	private: void operator=(const UsbDevice&);


	public:
		// open the device
		virtual void open(uint16_t idvendor, uint16_t idproduct);
		// close the device
		virtual void close();
		
		bool isOpen();
		
		int controlTrans(uint8 reqtype, uint8 req, uint16 value, uint16 index,
						uint timeout = DEFAULT_TIMEOUT);
						
		int controlTransOut(uint8 reqtype, uint8 req, uint16 value, uint16 index,
							const Array<byte>& data, uint timeout = DEFAULT_TIMEOUT);
		int controlTransOut(uint8 reqtype, uint8 req, uint16 value, uint16 index,
							const byte* data, uint datasize, uint timeout = DEFAULT_TIMEOUT);
							
		int controlTransIn(uint8 reqtype, uint8 req, uint16 value, uint16 index,
						   Array<byte>& data, uint timeout = DEFAULT_TIMEOUT);
		int controlTransIn(uint8 reqtype, uint8 req, uint16 value, uint16 index,
						   byte* data, uint datasize, uint timeout = DEFAULT_TIMEOUT);
		
	protected:
		void checkDevice();

};


} // namespace


#endif
