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



#include <tmb/io/File.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

# include <unistd.h>
# include <dirent.h>

#include <fcntl.h>
#include <libgen.h>



namespace tmb {

using namespace std;

char File::fileSeparator = '/';
char File::pathSeparator = ':';


char File::getFileSeparator()
{
	return fileSeparator;
}

char File::getPathSeparator()
{
	return pathSeparator;
}

String File::dirName(const String& a_path)
{
	String s;
	a_path.copy(s); // make a deep copy of a_path, because dirname() may modifiy the argument string
	return ::dirname(s.str());
}
String File::baseName(const String& a_path)
{
	String s;
	a_path.copy(s); // make a deep copy of a_path, because basename() may modifiy the argument string
	return ::basename(s.str());
}


File::File(const String& a_filename)
{
	setFileName(a_filename);
}

File::File(const File &a_f, const String &a_name)
{
	setFileName(a_f.getFileName() + getFileSeparator() + a_name);
}

File::File(const String& a_path, const String& a_name)
{
	setFileName(a_path + getFileSeparator() + a_name);
}


File::File(const File& a_f)
		: c_filename(a_f.c_filename), c_name(a_f.c_name), c_path(a_f.c_path)
{
}

const File& File::operator= (const File& a_f)
{
	if ( this != &a_f )
	{
		c_filename = a_f.c_filename;
		c_name = a_f.c_name;
		c_path = a_f.c_path;
	}
	return *this;
}

const File& File::operator= (const String& a_f)
{
	setFileName(a_f);
	return *this;
}


void File::setFileName(const String& a_filename)
{
	c_filename = a_filename;

	int m_index = a_filename.lastIndexOf(getFileSeparator());

	c_path = a_filename.substring(0, m_index);
	if ( m_index >= 0 )
		c_name = a_filename.substring(m_index + 1);
	else
		c_name.clear();
}


String File::getParent() const
{
	int m_index = c_path.lastIndexOf(getFileSeparator());
	if ( m_index >= 0 )
		return c_path.substring(0, m_index);
	return String::null;
}


void File::dirList(StringList& a_sl, bool a_ignoredots) const
{
	a_sl.clear();
	if ( !isDirectory() )
		throw IOException(getFileName() + " : not a directory");

	DIR* m_handle = ::opendir(c_filename.c_str());
	if ( m_handle == 0 )
		throw IOException(getFileName() + " : can not open directory");
	for(;;)
	{
		errno = 0;
		struct dirent* m_dirent = ::readdir(m_handle);
		if ( m_dirent == 0 )
		{
			if ( errno != 0 )
			{
				::closedir(m_handle);
				throw IOException();
			}
			break;
		}
		if ( a_ignoredots && (::strcmp(m_dirent->d_name, ".") == 0
							|| ::strcmp(m_dirent->d_name, "..") == 0) )
			continue;
		a_sl.append(String(m_dirent->d_name));
	}
	::closedir(m_handle);
}


void File::copy(const String& a_newfile, bool a_recursive) const
{
	if ( !isDirectory() )
	{
		int m_in = ::open(getFileName().c_str(), O_RDONLY);
		if ( m_in < 0 )
		{
			throw FileNotFoundException(String(getFileName()) << " : " << ::strerror(errno), errno);
		}

		int m_out = ::open(a_newfile.c_str(), O_CREAT|O_WRONLY|O_TRUNC, 0777);
		if ( m_out < 0 )
		{
			::close(m_in);
			throw IOException(String("can not create file: ") << a_newfile << " : " << ::strerror(errno), errno);
		}

		struct stat m_stat;
		int m_ret = ::fstat(m_in, &m_stat);
		if ( m_ret != 0 )
		{
			::close(m_in);
			::close(m_out);
			throw IOException(String(::strerror(errno)), errno);
		}

		unsigned long m_size = m_stat.st_size;

		unsigned long m_bufsize = (m_size < 1024 * 1024 ? m_size : 1024 * 1024);
		char* m_buffer = new char [m_bufsize + 1];

		for(;;)
		{
			long m_len = ::read(m_in, m_buffer, m_bufsize);
			if ( m_len < 0 )
			{
				::close(m_in);
				::close(m_out);
				delete m_buffer;
				throw IOException(String("file copy error: ") << getFileName() << ": " << ::strerror(errno), errno);
			}
			if ( m_len == 0 )
				break;
			if ( m_len != ::write(m_out, m_buffer, m_len) )
			{
				::close(m_in);
				::close(m_out);
				delete m_buffer;
				throw IOException(String("file copy error: ") << getFileName() << ": " << ::strerror(errno), errno);
			}
		}
		::close(m_in);
		::close(m_out);
		delete m_buffer;
	}
}

void File::copy(const File& a_newfile, bool a_recursive) const
{
	copy(a_newfile.getFileName(), a_recursive);
}


void File::remove(bool a_recursive) const
{
	errno = 0;
    int r = ::remove(c_filename.c_str());
	if ( r == -1 )
	{
		throw IOException(getFileName() + " : " + strerror(errno));
	}
}

void File::rename(const String& a_newfile)
{
	throw IOException();
}

void File::rename(const File& a_newfile)
{
	rename(a_newfile.getFileName());
}

void File::mkdir(bool a_recursive) const
{
	if ( a_recursive && c_path.length() > 0 )
	{
		File parent(c_path);
		if ( !parent.exists() )
			parent.mkdir(true);
	}
	errno = 0;
	int m_ret = ::mkdir(c_filename.c_str(), 0777);
	if ( m_ret == -1 )
	{
		throw IOException(getFileName() + " : " + strerror(errno));
	}
}

bool File::exists() const
{
	return (::access(c_filename.c_str(), 0) != -1);
}

bool File::isDirectory() const
{
	struct stat m_stat;

	int m_ret = ::stat(c_filename.c_str(), &m_stat);
	if ( m_ret == 0 )
	{
		return S_ISDIR(m_stat.st_mode);
	}
	return false;
}

bool File::isFile() const
{
	struct stat m_stat;

	int m_ret = ::stat(c_filename.c_str(), &m_stat);
	if ( m_ret == 0 )
	{
		return S_ISREG(m_stat.st_mode);
	}
	return false;
}

bool File::isSymLink() const
{
	struct stat m_stat;

	int m_ret = ::lstat(c_filename.c_str(), &m_stat);
	if ( m_ret == 0 )
	{
		return S_ISREG(m_stat.st_mode);
	}
	return false;
}

uint File::size() const
{
	struct stat stat;
	errno = 0;
	int r = ::stat(c_filename.c_str(), &stat);
	if ( r != 0 )
	{
		throw IOException(getFileName() + " : " + strerror(errno));
	}
	return stat.st_size;
}

uint File::mtime() const
{
	struct stat stat;
	errno = 0;
	int r = ::stat(c_filename.c_str(), &stat);
	if ( r != 0 )
	{
		throw IOException(getFileName() + " : " + strerror(errno));
	}
	return stat.st_mtime;
}

void File::load(Array<byte>& a_array) const
{
	int m_in = ::open(getFileName().c_str(), O_RDONLY);
	if ( m_in < 0 )
	{
		throw FileNotFoundException(String(getFileName()) << " : " << ::strerror(errno), errno);
	}

	struct stat m_stat;
	int m_ret = ::fstat(m_in, &m_stat);
	if ( m_ret != 0 )
	{
		::close(m_in);
		throw IOException(String(::strerror(errno)), errno);
	}

	int m_size = m_stat.st_size;

	a_array.clear();
	a_array.setSize(m_size);

	int m_len = ::read(m_in, (void*)a_array.data(), a_array.size());

	::close(m_in);

	if ( m_len != m_size )
	{
		throw IOException(String(getFileName()) << " : " << ::strerror(errno), errno);
	}
}

String File::load() const
{
	return load(false);
}
String File::load(bool a_checkzero) const
{
	int in = ::open(getFileName().c_str(), O_RDONLY);
	if ( in < 0 )
	{
		throw FileNotFoundException(String(getFileName()) << " : " << ::strerror(errno), errno);
	}

	struct stat stat;
	int ret = ::fstat(in, &stat);
	if ( ret != 0 )
	{
		::close(in);
		throw IOException(String(::strerror(errno)), errno);
	}

	int size = stat.st_size;

	String str(size, 0);

	int len = ::read(in, (void*)str.str(), str.length());

	::close(in);

	if ( len != size )
	{
		throw IOException(String(getFileName()) << " : " << ::strerror(errno), errno);
	}

	if ( a_checkzero )
	{
		while ( --len >= 0 )
		{
			if ( str[len] == 0 )
				throw InvalidOperationException(String(getFileName()) << " : string contains zeros");
		}
	}

	return str;
}

} // namespace
