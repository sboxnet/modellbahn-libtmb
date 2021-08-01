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

#include <tmb/log/BufferedLogDevice.h>

namespace tmb {

BufferedLogDevice::BufferedLogDevice(LogDevice* a_outdev, int a_types, int a_bufsize)
	: LogDevice(a_types),
		_out_device(a_outdev),
		_mutex(),
		_buffer(a_bufsize + 16)
{
}

BufferedLogDevice::BufferedLogDevice(LogDevice* a_outdev, LogLevel a_level, int a_bufsize)
	: LogDevice(a_level),
	  _out_device(a_outdev),
	  _mutex(),
	  _buffer(a_bufsize + 16)
{
}

	
BufferedLogDevice::~BufferedLogDevice() {
	flush();
	delete _out_device;
}

LogDevice* BufferedLogDevice::getOutputDevice() const {
	return _out_device;
}

void BufferedLogDevice::write(LogDevice::LogType a_type, const String& a_output) {
	if (!_out_device)
		return;

	_mutex.lock();

	int buflen = _buffer.length();
	if (buflen + a_output.length() <= (_buffer.bufsize() - 2)) {
		_buffer << a_output;
	} else {
		try	{
			if (buflen)
				_out_device->write(Log_INFO, _buffer + a_output);
			else
				_out_device->write(Log_INFO, a_output);
		} catch(...) {
		}
		_buffer.clear();
	}
	_mutex.unlock();
}

void BufferedLogDevice::flush() {
	if ( !_out_device )
		return;

	_mutex.lock();

	if (_buffer.length() > 0) {
		try { _out_device->write(Log_INFO, _buffer); } catch(...) {}
		_buffer.clear();
	}
	_mutex.unlock();
}


} // namespace
