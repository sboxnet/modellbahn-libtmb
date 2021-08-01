/***************************************************************************
 *   Copyright (C) 2000 - 2005 by Thomas Maier                             *
 *   balagi@justmail.de                                                    *
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
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef _TMB_XML_XMLTEMPLATE_H_
#define _TMB_XML_XMLTEMPLATE_H_


#include <tmb/lang/String.h>
#include <tmb/lang/HashMap.h>
#include <tmb/io/File.h>
#include <tmb/io/OutputStream.h>
#include <tmb/xml/Xml.h>


namespace tmb {


class XmlTemplate
{
	public: typedef HashMap<String> TagMap;

	protected:
		String		c_template;
		TagMap		c_tagmap;

	public: XmlTemplate();
	public: XmlTemplate(const String& a_tmpl);
	public: XmlTemplate(const XmlTemplate&);
	public: const XmlTemplate& operator=(const XmlTemplate&);
	public: const XmlTemplate& operator=(const String&);

	public: virtual ~XmlTemplate();

	public: void setContent(const String& a_tmpl);

	public: String& content();
	public: const String& content() const;

	public: void load(const String& a_filename);
	public: void load(const File& a_file);

	public: void addTag(const String& a_tag, const String& a_val);

	public: void writeTo(OutputStream& a_stream);
};

} // namespace

#endif
