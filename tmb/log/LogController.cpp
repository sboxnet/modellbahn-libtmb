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

#include <tmb/log/LogController.h>

#include <stdarg.h>


namespace tmb {

LogController::LogController()
	: _devices(AutoDelete_DESTRUCT) {
	_types_enabled = LogDevice::LogLevel_INFO;
}

LogController::~LogController() {
}

void LogController::iwrite(const String& a_s, LogDevice::LogType a_type) const {
	if (!((int)a_type & _types_enabled))
		return;

	int n = _devices.size();
	for (int i = 0; i < n; i++) {
		LogDevice* logdev = _devices.get(i);
		if (logdev->canWrite(a_type))
			logdev->write(a_type, logdev->format(a_type, a_s, _prefix));
	}
}

void LogController::debug(const String& a_s) const {
	iwrite(a_s, LogDevice::Log_DEBUG);
}

void LogController::info(const String& a_s) const {
	iwrite(a_s, LogDevice::Log_INFO);
}

void LogController::notice(const String& a_s) const {
	iwrite(a_s, LogDevice::Log_NOTICE);
}

void LogController::warning(const String& a_s) const {
	iwrite(a_s, LogDevice::Log_WARNING);
}

void LogController::error(const String& a_s) const {
	iwrite(a_s, LogDevice::Log_ERROR);
}

void LogController::critical(const String& a_s) const {
	iwrite(a_s, LogDevice::Log_CRITICAL);
}

void LogController::alert(const String& a_s) const {
	iwrite(a_s, LogDevice::Log_ALERT);
}

void LogController::fatal(const String& a_s) const {
	iwrite(a_s, LogDevice::Log_FATAL);
}


void LogController::debug(const char* a_fmt, ...) const {
	String s;
	va_list va;
	va_start(va, a_fmt);
	s.vformat(a_fmt, va);
	va_end(va);
	debug(s);
}

void LogController::info(const char* a_fmt, ...) const {
	String s;
	va_list va;
	va_start(va, a_fmt);
	s.vformat(a_fmt, va);
	va_end(va);
	info(s);
}

void LogController::notice(const char* a_fmt, ...) const {
	String s;
	va_list va;
	va_start(va, a_fmt);
	s.vformat(a_fmt, va);
	va_end(va);
	notice(s);
}

void LogController::warning(const char* a_fmt, ...) const {
	String s;
	va_list va;
	va_start(va, a_fmt);
	s.vformat(a_fmt, va);
	va_end(va);
	warning(s);
}

void LogController::error(const char* a_fmt, ...) const {
	String s;
	va_list va;
	va_start(va, a_fmt);
	s.vformat(a_fmt, va);
	va_end(va);
	error(s);
}

void LogController::critical(const char* a_fmt, ...) const {
	String s;
	va_list va;
	va_start(va, a_fmt);
	s.vformat(a_fmt, va);
	va_end(va);
	critical(s);
}

void LogController::alert(const char* a_fmt, ...) const {
	String s;
	va_list va;
	va_start(va, a_fmt);
	s.vformat(a_fmt, va);
	va_end(va);
	alert(s);
}

void LogController::fatal(const char* a_fmt, ...) const {
	String s;
	va_list va;
	va_start(va, a_fmt);
	s.vformat(a_fmt, va);
	va_end(va);
	fatal(s);
}



void LogController::add(LogDevice* a_dev) {
	if (a_dev != 0)
		_devices.add(a_dev);
}

int LogController::getNumberOfDevices() const {
	return _devices.size();
}

void LogController::setTypeEnabled(LogDevice::LogType a_type, bool a_on) {
	if (a_on)
		_types_enabled |= (1 << a_type);
	else
		_types_enabled &= ~(1 << a_type);
}

void LogController::setTypesEnabled(int a_types, bool a_on) {
	if (a_on)
		_types_enabled |= a_types;
	else
		_types_enabled &= ~a_types;
}

void LogController::setLogLevel(LogDevice::LogLevel a_level) {
	_types_enabled = a_level;
}


bool LogController::isTypeEnabled(LogDevice::LogType a_type) const {
	return (_types_enabled & (1 << (int)a_type)) != 0;
}

const String& LogController::getPrefix() const {
	return _prefix;
}

void LogController::setPrefix(const String& a_s) {
	_prefix = a_s;
}


} // namespace
