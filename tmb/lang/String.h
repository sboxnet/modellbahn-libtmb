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



#ifndef _TMB_LANG_STRING_H_
#define _TMB_LANG_STRING_H_


#include <iostream>

#include <tmb/lang/tmb.h>
#include <tmb/thread/AtomicInt.h>

namespace tmb {

/**
 * The String class. Uses the StringData class.
 */
class String
{
			// Number formats
	public: enum NumFormat { dec = 0,  ///< decimal
							 hex = 1,  ///< hexadecimal
							 oct = 2,  ///< octal
							 phex = 3, ///< pointer hexadecimal
							 poct = 4  ///< pointer octal
							};

	protected:
			struct stringdata
				{
					AtomicInt links; ///< number of references to this data
					int length;          ///< length of the C string data without the '\0'
					int bufsize;         ///< size of the data buffer
					char string[0];      ///< data buffer
				};
			struct nullstringdata
				{
					struct stringdata d;
					char null[1];
				};

			/// pointer to the string data structure
			struct stringdata* _data;

			/// the null string
			static struct nullstringdata _nullstring;


	public: static const String null;



			/** Get the link/reference count.
			 * @return the number of links referencing this data structure */
	protected: unsigned long ilinks() const throw() { return _data->links; }
			/** Set the length of the string.
			 * @param a_l  the length */
	protected: void isetlength(int a_l) throw() { _data->length = a_l; }
			/** Set the buffer size.
			 * @param a_s  the buffer size */
	protected: void isetbufsize(int a_s) throw() { _data->bufsize = a_s; }
			/** Set the number of links.
			 * @param a_l link count */
	protected: void isetlinks(unsigned long a_l) throw() { _data->links = a_l; }
			/** Increment the reference count by one. */
	protected: void iinclinks(struct stringdata* a_data) throw() { a_data->links++; };
			/** Decrement the reference count by one. */
	protected: void ideclinks(struct stringdata* a_data) throw() { a_data->links--; };
			/** Test if this string data structure is used by more than one StringData object.
			 * @return true if links() > 1 */
	protected: bool iislinked() const throw() { return (_data->links > 1); }

			/**
			 * Initialize a new StringData object with a new data structure.
			 * @param a_length     length of the string
			 * @param a_minbufsize minimum buffer size. Automatically adjusted to
			 *                     length()+1 if a_minbufsize is less then this value.
			 * @return the string length
			 */
	protected: int iinitData(int a_length, int a_minbufsize) throw();

			/**
			 * Initialize a new StringData object with a new data structure and
			 * copy string characters.
			 * @param a_s          the C string to copy into this StringData object
			 * @param a_length     length of the string
			 * @param a_minbufsize minimum buffer size. Automatically adjusted to
			 *                     length()+1 if a_minbufsize is less then this value.
			 */
	protected: void iinitCopyData(const char* a_s, int a_len, int a_minbufsize) throw();
			/**
			 * Initialize a new StringData object with a new data structure and
			 * copy string characters.
			 * @param a_s          the C string to copy into this StringData object
			 * @param a_minbufsize minimum buffer size. Automatically adjusted to
			 *                     length()+1 if a_minbufsize is less then this value.
			 */
	protected: void iinitCopyData(const char* a_s, int a_minbufsize) throw();
			/**
			 * Initialize a new StringData object with a new data structure and
			 * copy string characters.
			 * @param a_s          pointer to a data structure from which the C string is copied
			 * @param a_minbufsize minimum buffer size. Automatically adjusted to
			 *                     length()+1 if a_minbufsize is less then this value.
			 */
	protected: void iinitCopyData(const struct stringdata* a_s, int a_minbufsize) throw()
			{
				iinitCopyData(a_s->string, a_s->length, a_minbufsize);
			}
			/**
			 * Initialize a new StringData object with a new data structure and
			 * copy string characters.
			 * @param a_s          pointer to a String object from which the C string is copied
			 * @param a_minbufsize minimum buffer size. Automatically adjusted to
			 *                     length()+1 if a_minbufsize is less then this value.
			 */
	protected: void iinitCopyData(const String& a_s, int a_minbufsize) throw()
			{
				iinitCopyData(a_s._data, a_minbufsize);
			}

			/**
			 * Unlink the string data. If no one refers to this data, the string
			 * data structure is deleted.
			 * @param a_data  pointer to string data structure
			 */
	protected: void ireleaseData(struct stringdata* a_data) throw()
			{
				if ( (a_data->links--) == 1 )
				{
					delete a_data;
				}
			}
			/**
			 * Unlink the string data associated with this object.
			 * If no one refers to this data, the string
			 * data structure is deleted.
			 */
	protected: void ireleaseData() throw()
			{
				ireleaseData(_data);
			}

			/**
			 * Make a flat copy of a string data into this object. Simply
			 * increments the link counter in the data structure.
			 * @param a_data  the data structure
			 */
	protected: void iflatCopyData(struct stringdata* a_data) throw()
			{
         		iinclinks(a_data); // thread safe, first increment link count
         		_data = a_data;
			}
			/**
			 * Make a flat copy of a StringData object into this object. Simply
			 * increments the link counter in the data structure.
			 * @param a_data  the StringData object
			 */
	protected: void iflatCopyData(const String& a_data) throw()
			{
				iflatCopyData(a_data._data);
			}

			/**
			 * Make a deep copy of a string. Internally does an initCopyData().
			 * @param a_data    the string data structure to copy from
			 * @param a_minbufsize  the min. buffer size
			 */
	protected: void ideepCopyData(const struct stringdata* a_data, int a_minbufsize) throw()
			{
				iinitCopyData(a_data, a_minbufsize);
			}
			/**
			 * Make a deep copy of a string. Internally does an initCopyData().
			 * @param a_data    the StringData object to copy from
			 * @param a_minbufsize  the min. buffer size
			 */
	protected: void ideepCopyData(const String& a_data, int a_minbufsize) throw()
			{
				ideepCopyData(a_data._data, a_minbufsize);
			}

			/**
			 * Unlink this String object from the current string data structure
			 * and init this object with a new string.
			 * @param a_s           the C string to copy
			 * @param a_len         length of the string
			 * @param a_minbufsize  the min. buffer size
			 */
	protected: void iunlinkCopyData(const char* a_s, int a_len, int a_minbufsize) throw()
			{
				struct stringdata* m_data = _data;
				iinitCopyData(a_s, a_len, a_minbufsize);
				ireleaseData(m_data);
			}
			/**
			 * Unlink this String object from the current string data structure
			 * and init this object with a deep copy of this string.
			 * @param a_minbufsize  the min. buffer size
			 */
	protected: void iunlinkCopyData(int a_minbufsize) throw()
			{
				iunlinkCopyData(_data->string, _data->length, a_minbufsize);
			}
			/**
			 * Unlink this StringData object from the current string data structure
			 * and init this object with a deep copy of this string.
			 */
	protected: void iunlinkCopyData() throw()
			{
				iunlinkCopyData(_data->bufsize);
			}

			/**
			 * Assign a C string to this string.
			 * @param a_s   the C string to assign
			 */
	protected: void setCString(const char* a_s) throw();





	public:	int length() const throw() { return _data->length; }
	public: int bufsize() const throw() { return _data->bufsize; }

	public: const char* c_str() const throw() { return _data->string; }
	public: char* str() throw() { return _data->string; }

	public: operator const char * () const { return _data->string; }
	public: operator char * () { return _data->string; }


	public: const String& format(const char* a_fmt, ...) throw();
	public: const String& format(const String& a_fmt, ...) throw();

	public: void vformat(const char* a_fmt, va_list a_va);



			/**
			 * Constructor. Instantiate an emtpy string
			 */
	public: explicit String() throw()
			{
				iflatCopyData((struct stringdata*)&_nullstring);
			}

			/**
			 * Constructor. Instantiate an emtpy string with a minimum buffer size.
			 * @param a_minbufsize   min. buffer size
			 */
	public: explicit String(int a_minbufsize) throw()
			{
				iinitData(0, a_minbufsize);
			}

			/**
			 * Constructor. An internal constructor to allocate space for a string.
			 * @param a_length     string length
			 * @param a_minbufsize min. buffer size
			 */
	public: explicit String(int a_length, int a_minbufsize) throw()
			{
				iinitData(a_length, a_minbufsize);
			}

			//@{
			/**
			 * Constructor. Init a String from a C string with a minimum buffer size. Makes a deep copy
			 * of a_initstring
			 * @param a_initstring     C string
			 * @param a_minbufsize     min. buffer size
			 */
	public: String(const char* a_initstring, int a_minbufsize = 0) throw()
			{
				iinitCopyData(a_initstring, a_minbufsize);
			}
	public: String(const unsigned char* a_initstring, int a_minbufsize = 0) throw()
			{
				iinitCopyData((const char*)a_initstring, a_minbufsize);
			}
			//@}

	public: explicit String(char a_c, int a_minbufsize = 0) throw()
			{
				char buf[2] = { a_c, 0 };
				iinitCopyData(buf, a_minbufsize);
			}

			/**
			 * Constructor. Init a String from a std::string with a minimum buffer size. Makes a deep copy
			 * of a_initstring
			 * @param a_initstring     std::string to copy
			 * @param a_minbufsize     min. buffer size
			 */
	public: explicit String(const std::string& a_initstring, int a_minbufsize = 0) throw()
			{
				iinitCopyData(a_initstring.c_str(), a_minbufsize);
			}

			/**
			 * Copy constructor. Makes a <b>flat</b> copy of a_initstring.
			 * @param a_initstring   the string to copy
			 */
	public: String(const String& a_initstring) throw()
			{
				iflatCopyData(a_initstring);
			}

			/**
			 * Copy constructor. If not deep copy is forced and
			 * a_minbufsize == a_initstring.bufsize(), makes a flat copy,
			 * else makes a deep copy with a new min. buffer size.
			 * @param a_initstring      the string to copy
			 * @param a_minbufsize      min. buffer size
			 * @param a_forcedeepcopy   force a deep copy
			 */
	public: explicit String(const String& a_initstring, int a_minbufsize,
					bool a_forcedeepcopy = false) throw();


			//@{
			/**
			 * Assignment
			 * @return this string
			 */
	public: const String& operator= (const String& a_s) throw()
			{
				if ( (this != &a_s) && (_data != a_s._data) )
				{
					ireleaseData();
					iflatCopyData(a_s);
				}
				return *this;
			}
	public: const String& operator= (const char* a_s) throw()
			{
				setCString(a_s);
				return *this;
			}
	public: const String& operator= (const unsigned char* a_s) throw()
			{
				return operator=((const char* )a_s);
			}
	public: const String& operator= (char a_c) throw()
			{
				char buf[2] = { a_c, 0 };
				return operator=(buf);
			}
	public: const String& operator= (unsigned char a_c) throw()
			{
				char buf[2] = { (char)a_c, 0 };
				return operator=(buf);
			}
			//@}

			/**
			 * Destruktor
			 */
	public: ~String() throw()
			{
				ireleaseData();
			}



			/**
			 * Make a deep copy of this string and returns the copy.
			 * @param a_minbufsize   min. buffer size for the deep copied string
			 * @return the new copied string
			 */
	public: void copy(char* a_s) const throw();
	public: void copy(unsigned char* a_s) const throw() { copy((char*)a_s); }
	public: void copy(char* a_s, unsigned int a_num) const throw();
	public: void copy(unsigned char* a_s, unsigned int a_num) const throw() { copy((char*)a_s, a_num); }
	public: void copy(String& a_s, int a_minbufsize = 0) const throw() { a_s = String(*this, a_minbufsize, true); }
	public: String copy(int a_minbufsize = 0) const throw()
			{
				return String(*this, a_minbufsize, true); // makes a deep copy
			}


			/**
			 * Sets the minimum buffer size. The new buffer size is adjusted so that
			 * the string can fit in if necessary.
			 * @param a_minbufsize   min. buffer size
			 */
	public: void setMinBufSize(int a_minbufsize) throw();

			/**
			 * Internal method: implements the basic insert method.
			 * @param a_s        C string to insert
			 * @param a_len      length of a_s
			 * @param a_pos      position index to insert into this string
			 * @return this string
			 */
	protected: String& iinsert(const char* a_s, int a_len, int a_pos) throw();

			//@{
			/**
			 * Insert a string or character into this string.
			 * @param a_s       the string to insert
			 * @param a_pos     position index to insert into this string
			 * @return this string
			 */
	public: String& insert(const String& a_s, int a_pos) throw();
	public: String& insert(const char* a_s, int a_pos) throw();
	public: String& insert(const unsigned char* a_s, int a_pos) throw() { return insert((const char*)a_s, a_pos); }
	public: String& insert(char a_s, int a_pos) throw();
	public: String& insert(unsigned char a_s, int a_pos) throw() { return insert((char)a_s, a_pos); }
			//@}
			//@{
			/**
			 * Insert a numerical data into this string. The data is converted to a string
			 * according to the NumFormat specified.
			 * @param a_*       the data
			 * @param a_pos     position index to insert into this string
			 * @param a_f       number format
			 * @return this string
			 */
	public: String& insert(short a_short, int a_pos, NumFormat a_f = dec) throw();
	public: String& insert(int a_integer, int a_pos, NumFormat a_f = dec) throw();
	public: String& insert(long a_long, int a_pos, NumFormat a_f = dec) throw();
	public: String& insert(unsigned short a_short, int a_pos, NumFormat a_f = dec) throw();
	public: String& insert(unsigned int a_integer, int a_pos, NumFormat a_f = dec) throw();
	public: String& insert(unsigned long a_long, int a_pos, NumFormat a_f = dec) throw();
			//@}
			//@{
			/**
			 * Insert a numerical data into this string.
			 * @param a_*       the data
			 * @param a_pos     position index to insert into this string
			 * @return this string
			 */
	public: String& insert(float a_float, int a_pos) throw();
	public: String& insert(double a_double, int a_pos) throw();
	public: String& insert(bool a_bool, int a_pos) throw();
			//@}

			//@{
			/**
			 * Aliases for insert() with a_pos = length(). Appends the specified data
			 * to this string.
			 */
	public: String& append(const String& a_s) throw() { return insert(a_s, length()); }
	public: String& append(const char* a_s) throw() { return insert(a_s, length()); }
	public: String& append(const unsigned char* a_s) throw() { return append((const char*)a_s); }
	public: String& append(char a_char) throw() { return insert(a_char, length()); }
	public: String& append(short a_short, NumFormat a_f = dec) throw() { return insert(a_short, length(), a_f); }
	public: String& append(int a_integer, NumFormat a_f = dec) throw() { return insert(a_integer, length(), a_f); }
	public: String& append(long a_long, NumFormat a_f = dec) throw() { return insert(a_long, length(), a_f); }
	public: String& append(unsigned char a_char) throw() { return append((char)a_char); }
	public: String& append(unsigned short a_short, NumFormat a_f = dec) throw() { return insert(a_short, length(), a_f); }
	public: String& append(unsigned int a_integer, NumFormat a_f = dec) throw() { return insert(a_integer, length(), a_f); }
	public: String& append(unsigned long a_long, NumFormat a_f = dec) throw() { return insert(a_long, length(), a_f); }
	public: String& append(float a_float) throw() { return insert(a_float, length()); }
	public: String& append(double a_double) throw() { return insert(a_double, length()); }
	public: String& append(bool a_bool) throw() { return insert(a_bool, length()); }
			//@}

			//@{
			/**
			 * Aliases for append(). Appends the specified data
			 * to this string.
			 */
	public: String& operator<<(const String& a_s) throw() { return append(a_s); }
	public: String& operator<<(const char* a_s) throw() { return append(a_s); }
	public: String& operator<<(const unsigned char *a_s) throw() { return append(a_s); }
	public: String& operator<<(char a_char) throw() { return append(a_char); }
	public: String& operator<<(short a_short) throw() { return append(a_short); }
	public: String& operator<<(int a_integer) throw() { return append(a_integer); }
	public: String& operator<<(long a_long) throw() { return append(a_long); }
	public: String& operator<<(unsigned char a_char) throw() { return append(a_char); }
	public: String& operator<<(unsigned short a_short) throw() { return append(a_short); }
	public: String& operator<<(unsigned int a_integer) throw() { return append(a_integer); }
	public: String& operator<<(unsigned long a_long) throw() { return append(a_long); }
	public: String& operator<<(float a_float) throw() { return append(a_float); }
	public: String& operator<<(double a_double) throw() { return append(a_double); }
	public: String& operator<<(bool a_bool) throw() { return append(a_bool); }
			//@}

			//@{
			/**
			 * Aliases for append(). Appends the specified data
			 * to this string.
			 */
	public: const String& operator+=(const String& a_s) throw() { return append(a_s); }
	public: const String& operator+=(const char* a_s) throw() { return append(a_s); }
	public: const String& operator+=(const unsigned char* a_s) throw() { return append(a_s); }
	public: const String& operator+=(char a_char) throw() { return append(a_char); }
	public: const String& operator+=(short a_short) throw() { return append(a_short); }
	public: const String& operator+=(int a_integer) throw() { return append(a_integer); }
	public: const String& operator+=(long a_long) throw() { return append(a_long); }
	public: const String& operator+=(unsigned char a_char) throw() { return append(a_char); }
	public: const String& operator+=(unsigned short a_short) throw() { return append(a_short); }
	public: const String& operator+=(unsigned int a_integer) throw() { return append(a_integer); }
	public: const String& operator+=(unsigned long a_long) throw() { return append(a_long); }
	public: const String& operator+=(float a_float) throw() { return append(a_float); }
	public: const String& operator+=(double a_double) throw() { return append(a_double); }
	public: const String& operator+=(bool a_bool) throw() { return append(a_bool); }
			//@}

			//@{
			/**
			 * Similar to append(), but returns a new string. Creates a new string
			 * and appends the specified data.
			 * @return new string
			 */
	public: String concat(const String& a_s) const throw() { return String(*this).append(a_s); }
	public: String concat(const char* a_s) const throw() { return String(*this).append(a_s); }
	public: String concat(const unsigned char* a_s) const throw() { return String(*this).append(a_s); }
	public: String concat(char a_char) const throw() { return String(*this).append(a_char); }
	public: String concat(short a_short, NumFormat a_f = dec) const throw() { return String(*this).append(a_short, a_f); }
	public: String concat(int a_integer, NumFormat a_f = dec) const throw() { return String(*this).append(a_integer, a_f); }
	public: String concat(long a_long, NumFormat a_f = dec) const throw() { return String(*this).append(a_long, a_f); }
	public: String concat(unsigned char a_char) const throw() { return String(*this).append(a_char); }
	public: String concat(unsigned short a_short, NumFormat a_f = dec) const throw() { return String(*this).append(a_short, a_f); }
	public: String concat(unsigned int a_integer, NumFormat a_f = dec) const throw() { return String(*this).append(a_integer, a_f); }
	public: String concat(unsigned long a_long, NumFormat a_f = dec) const throw() { return String(*this).append(a_long, a_f); }
	public: String concat(float a_float) const throw() { return String(*this).append(a_float); }
	public: String concat(double a_double) const throw() { return String(*this).append(a_double); }
	public: String concat(bool a_bool) const throw() { return String(*this).append(a_bool); }
			//@}

			//@{
			/**
			 * Aliases for concat(). Creates a new string
			 * and appends the specified data.
			 */
	public: String operator+(const String& a_s) const throw() { return concat(a_s); }
	public: String operator+(const char* a_s) const throw() { return concat(a_s); }
	public: String operator+(const unsigned char* a_s) const throw() { return concat(a_s); }
	public: String operator+(char a_char) const throw() { return concat(a_char); }
	public: String operator+(short a_short) const throw() { return concat(a_short); }
	public: String operator+(int a_integer) const throw() { return concat(a_integer); }
	public: String operator+(long a_long) const throw() { return concat(a_long); }
	public: String operator+(unsigned char a_char) const throw() { return concat(a_char); }
	public: String operator+(unsigned short a_short) const throw() { return concat(a_short); }
	public: String operator+(unsigned int a_integer) const throw() { return concat(a_integer); }
	public: String operator+(unsigned long a_long) const throw() { return concat(a_long); }
	public: String operator+(float a_float) const throw() { return concat(a_float); }
	public: String operator+(double a_double) const throw() { return concat(a_double); }
	public: String operator+(bool a_bool) const throw() { return concat(a_bool); }
			//@}

			//@{
			/**
			 * Test if the specified string is equal to this string.
			 * @param a_s    the string to compare with
			 * @return true if the two strings are equal
			 */
	public: bool operator==(const String& a_s) const throw();
	public: bool operator==(const char* a_s) const throw();
	public: bool operator==(const unsigned char* a_s) const throw() { return operator==((const char*)a_s); }
	public: bool equals(const String& a_s) const throw() { return operator==(a_s); }
	public: bool equals(const char* a_s) const throw() { return operator==(a_s); }
	public: bool equals(const unsigned char* a_s) const throw() { return operator==(a_s); }
	public: bool equals(const char* a_s, unsigned int a_num) const throw();
	public: bool equals(const unsigned char* a_s, unsigned int a_num) const throw() { return equals((const char*)a_s, a_num); }
			//@}

			//@{
			/**
			 * Test if the specified string is not equal to this string.
			 * @param a_s    the string to compare with
			 * @return true if the two strings are not equal
			 */
	public: bool operator!=(const String& a_s) const throw() { return !operator==(a_s); }
	public: bool operator!=(const char* a_s) const throw() { return !operator==(a_s); }
	public: bool operator!=(const unsigned char* a_s) const throw() { return operator!=((const char*)a_s); }
			//@}

			//@{
			/**
			 * Test if the specified string is case insensitive equal to this string.
			 * @param a_s    the string to compare with
			 * @return true if the two strings are equal (case insensitive)
			 */
	public: bool equalsIgnoreCase(const String& a_s) const throw();
	public: bool equalsIgnoreCase(const char* a_s) const throw();
	public: bool equalsIgnoreCase(const unsigned char* a_s) const throw() { return equalsIgnoreCase((const char*)a_s); }
	public: bool equalsIgnoreCase(const char* a_s, unsigned int a_num) const throw();
	public: bool equalsIgnoreCase(const unsigned char* a_s, unsigned int a_num) const throw() { return equalsIgnoreCase((const char*)a_s, a_num); }
			//@}

			//@{
			/**
			 * Compare two string lexicographically.
			 * @param a_s    the second string
			 * @return the result like by strcmp(). -1 if this string is less than a_s,
			 *         0 if this string is equal a_s and 1 if this string is greater
			 *         than a_s.
			 */
	public: int compare(const String& a_s) const throw();
	public: int compare(const char* a_s) const throw();
	public: int compare(const unsigned char* a_s) const throw() { return compare((const char*)a_s); }
	public: int compare(const char* a_s, unsigned int a_num) const throw();
	public: int compare(const unsigned char* a_s, unsigned int a_num) const throw() { return compare((const char*)a_s, a_num); }
			//@}

			//@{
			/** Aliases for compare()
			 * @param a_s    the second string
			 * @return true if the operator is true
			 */
	public: bool operator>(const String& a_s) const throw() { return (compare(a_s) > 0); }
	public: bool operator>(const char* a_s) const throw() { return (compare(a_s) > 0); }
	public: bool operator>(const unsigned char* a_s) const throw() { return (compare(a_s) > 0); }

	public: bool operator>=(const String& a_s) const throw() { return (compare(a_s) >= 0); }
	public: bool operator>=(const char* a_s) const throw() { return (compare(a_s) >= 0); }
	public: bool operator>=(const unsigned char* a_s) const throw() { return (compare(a_s) >= 0); }

	public: bool operator<(const String& a_s) const throw() { return (compare(a_s) < 0); }
	public: bool operator<(const char* a_s) const throw() { return (compare(a_s) < 0); }
	public: bool operator<(const unsigned char* a_s) const throw() { return (compare(a_s) < 0); }

	public: bool operator<=(const String& a_s) const throw() { return (compare(a_s) <= 0); }
	public: bool operator<=(const char* a_s) const throw() { return (compare(a_s) <= 0); }
	public: bool operator<=(const unsigned char* a_s) const throw() { return (compare(a_s) <= 0); }
			//@}

			//@{
			/**
			 * Compare two string lexicographically ignoring case.
			 * @param a_s    the second string
			 * @return the result like by strcmp(). -1 if this string is less than a_s,
			 *         0 if this string is equal a_s and 1 if this string is greater
			 *         than a_s.
			 */
	public: int compareIgnoreCase(const String& a_s) const throw();
	public: int compareIgnoreCase(const char* a_s) const throw();
	public: int compareIgnoreCase(const unsigned char* a_s) const throw() { return compareIgnoreCase((const char*)a_s); }
	public: int compareIgnoreCase(const char* a_s, unsigned int a_num) const throw();
	public: int compareIgnoreCase(const unsigned char* a_s, unsigned int a_num) const throw() { return compareIgnoreCase((const char*)a_s, a_num); }
			//@}

			/**
			 * Get a substring of this string.
			 * @param a_start     start index
			 * @param a_end       end index, points to the next character that should not
			 *                    be included in the substring. -1 means till to the end
			 *                    of the string.
			 * @return the substring as a new string
			 */
	public: String substring(int a_start, int a_end = -1) const throw();
			/**
			 * Get a substring of this string containing the left most n characters.
			 * Equal to substring(0, a_len).
			 * @param a_len   number of characters
			 * @return the substring as a new string
			 */
	public: String right(int a_len) const throw() { return substring(0, a_len); }
			/**
			 * Get a substring of this string.
			 * Equal to substring(a_start, a_start+a_len).
			 * @param a_start start index
			 * @param a_len   number of characters
			 * @return the substring as a new string
			 */
	public: String mid(int a_start, int a_len) const throw() { return substring(a_start, a_start + a_len); }
			/**
			 * Get a substring of this string containing the right most n characters.
			 * Equal to substring(length()-a_len, -1).
			 * @param a_len   number of characters
			 * @return the substring as a new string
			 */
	public: String left(int a_len) const throw() { return substring(length() - a_len, -1); }

			/**
			 * Convert this string to lower case.
			 * @return this string
			 */
	public: String& lowerCase() throw();

			/**
			 * Convert a copy of this string to lower case and get it.
			 * @return the converted string
			 */
	public: String getLowerCase() const throw()
			{
				return String(*this).lowerCase();
			}

			/**
			 * Convert this string to upper case.
			 * @return this string
			 */
	public: String& upperCase() throw();

			/**
			 * Convert a copy of this string to lower case and get it.
			 * @return the converted string
			 */
	public: String getUpperCase() const throw()
			{
				return String(*this).upperCase();
			}

			/**
			 * Convert this string into a "long" value.
			 * @param a_base   base of the conversion like passed to strtol(). If 0, the
			 *                 base is autoselected from the string. "0x" prefix means hexadecimal,
			 *                 "0" prefix means octal.
			 * @return the value
			 * @exception NumberFormatException if this string can not be converted to a long value
			 */
	public: long toLong(int a_base = 0) const; //throw(NumberFormatException)
			/**
			 * Convert this string into a "unsigned long" value.
			 * @param a_base   base of the conversion like passed to strtol(). If 0, the
			 *                 base is autoselected from the string. "0x" prefix means hexadecimal,
			 *                 "0" prefix means octal.
			 * @return the value
			 * @exception NumberFormatException if this string can not be converted to a
			 *                                  unsigned long value
			 */
	public: unsigned long toUnsignedLong(int a_base = 0) const; //throw(NumberFormatException)
			/**
			 * Convert this string into a "double" value.
			 * @param a_base   base of the conversion like passed to strtod().
			 * @return the value
			 * @exception NumberFormatException if this string can not be converted to a double value
			 */
	public: double toDouble() const; //throw(NumberFormatException)

			/**
			 * Strip whitespace characters (defined by isspace()) from the beginning
			 * of this string.
			 * @return this string
			 */
	public: String& stripWhiteSpaceAtBegin() throw();
			/**
			 * Strip whitespace characters (defined by isspace()) from the end
			 * of this string.
			 * @return this string
			 */
	public: String& stripWhiteSpaceAtEnd() throw();
			/**
			 * Strip whitespace characters (defined by isspace()) from the beginning
			 * and the end of this string.
			 * @return this string
			 */
	public: String& stripWhiteSpace() throw()
			{
				stripWhiteSpaceAtBegin();
				stripWhiteSpaceAtEnd();
				return *this;
			}

			//@{
			/**
			 * Finds the first occurance of a string in this string starting at index
			 * a_start. If a_start < 0, a_start == 0 is assumed. If a_start >= length(),
			 * nothing will be found.
			 * @param a_s      the string to search for
			 * @param a_start  the start index
			 * @return if found the index of the element in the string,
			 *          else -1 if nothing is found
			 */
	public: int indexOf(char a_s, int a_start = 0) const throw();
	public: int indexOf(unsigned char a_s, int a_start = 0) const throw() { return indexOf((char)a_s, a_start); }
	public: int indexOf(const String& a_s, int a_start = 0) const throw();
	public: int indexOf(const char* a_s, int a_start = 0) const throw();
	public: int indexOf(const unsigned char* a_s, int a_start = 0) const throw() { return indexOf((const char*)a_s, a_start); }
			//@}

	public: bool contains(char a_s) const throw() { return indexOf(a_s) >= 0; }
	public: bool contains(unsigned char a_s) const throw() { return indexOf(a_s) >= 0; }
	public: bool contains(const String& a_s) const throw() { return indexOf(a_s) >= 0; }
	public: bool contains(const char* a_s) const throw() { return indexOf(a_s) >= 0; }
	public: bool contains(const unsigned char* a_s) const throw() { return indexOf(a_s) >= 0; }

			//@{
			/**
			 * Finds the last occurance of a string in this string
			 * @param a_s      the string to search for
			 * @return if found the index of the element in the string,
			 *          else -1 if nothing is found
			 */
	public: int lastIndexOf(char a_s) const throw();
	public: int lastIndexOf(unsigned a_s) const throw() { return lastIndexOf((char)a_s); }
	public: int lastIndexOf(const String& a_s) const throw();
	public: int lastIndexOf(const char* a_s) const throw();
	public: int lastIndexOf(const unsigned char* a_s) const throw() { return lastIndexOf((const char*)a_s); }
			//@}

			/**
			 * Truncate this string to length a_length. If a_length >= length(), nothing
			 * is done.
			 * @param a_length   the new length
			 * @return this string
			 */
	public: String& truncate(int a_length) throw();
	public: String& clear() throw() { return truncate(0); }
			/** Create a copy of this string and truncate it.
			 * @param a_length  the new length
			 * @return the new truncated string
			 */
	public: String getTruncated(int a_length) const throw() { return String(*this).truncate(a_length); }

			//@{
			/**
			 * Test if this string begin with the specified string.
			 * @param a_s      the string to test for
			 * @return true if this string starts with a_s
			 */
	public: bool startsWith(char a_s) const throw();
	public: bool startsWith(unsigned char a_s) const throw() { return startsWith((char)a_s); }
	public: bool startsWith(const String& a_s) const throw();
	public: bool startsWith(const char* a_s) const throw();
	public: bool startsWith(const unsigned char* a_s) const throw() { return startsWith((const char*)a_s); }

	public: bool startsWithIgnoreCase(char a_s) const throw();
	public: bool startsWithIgnoreCase(unsigned char a_s) const throw() { return startsWithIgnoreCase((char)a_s); }
	public: bool startsWithIgnoreCase(const String& a_s) const throw();
	public: bool startsWithIgnoreCase(const char* a_s) const throw();
	public: bool startsWithIgnoreCase(const unsigned char* a_s) const throw() { return startsWithIgnoreCase((const char*)a_s); }
	public: bool startsWithIgnoreCase(const std::string& a_s) const throw() { return startsWithIgnoreCase(a_s.c_str()); }
			//@}

			//@{
			/**
			 * Test if this string ends with the specified string.
			 * @param a_s      the string to test for
			 * @return true if this string starts with a_s
			 */
	public: bool endsWith(char a_s) const throw();
	public: bool endsWith(unsigned char a_s) const throw() { return endsWith((char)a_s); }
	public: bool endsWith(const char* a_s) const throw();
	public: bool endsWith(const unsigned char* a_s) const throw() { return endsWith((const char*)a_s); }
	public: bool endsWith(const String& a_s) const throw();

	public: bool endsWithIgnoreCase(char a_s) const throw();
	public: bool endsWithIgnoreCase(unsigned char a_s) const throw() { return endsWithIgnoreCase((char)a_s); }
	public: bool endsWithIgnoreCase(const char* a_s) const throw();
	public: bool endsWithIgnoreCase(const unsigned char* a_s) const throw() { return endsWithIgnoreCase((const char*)a_s); }
	public: bool endsWithIgnoreCase(const String& a_s) const throw();
	public: bool endsWithIgnoreCase(const std::string& a_s) const throw() { return endsWithIgnoreCase(a_s.c_str()); }
			//@}

			/**
			 * Remove characters of this string.
			 * @param a_start     start index
			 * @param a_end       end index, points to the next character that should not
			 *                    be removed of the string. -1 means till to the end
			 *                    of the string.
			 * @return this string
			 */
	public: String& remove(int a_start, int a_end = -1) throw();
			/**
			 * Creates a copy of this string and removes characters of this new string.
			 * @param a_start     start index
			 * @param a_end       end index, points to the next character that should not
			 *                    be removed of the string. -1 means till to the end
			 *                    of the string.
			 * @return the new string
			 */
	public: String getRemoved(int a_start, int a_end = -1) const throw() { return String(*this).remove(a_start, a_end); }

			/**
			 * Replace characters of this string. All occurances of a_1 in the string
			 * will be replaced with a_2.
			 * @param a_1     the character to replace
			 * @param a_2     the character that replaces a_1
			 * @return this string
			 */
	public: String& replace(char a_1, char a_2) throw();
	public: String& replace(const String& a_1, const String& a_2) throw();
			/**
			 * Create a copy of this string and replace characters of the new string.
			 * All occurances of a_1 in the new string will be replaced with a_2.
			 * @param a_1     the character to replace
			 * @param a_2     the character that replaces a_1
			 * @return the new string
			 */
	public: String getReplaced(char a_1, char a_2) const throw() { return String(*this).replace(a_1, a_2); }

			/**
			 * Erase characters of this string. All characters specified by a_c are deleted
			 * from this string.
			 * @param a_c     the character to erase
			 * @return this string
			 */
	public: String& erase(char a_c) throw();
			/**
			 * Erase characters of this string. All characters specified by a_c are deleted
			 * from this string.
			 * @param a_c     the character to erase
			 * @return this string
			 */
	public: String getErased(char a_c) const throw() { return String(*this).erase(a_c); }

			/**
			 * Get the character at index a_index;
			 * @param a_index  the index position
			 * @return the character
			 * @exception IndexOutOfBoundsException if the index is invalid
			 */
	public: char charAt(int a_index) const; //throw(IndexOutOfBoundsException)
			/// An alias for charAt()
	public: char operator[] (int a_index) const throw()
			{
				return _data->string[a_index];
			}
	public: char& operator[] (int a_index) throw()
			{
				return _data->string[a_index];
			}

	public: int nextToken(int a_index, String& a_token,
						const char* a_linecomment = 0,
						const char* a_quotes = 0,
						bool a_use_escapes = false,
						const char* a_sepchars = 0) const;


	public: uint32 hashcode() const throw();

	public: bool isEmpty() const throw() { return _data->length == 0; }
	public: bool isNotEmpty() const throw() { return _data->length > 0; }

	public: bool containsAlpha() const throw();
	public: bool containsAlphaNum() const throw();
	public: bool containsDigits() const throw();

	public: int count(char a_c) const throw();
	
	public: bool hasNextToken(int a_index,
						const char* a_linecomment = 0,
						const char* a_sepchars = 0) const;
};

/**
 * Output a string to a std::ostream
 * @param a_s    the string to output
 */
inline std::ostream& operator<<(std::ostream& a_stream, const String& a_s)
{
	return (a_stream << a_s.c_str());
}


inline String operator+ (const char* a_s1, const String& a_s2)
{
	return String(a_s1) << a_s2;
}

inline String operator<< (const char* a_s1, const String& a_s2)
{
	return String(a_s1) << a_s2;
}

} // namespace


#endif
