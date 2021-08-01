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

#ifndef _TMB_LOG_SYSLOGDEVICE_H_
#define _TMB_LOG_SYSLOGDEVICE_H_

#include <tmb/lang/String.h>
#include <tmb/log/LogDevice.h>

namespace tmb {
	
class SysLogDevice : public LogDevice
{
	protected:
		String _appname;

	public: SysLogDevice(int a_types);
	public: SysLogDevice(const String& a_appname, int a_types);
	public: SysLogDevice(LogLevel a_level = LogLevel_INFO);
	public: SysLogDevice(const String& a_appname, LogLevel a_level = LogLevel_INFO);

	private: SysLogDevice(const SysLogDevice& a_ld);
	private: void operator=(const SysLogDevice& a_ld);
	
	public: virtual ~SysLogDevice();
	
	public: virtual const String& getAppName() const;
	public: virtual void setAppName(const String& a_name);

	public: virtual void write(LogDevice::LogType a_type, const String& a_output);
};


} // namespace

#endif
