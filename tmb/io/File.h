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



#ifndef _TMB_IO_FILE_H_
#define _TMB_IO_FILE_H_


#include <tmb/lang/String.h>
#include <tmb/lang/StringList.h>
#include <tmb/lang/Array.h>

#include <tmb/io/IOException.h>
#include <tmb/io/FileNotFoundException.h>


namespace tmb {

/**
 * A generic file class.
 */
class File
{
	private: String c_filename; ///< full name of the file
	private: String c_name;     ///< the basename of the file
	private: String c_path;     ///< the dirname of the file

	private: static char fileSeparator; ///< file separator character
	private: static char pathSeparator; ///< path separator character

			/**
			 * Get the file separator character.
			 * @return the file separator character
			 */
	public: static char getFileSeparator();
			/**
			 * Get the path separator character.
			 * @return the path separator character
			 */
	public: static char getPathSeparator();

	public: static String dirName(const String& a_path);
	public: static String baseName(const String& a_path);

			/**
			 * Default constructor. The file name is unspecified.
			 */
	public: explicit File() {}

			/**
			 * Create a File object with the specified file name.
			 * @param a_filename    name of a file
			 */
	public: explicit File(const String& a_filename);

			/**
			 * Copy constructor. (Note: this is NOT a file copy !!)
			 * @param a_f    File object to copy
			 */
	public: File(const File& a_f);
			/**
			 * Copy constructor. (Note: this is NOT a file copy !!)
			 * @param a_f    File object to copy
			 * @param a_name name that is appended to the filename of a_f, separated by
			 *               fileSeparator
			 */
	public: File(const File& a_f, const String& a_name);
			/**
			 * Constructor. Creates a File object with a filename of
			 * a_path + fileSeparator + a_name .
			 * @param a_path   the dirname
			 * @param a_name   the basename
			 */
	public: File(const String& a_path, const String& a_name);
			/**
			 * Assignment.
			 * @param a_f   File object to assign
			 * @return this object
			 */
	public: virtual const File& operator= (const File& a_f);
	public: virtual const File& operator= (const String& a_f);

			/** Destructor */
	public: virtual ~File() {}

			/**
			 * Get the file name of this File object.
			 * @return the file name
			 */
	public: virtual const String& getFileName() const { return c_filename; }
			/**
			 * Set the file name of this File object.
			 * @param a_filename  the file name
			 */
	public: virtual void setFileName(const String& a_filename);

			/**
			 * Get the basename of this File object.
			 * @return the basename
			 */
	public: virtual const String& getName() const { return c_name; }

			/**
			 * Get the dirname of this File object.
			 * @return the dirname
			 */
	public: virtual const String& getPath() const { return c_path; }

			/**
			 * Get a new File object representing the dirname of this file object.
			 * @return the new File object
			 */
	public: virtual File getPathFile() const { return File(getPath()); }

			/**
			 * Get the filename of the parent directory of this File object.
			 * @return the filename of the parent directory
			 */
	public: virtual String getParent() const;

			/**
			 * Get the filename of the parent directory of this File object.
			 * @return the filename of the parent directory
			 */
	public: virtual File getParentFile() const { return File(getParent()); }

			/**
			 * Get a list of all file elements in the directory represented by this
			 * file object.
			 * @param a_sl    output: the list to put the elements in
			 * @exception IOException if this File object is not a directory or an other
			 *                        IO error occurs
			 */
	public: virtual void dirList(StringList& a_sl, bool a_ignoredots = true) const;

			//@{
			/**
			 * Copy the file represented by this File object to another file.
			 * @param a_newfile  filename of the new file to copy to
			 * @param a_recursive copy recursive
			 * @exception IOException on IO errors
			 */
	public: virtual void copy(const String& a_newfile, bool a_recursive = false) const;
	public: virtual void copy(const File& a_newfile, bool a_recursive = false) const;
			//@}

			/**
			 * Remove the file represented by this File object.
			 * @param a_recursive  recursive remove
			 * @exception IOException on IO errors
			 */
	public: virtual void remove(bool a_recursive = false) const;

			//@{
			/**
			 * Rename the file represented by this File object to another file.
			 * @param a_newfile  filename of the new file
			 * @exception IOException on IO errors
			 */
	public: virtual void rename(const String& a_newfile);
	public: virtual void rename(const File& a_newfile);
			//@}

			/**
			 * Make the directory represented by this File object.
			 * @param a_recursive  recursive mkdir
			 * @exception IOException on IO errors
			 */
	public: virtual void mkdir(bool a_recursive = false) const;

			/**
			 * Test if the file represented by this File object exists.
			 * @return true if exists
			 */
	public: virtual bool exists() const;

			/**
			 * Test if the file represented by this File object is a directory.
			 * @return true if it is a directory
			 */
	public: virtual bool isDirectory() const;
			/**
			 * Test if the file represented by this File object is a normal file,
			 * means not a directory.
			 * @return true if it is a normal file
			 */
	public: virtual bool isFile() const;
	public: virtual bool isSymLink() const;

	public: virtual uint size() const;
	public: virtual uint mtime() const;

	public: virtual void load(Array<byte>& a_array) const;
	public: virtual String load() const;
	public: virtual String load(bool a_checkzero) const;
};


} // namespace


#endif
