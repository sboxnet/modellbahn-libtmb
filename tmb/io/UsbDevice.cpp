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

#include <tmb/io/UsbDevice.h>
#include <tmb/lang/Array.h>

#include <libusb.h>
#include <iostream>

namespace tmb {

static inline void throwUsbError(int rc, const String& msg) {
	throw IOException(msg + ": " + libusb_error_name(rc), rc);
}



UsbDevice::UsbDevice()
		: _usbctx(NULL), _usbdevh(NULL) {
	int rc = libusb_init((libusb_context**)&_usbctx);
	if (rc != 0) {
		throwUsbError(rc, "libusb init failed");
	}
}

UsbDevice::~UsbDevice() {
	try { close(); } catch(...) {}
	if (_usbctx) {
		libusb_exit((libusb_context*)_usbctx);
	}
}

void UsbDevice::open(uint16_t idvendor, uint16_t idproduct) {
    int rc;
	
	close();
	
	libusb_device** devlist;
	rc = libusb_get_device_list((libusb_context*)_usbctx, &devlist);
    if (rc < 0) {
        throwUsbError(rc, "libusb_get_device_list failed");
    }
    int found = 0;
    libusb_device_handle* dh = NULL;
	libusb_device* dev = NULL;
    for (int i = 0, cnt = rc; i < cnt; i++) {
        libusb_device* d = devlist[i];
        libusb_device_descriptor desc;
		rc = libusb_get_device_descriptor(d, &desc);
        if (rc == 0) {
            if (desc.idVendor == idvendor && desc.idProduct == idproduct) {
				found++;
				if (!dev) {
					dev = d;
                }
			}
		}
	}
	if (found == 1) {
		rc = libusb_open(dev, &dh);
        if (rc != 0 || dh == NULL) {
			libusb_free_device_list(devlist, 1);
            const char* enam =libusb_error_name(rc);
            throwUsbError(rc, String("can not open usb device: ") + enam);
        }
    }
    libusb_free_device_list(devlist, 1);
	if (found > 1) {
		throw IOException("more then one USB device found");
	} else if (found == 0 || dh == NULL) {
		throw IOException("no USB device found");
	}
	
	_usbdevh = dh;
}

void UsbDevice::close() {
	if (_usbdevh) {
		libusb_close((libusb_device_handle*)_usbdevh);
		_usbdevh = NULL;
	}
}

bool UsbDevice::isOpen() {
	return _usbdevh != NULL;
}

int UsbDevice::controlTrans(uint8 reqtype, uint8 req, uint16 value, uint16 index, uint timeout) {
	int rc;
	
	checkDevice();
	
	rc = libusb_control_transfer((libusb_device_handle*)_usbdevh, reqtype & 0x7f,
								 req, value, index, NULL, 0, timeout);
	if (rc < 0) {
		throwUsbError(rc, "USB control transfer error");
	}
	return rc;
}

int UsbDevice::controlTransOut(uint8 reqtype, uint8 req, uint16 value, uint16 index, const Array<byte>& data,
							   uint timeout) {
	return controlTransOut(reqtype, req, value, index, data.data(), data.size(), timeout);
}

int UsbDevice::controlTransOut(uint8 reqtype, uint8 req, uint16 value, uint16 index, const byte* data,
							   uint datasize, uint timeout) {
	int rc;
	
	checkDevice();
	
	rc = libusb_control_transfer((libusb_device_handle*)_usbdevh, reqtype & 0x7f,
								 req, value, index, (byte*)data, datasize, timeout);
	if (rc < 0) {
		throwUsbError(rc, "USB control transfer error");
	}
	return rc;
}

int UsbDevice::controlTransIn(uint8 reqtype, uint8 req, uint16 value, uint16 index, Array<byte>& data,
							  uint timeout) {
	return controlTransIn(reqtype, req, value, index, data.data(), data.size(), timeout);
}

int UsbDevice::controlTransIn(uint8 reqtype, uint8 req, uint16 value, uint16 index, byte* data, uint datasize,
							  uint timeout) {
	int rc;
	
	checkDevice();
	
	rc = libusb_control_transfer((libusb_device_handle*)_usbdevh, reqtype | 0x80,
								 req, value, index, data, datasize, timeout);
	if (rc < 0) {
		throwUsbError(rc, "USB control transfer error");
	}
	return rc;
}


void UsbDevice::checkDevice() {
	if (!_usbdevh) {
		throw IOException("USB device is not open");
	}
}

} // namespace

