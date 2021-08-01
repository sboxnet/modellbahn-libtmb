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

#include <tmb/log/FileLogDevice.h>

#include <iostream>
#include <fstream>

using namespace std;

namespace tmb {

FileLogDevice::FileLogDevice(int a_types)
					: LogDevice(a_types),
					  _filename(),
					  _file_mutex()
{
}

FileLogDevice::FileLogDevice(const String& a_filename, int a_types)
					: LogDevice(a_types),
					  _filename(a_filename),
					  _file_mutex()
{
}

FileLogDevice::FileLogDevice(const File& a_file, int a_types)
					: LogDevice(a_types),
					  _filename(a_file.getFileName()),
					  _file_mutex()
{
}
	
FileLogDevice::FileLogDevice(LogLevel a_level)
	: LogDevice(a_level), _filename(), _file_mutex() {
}

FileLogDevice::FileLogDevice(const String& a_filename, LogLevel a_level)
	: LogDevice(a_level), _filename(a_filename), _file_mutex() {
}

FileLogDevice::FileLogDevice(const File& a_file, LogLevel a_level)
	: LogDevice(a_level), _filename(a_file.getFileName()), _file_mutex() {
}

FileLogDevice::~FileLogDevice() {
}

const String& FileLogDevice::getFileName() const {
	return _filename;
}

void FileLogDevice::setFileName(const String& a_filename) {
	_filename = a_filename;
}

void FileLogDevice::setFileName(const File& a_file) {
	_filename = a_file.getFileName();
}
	
void FileLogDevice::write(LogDevice::LogType a_type, const String& a_output) {
	if (_filename.length() > 0) {
		_file_mutex.lock();

		ofstream f(_filename.c_str(), ios::out|ios::app);
		if (f.good()) {
			f << a_output << flush;
			f.close();
		}
		_file_mutex.unlock();
	}
}


} // namespace
