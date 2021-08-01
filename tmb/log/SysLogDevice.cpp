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

#include <tmb/log/SysLogDevice.h>

#include <syslog.h>

namespace tmb {

SysLogDevice::SysLogDevice(int a_types)
	: LogDevice(a_types), _appname() {
}
SysLogDevice::SysLogDevice(const String& a_name, int a_types)
	: LogDevice(a_types), _appname(a_name) {
}
SysLogDevice::SysLogDevice(LogLevel a_level)
	: LogDevice(a_level), _appname() {
	}

SysLogDevice::SysLogDevice(const String& a_name, LogLevel a_level)
	: LogDevice(a_level), _appname(a_name) {
}
		
SysLogDevice::~SysLogDevice() {
}

const String& SysLogDevice::getAppName() const {
	return _appname;
}

void SysLogDevice::setAppName(const String& a_name) {
	_appname = a_name;
}
	
void SysLogDevice::write(LogDevice::LogType a_type, const String& a_output) {
	int level;
	switch (a_type) {
		case Log_FATAL:		level = LOG_EMERG;	break;
		case Log_ALERT:		level = LOG_ALERT;	break;
		case Log_CRITICAL:	level = LOG_CRIT;	break;
		case Log_ERROR:		level = LOG_ERR;	break;
		case Log_WARNING:	level = LOG_WARNING;	break;
		case Log_NOTICE:	level = LOG_NOTICE;	break;
		case Log_INFO:		level = LOG_INFO;	break;
		case Log_DEBUG:		level = LOG_DEBUG;	break;
		default:			level = LOG_INFO;	break;
	}
	
	::openlog("", LOG_PID|LOG_NOWAIT, LOG_USER);
	
	::syslog(level | LOG_USER, "%s", a_output.c_str());

	::closelog();
}


} // namespace
