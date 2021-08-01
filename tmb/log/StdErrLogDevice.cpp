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

#include <tmb/log/StdErrLogDevice.h>

#include <iostream>

using namespace std;

namespace tmb {

StdErrLogDevice::StdErrLogDevice(int a_types)
	: LogDevice(a_types) {
}
StdErrLogDevice::StdErrLogDevice(LogLevel a_level)
	: LogDevice(a_level) {
}
	
StdErrLogDevice::~StdErrLogDevice() {
}
	
void StdErrLogDevice::write(LogDevice::LogType a_type, const String& a_output) {
	cerr << a_output << flush;
}


} // namespace
