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


#include <tmb/xml/XmlTemplate.h>


namespace tmb {


XmlTemplate::XmlTemplate()
				: c_template()
{
}

XmlTemplate::XmlTemplate(const String& a_tmpl)
				: c_template(a_tmpl)
{
}

XmlTemplate::~XmlTemplate()
{
}

XmlTemplate::XmlTemplate(const XmlTemplate& a_tmpl)
				: c_template(a_tmpl.c_template)
{
}

const XmlTemplate& XmlTemplate::operator=(const XmlTemplate& a_tmpl)
{
	c_template = a_tmpl.c_template;
	return *this;
}

const XmlTemplate& XmlTemplate::operator=(const String& a_tmpl)
{
	c_template = a_tmpl;
	return *this;
}

void XmlTemplate::setContent(const String& a_tmpl)
{
	c_template = a_tmpl;
}

String& XmlTemplate::content()
{
	return c_template;
}
const String& XmlTemplate::content() const
{
	return c_template;
}

void XmlTemplate::load(const String& a_filename)
{
	load(File(a_filename));
}

void XmlTemplate::load(const File& a_file)
{
	c_template = a_file.load();
}

void XmlTemplate::addTag(const String& a_tag, const String& a_val)
{
	c_tagmap.put(a_tag, a_val);
}

void XmlTemplate::writeTo(OutputStream& a_stream)
{
	int idx = 0;
	for(;;)
	{
		int previdx = idx;
		idx = c_template.indexOf("<%", previdx);
		if ( idx < 0 )
		{
			int len = c_template.length() - previdx;
			if ( len > 0 )
				a_stream.write(c_template.c_str() + previdx, len, OutputStream::WriteFlag_FORCECS);
			break;
		}
		int eidx = c_template.indexOf("%>", idx+2);
		if ( eidx < 0 )
			break;
		int len = idx - previdx;
		if ( len > 0 )
 			a_stream.write(c_template.c_str() + previdx, len, OutputStream::WriteFlag_FORCECS);
        String tag = c_template.substring(idx+2, eidx);
		try
		{
			const String& val = c_tagmap.get(tag);
			if ( val.length() > 0 )
				a_stream.write(val, OutputStream::WriteFlag_FORCECS);
		}
		catch(const NotFoundException&)
		{
		}

		idx = eidx + 2;
	}
}


} // namespace
