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

#ifndef _TMB_UTIL_PROPERTIES_H_
#define _TMB_UTIL_PROPERTIES_H_

#include <tmb/lang/String.h>
#include <tmb/lang/HashMap.h>
#include <tmb/io/File.h>

namespace tmb {

class Properties {
	
	protected:
		HashMap<String>	_props;
		
	public: Properties();
	public: virtual ~Properties();
	
	public: void put(const String& a_key, const String& a_val);
	public: bool contains(const String& a_key) const;
	public: const String& get(const String& a_key) const;
	public: const String& get(const String& a_key, const String& a_default) const;
	
	public: byte   getByte(const String& a_key) const;
	public: int32  getInt32(const String& a_key) const;
	public: uint32 getUInt32(const String& a_key) const;
	public: int16  getInt16(const String& a_key) const;
	public: uint16 getUInt16(const String& a_key) const;
	
	public: byte   getByte(const String& a_key, byte a_default) const;
	public: int32  getInt32(const String& a_key, int32 a_default) const;
	public: uint32 getUInt32(const String& a_key, uint32 a_default) const;
	public: int16  getInt16(const String& a_key, int16 a_default) const;
	public: uint16 getUInt16(const String& a_key, uint16 a_default) const;

	public: void clear();
	
	public: void load(const String& a_filename);
	public: void load(const File& a_file);
	
	public: void save(const String& a_filename);
	public: void save(const File& a_file);
	
	public: const HashMap<String>& map() const { return _props; }
	
};
	
} // namespace

#endif
