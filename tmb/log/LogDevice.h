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

#ifndef _TMB_LOG_LOGDEVICE_H_
#define _TMB_LOG_LOGDEVICE_H_

#include <tmb/lang/String.h>


namespace tmb {

class LogDevice
{
	public: enum LogType {
				Log_FATAL		= 1 << 0,
				Log_ALERT		= 1 << 1,
				Log_CRITICAL	= 1 << 2,
				Log_ERROR		= 1 << 3,
				Log_WARNING		= 1 << 4,
				Log_NOTICE		= 1 << 5,
				Log_INFO		= 1 << 6,
				Log_DEBUG		= 1 << 7,
			};
	public: enum LogLevel {
				LogLevel_FATAL		= 0x0001,
	   			LogLevel_ALERT 		= 0x0003,
		  		LogLevel_CRITICAL 	= 0x0007,
		 		LogLevel_ERROR		= 0x000f,
				LogLevel_WARNING	= 0x001f,
	   			LogLevel_NOTICE		= 0x003f,
		  		LogLevel_INFO		= 0x007f,
		 		LogLevel_DEBUG		= 0x00ff,
			};
	
	protected: int		_types;
	protected: bool		_enabled;
	protected: String	_prefix;
	
	public: LogDevice(int a_types);
	public: LogDevice(LogLevel a_level = LogLevel_INFO);
			
	private: LogDevice(const LogDevice&);
	private: void operator=(const LogDevice&);
	
	public: virtual ~LogDevice();
	
	
	public: virtual int getTypes() const;
	public: virtual bool isType(LogDevice::LogType a_type) const;
	public: virtual void setTypesEnabled(int a_type, bool a_on);
	public: virtual void setTypeEnabled(LogDevice::LogType a_type, bool a_on);
	public: virtual void setTypeLevel(LogDevice::LogLevel a_level);

	public: virtual void setEnabled(bool a_on);
	public: virtual bool isEnabled() const;

	public: virtual const String& getPrefix() const;
	public: virtual void setPrefix(const String& a_s);

	public: virtual bool canWrite(LogDevice::LogType a_type) const;
	
	public: virtual void write(LogDevice::LogType a_type, const String& a_output) = 0;


	public: virtual String format(LogDevice::LogType a_type, const String& a_msg,
								  const String& a_defaultprefix = String::null) const;
};

} // namespace

#endif // _TMB_LOG_LOGDEVICE_H_
