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

#ifndef _TMB_LOG_STDOUTLOGDEVICE_H_
#define _TMB_LOG_STDOUTLOGDEVICE_H_

#include <tmb/lang/String.h>
#include <tmb/log/LogDevice.h>

namespace tmb {

class StdOutLogDevice : public LogDevice
{
	public: StdOutLogDevice(int a_types);
	public: StdOutLogDevice(LogLevel a_level = LogLevel_INFO);
	
	private: StdOutLogDevice(const StdOutLogDevice&);
	private: void operator=(const StdOutLogDevice&);
	
	public: virtual ~StdOutLogDevice();
	
	public: virtual void write(LogDevice::LogType a_type, const String& a_output);
};


} // namespace

#endif
