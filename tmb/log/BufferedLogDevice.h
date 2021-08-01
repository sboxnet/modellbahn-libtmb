/***************************************************************************
 *   Copyright (C) 2007
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

#ifndef _TMB_LOG_BUFFEREDLOGDEVICE_H_
#define _TMB_LOG_BUFFEREDLOGDEVICE_H_

#include <tmb/lang/String.h>
#include <tmb/log/LogDevice.h>
#include <tmb/thread/Mutex.h>

namespace tmb {

class BufferedLogDevice : public LogDevice
{
	protected:
		LogDevice*	_out_device;
		Mutex		_mutex;
		String		_buffer;

	public: enum { DEFAULT_BUFSIZE = 1024 };

	public: BufferedLogDevice(LogDevice* a_outdev, int a_types, int a_bufsize = DEFAULT_BUFSIZE);
	public: BufferedLogDevice(LogDevice* a_outdev, LogLevel a_level = LogLevel_INFO, int a_bufsize = DEFAULT_BUFSIZE);
	
	private: BufferedLogDevice(const BufferedLogDevice& a_ld);
	private: void operator=(const BufferedLogDevice& a_ld);
	
	public: virtual ~BufferedLogDevice();
	
	public: virtual LogDevice* getOutputDevice() const;
	
	public: virtual void write(LogDevice::LogType a_type, const String& a_output);

	public: virtual void flush();
};


} // namespace

#endif
