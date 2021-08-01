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

#ifndef _TMB_LOG_FILELOGDEVICE_H_
#define _TMB_LOG_FILELOGDEVICE_H_

#include <tmb/lang/String.h>
#include <tmb/io/File.h>
#include <tmb/log/LogDevice.h>
#include <tmb/thread/Mutex.h>

namespace tmb {

class FileLogDevice : public LogDevice
{
	protected:
		String	_filename;
		Mutex	_file_mutex;

	public: FileLogDevice(int a_types);
	public: FileLogDevice(LogLevel a_level);
	public: FileLogDevice(const String& a_filename, int a_types);
	public: FileLogDevice(const String& a_filename, LogLevel a_level);
	public: FileLogDevice(const File& a_file, int a_types);
	public: FileLogDevice(const File& a_file, LogLevel a_level);
	
	private: FileLogDevice(const FileLogDevice&);
	private: void operator=(const FileLogDevice&);
	
	public: virtual ~FileLogDevice();
	
	public: virtual const String& getFileName() const;
	public: virtual void setFileName(const String& a_filename);
	public: virtual void setFileName(const File& a_file);
	
	public: virtual void write(LogDevice::LogType a_type, const String& a_output);
};


} // namespace

#endif
