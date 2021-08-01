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

#include <tmb/util/Properties.h>
#include <tmb/lang/StringTokenizer.h>
#include <tmb/lang/StringList.h>
#include <fstream>

using namespace std;

namespace tmb {
	
Properties::Properties() {
}

Properties::~Properties() {
}

void Properties::put(const String& a_key, const String& a_val) {
	_props.put(a_key, a_val);
}

bool Properties::contains(const String& a_key) const {
	return _props.containsKey(a_key);
}
	
const String& Properties::get(const String& a_key) const {
	try {
		return _props.get(a_key);
		
	} catch(const NotFoundException&) {
		return String::null;
	}
}
const String& Properties::get(const String& a_key, const String& a_default) const {
	try {
		const String& s = _props.get(a_key);
		if (!s.isEmpty())
			return s;
	} catch(const NotFoundException&) {
	}
	return a_default;
}

void Properties::clear() {
	_props.clear();
}
	
void Properties::load(const String& a_filename) {
	load(File(a_filename));
}

void Properties::save(const String& a_filename) {
	save(File(a_filename));
}
	
void Properties::load(const File& a_file) {
	String s = a_file.load();
	int idx = 0;
	int idx2 = 0;
	for (; idx < s.length() && idx2 >= 0; idx = idx2 + 1) {
		idx2 = s.indexOf('\n', idx);		
		String line = s.substring(idx, idx2);
		
		int i = line.indexOf('=');
		if (i >= 0) {
			line[i] = ' ';
			line.insert(" =", i);
		}
		StringTokenizer tz(line, "#", "\"\'", true);
		String key;
		String eq;
		String val;
		try {
			key = tz.next();
		} catch(const NotFoundException&) {
			continue;
		}
		try {
			eq  = tz.next();
			val = tz.next();
		} catch(const NotFoundException&) {
		}
		
		if (!key.isEmpty() && eq == "=" && !tz.hasNext())
			_props.put(key, val);
		else
			throw ParseException(line);
	}
}
	
void Properties::save(const File& a_file) {
	
	ofstream out(a_file.getFileName());
	
	HashMap<String>::Key k = 0;
	String val;
	const String* key;
	while ((k = _props.nextKey(k, val, &key)) != 0) {
		out << '\"' << *key << "\"=\"" << val << "\"\n";
	}
	
	out.close();
}


byte   Properties::getByte(const String& a_key) const {
	String s;
	try {
		s = _props.get(a_key);
		unsigned long i = s.toUnsignedLong();
		if (i > 0x00fful)
			throw NumberFormatException();
		return (byte)i;
	} catch(const NotFoundException&) {
		throw NotFoundException(a_key);
	} catch(const NumberFormatException& ex) {
		throw NumberFormatException(a_key + ": value is not a byte: " + s);
	}
}

int32  Properties::getInt32(const String& a_key) const {
	String s;
	try {
		s = _props.get(a_key);
		long i = s.toLong();
		return i;
	} catch(const NotFoundException&) {
		throw NotFoundException(a_key);
	} catch(const NumberFormatException& ex) {
		throw NumberFormatException(a_key + ": value is not a int32: " + s);
	}
}

uint32 Properties::getUInt32(const String& a_key) const {
	String s;
	try {
		s = _props.get(a_key);
		unsigned long i = s.toUnsignedLong();
		return i;
	} catch(const NotFoundException&) {
		throw NotFoundException(a_key);
	} catch(const NumberFormatException& ex) {
		throw NumberFormatException(a_key + ": value is not a uint32: " + s);
	}
}

int16  Properties::getInt16(const String& a_key) const {
	String s;
	try {
		s = _props.get(a_key);
		long i = s.toLong();
		if (i > 32767 || i < -32768)
			throw NumberFormatException();
		return (int16)i;
	} catch(const NotFoundException&) {
		throw NotFoundException(a_key);
	} catch(const NumberFormatException& ex) {
		throw NumberFormatException(a_key + ": value is not a int16: " + s);
	}
}

uint16 Properties::getUInt16(const String& a_key) const {
	String s;
	try {
		s = _props.get(a_key);
		unsigned long i = s.toUnsignedLong();
		if (i > 0x00fffful)
			throw NumberFormatException();
		return (uint16)i;
	} catch(const NotFoundException&) {
		throw NotFoundException(a_key);
	} catch(const NumberFormatException& ex) {
		throw NumberFormatException(a_key + ": value is not a uint16: " + s);
	}
}

byte   Properties::getByte(const String& a_key, byte a_default) const {
	try {
		return getByte(a_key);
	} catch(const NotFoundException&) {
		return a_default;
	}
}

int32  Properties::getInt32(const String& a_key, int32 a_default) const {
	try {
		return getInt32(a_key);
	} catch(const NotFoundException&) {
		return a_default;
	}
}

uint32 Properties::getUInt32(const String& a_key, uint32 a_default) const {
	try {
		return getUInt32(a_key);
	} catch(const NotFoundException&) {
		return a_default;
	}
}

int16  Properties::getInt16(const String& a_key, int16 a_default) const {
	try {
		return getInt16(a_key);
	} catch(const NotFoundException&) {
		return a_default;
	}
}

uint16 Properties::getUInt16(const String& a_key, uint16 a_default) const {
	try {
		return getUInt16(a_key);
	} catch(const NotFoundException&) {
		return a_default;
	}
}


	
} // namespace
