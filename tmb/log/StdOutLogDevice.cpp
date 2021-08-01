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

#include <tmb/log/StdOutLogDevice.h>

#include <iostream>

using namespace std;

namespace tmb {

StdOutLogDevice::StdOutLogDevice(int a_types)
	: LogDevice(a_types) {
}
StdOutLogDevice::StdOutLogDevice(LogLevel a_level)
	: LogDevice(a_level) {
}
	
StdOutLogDevice::~StdOutLogDevice() {
}
	
	
void StdOutLogDevice::write(LogDevice::LogType a_type, const String& a_output) {
	cout << a_output << flush;
}


} // namespace
