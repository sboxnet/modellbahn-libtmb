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

#ifndef _TMB_LOG_LOGCONTROLLER_H_
#define _TMB_LOG_LOGCONTROLLER_H_

#include <tmb/lang/String.h>
#include <tmb/lang/Map.h>
#include <tmb/lang/ListP.h>

#include <tmb/log/LogDevice.h>

namespace tmb {

class LogController
{
	protected:
		ListP<LogDevice> _devices;
		int				_types_enabled;
		String 			_prefix;

	public: LogController();
	
	private: LogController(const LogController&);
	private: void operator=(const LogController&);
	
	public: virtual ~LogController();
	
	
	private: void iwrite(const String& a_s, LogDevice::LogType a_type) const;
	
	public: virtual void debug(const String& a_s) const;
	public: virtual void info(const String& a_s) const;
	public: virtual void notice(const String& a_s) const;
	public: virtual void warning(const String& a_s) const;
	public: virtual void error(const String& a_s) const;
	public: virtual void critical(const String& a_s) const;
	public: virtual void alert(const String& a_s) const;
	public: virtual void fatal(const String& a_s) const;
	
	public: virtual void debug(const char* a_s, ...) const;
	public: virtual void info(const char* a_s, ...) const;
	public: virtual void notice(const char* a_s, ...) const;
	public: virtual void warning(const char* a_s, ...) const;
	public: virtual void error(const char* a_s, ...) const;
	public: virtual void critical(const char* a_s, ...) const;
	public: virtual void alert(const char* a_s, ...) const;
	public: virtual void fatal(const char* a_s, ...) const;
	
	
	public: virtual void add(LogDevice* a_dev);
	
	public: virtual int getNumberOfDevices() const;

	public: virtual void setTypesEnabled(int a_types, bool a_on);
	public: virtual void setTypeEnabled(LogDevice::LogType a_type, bool a_on);
	public: virtual void setLogLevel(LogDevice::LogLevel a_level);
	public: virtual bool isTypeEnabled(LogDevice::LogType a_type) const;

	public: virtual const String& getPrefix() const;
	public: virtual void setPrefix(const String& a_s);
};


} // namespace

#endif
