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

#include <tmb/log/LogDevice.h>

#include <tmb/lang/Exception.h>


#include <time.h>
#include <stdio.h>

namespace tmb {

LogDevice::LogDevice(int a_types)
	: _types(a_types), _enabled(true) {
}

LogDevice::LogDevice(LogDevice::LogLevel a_level)
	: _types(a_level), _enabled(true) {
}

LogDevice::~LogDevice() {
}


int LogDevice::getTypes() const {
	return _types;
}

bool LogDevice::isType(LogDevice::LogType a_type) const {
	return ((int)a_type & _types) != 0;
}

void LogDevice::setTypeEnabled(LogDevice::LogType a_type, bool a_on) {
	if (a_on)
		_types |= (int)a_type;
	else
		_types &= ~(int)a_type;
}

void LogDevice::setTypesEnabled(int a_types, bool a_on) {
	if (a_on)
		_types |= a_types;
	else
		_types &= ~a_types;
}

void LogDevice::setTypeLevel(LogDevice::LogLevel a_level) {
	_types = a_level;
}


void LogDevice::setEnabled(bool a_on) {
	_enabled = a_on;
}

bool LogDevice::isEnabled() const {
	return _enabled;
}

const String& LogDevice::getPrefix() const {
	return _prefix;
}

void LogDevice::setPrefix(const String& a_s) {
	_prefix = a_s;
}

bool LogDevice::canWrite(LogDevice::LogType a_type) const {
	return ((int)a_type & _types) && _enabled;
}


String LogDevice::format(LogDevice::LogType a_type, const String& a_msg, const String& a_defaultprefix) const {
	const char* typestr;
	switch (a_type)
	{
		case Log_FATAL:		typestr = "|FATAL|";	break;
		case Log_ALERT:		typestr = "|ALERT|";	break;
		case Log_CRITICAL:	typestr = "|CRITICAL|";	break;
		case Log_ERROR:		typestr = "|ERROR|";	break;
		case Log_WARNING:	typestr = "|WARNING|";	break;
		case Log_NOTICE:	typestr = "|NOTICE|";	break;
		case Log_INFO:		typestr = "|INFO|";		break;
		case Log_DEBUG:		typestr = "|DEBUG|";	break;
		default:			typestr = "|?|";		break;
	}

	const String& prefix = (_prefix.length() > 0 ? _prefix : a_defaultprefix);

	time_t curtime;
	struct tm tms, *ptm = &tms;
	curtime = ::time(NULL);
	ptm = ::localtime_r(&curtime, ptm);

	char buffer[64];
	::strftime(buffer, sizeof(buffer)-1, "%Y.%m.%d-%H:%M:%S", ptm);

	String msg(a_msg.length() + 64);
	msg << buffer;
	if ( prefix.length() > 0 )
		msg << "|" << prefix;
	msg << typestr << a_msg << "\n";

	return msg;
}


} // namespace
