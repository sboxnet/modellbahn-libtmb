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


#ifndef _TMB_LANG_TMB_H_
# define _TMB_LANG_TMB_H_

#include <stdio.h>
#include <stdint.h>

#include <tmb/thread/AtomicInt.h>


typedef uint8_t  	  byte; ///< a 8 bit unsigned type
typedef int8_t		  int8;
typedef uint8_t		 uint8;

typedef int16_t      int16; ///< a 16 bit signed value
typedef uint16_t	uint16; ///< a 16 bit unsigned value

typedef int32_t      int32; ///< a 32 bit signed value
typedef uint32_t	uint32; ///< a 32 bit unsigned value

typedef int64_t		 int64;	///< a 64 bit signed value
typedef uint64_t	uint64; ///< a 64 bit unsigned value


typedef unsigned short ushort;

typedef unsigned int uint;

typedef unsigned long ulong;



namespace tmb {

enum AutoDelete
{
	AutoDelete_NONE		= 0,
	AutoDelete_DESTRUCT = 0x0001,
	AutoDelete_REMOVE   = 0x0002,
	AutoDelete_ALL      = (AutoDelete_DESTRUCT|AutoDelete_REMOVE),
};


#define no_copy_method(_c) private: _c(const _c&);
#define no_assign_method(_c) private: void operator=(const _c&);
#define no_copy_assign_method(_c)  no_copy_method(_c) no_assign_method(_c)

#define def_class(_c)         class _c {
#define def_subclass(_c, _s)  class _c : public _s { private: typedef _s super;
#define end_class             };

#define def_class_nca(_c)	     def_class(_c)  no_copy_assign_method(_c)
#define def_subclass_nca(_c, _s) def_subclass(_c, _s) no_copy_assign_method(_c)


template<class T>
class ObjPtr
{
	protected:	T*	c_ptr;

	public: explicit ObjPtr() : c_ptr(0) {}

	public: explicit ObjPtr(T* a_ptr) : c_ptr(a_ptr) {}

	private: explicit ObjPtr(const ObjPtr<T>&);

	public: ~ObjPtr()
			{
				delete c_ptr;
			}

	private: const ObjPtr<T>& operator= (const ObjPtr<T>&);


	public: T* operator-> ()
			{
				return c_ptr;
			}

	public: T& operator* ()
			{
				return *c_ptr;
			}

	public: operator T* ()
			{
				return c_ptr;
			}

	public: bool operator! ()
			{
				return (c_ptr == 0);
			}

	public: bool operator== (T* a_ptr)
			{
				return c_ptr == a_ptr;
			}

	public: bool operator!= (T* a_ptr)
			{
				return c_ptr != a_ptr;
			}

	public: void release()
			{
				delete c_ptr;
				c_ptr = 0;
			}

	public: T* detach()
			{
				T* p = c_ptr;
				c_ptr = 0;
				return p;
			}

	public: void replace(T* a_ptr)
			{
				delete c_ptr;
				c_ptr = a_ptr;
			}
	public: void assign(T* a_ptr)
			{
				delete c_ptr;
				c_ptr = a_ptr;
			}
};


class ObjRefElem
{
	private:
		AtomicInt	_objref_links;

	public: ObjRefElem() : _objref_links(0) {}
	public: virtual ~ObjRefElem() {}

	public: void objref_addRef()
			{
				_objref_links++;
			}
	public: void objref_release()
			{
				if (_objref_links-- <= 1)
					delete this;
			}
};

template<class T>
class ObjRef
{
	protected:	T*	_ptr;

	public: explicit ObjRef() : _ptr(0) {}

	public: explicit ObjRef(T* a_ptr) : _ptr(a_ptr)
			{
				if (_ptr)
					_ptr->objref_addRef();
			}

	public: ObjRef(const ObjRef<T>& a_ref)
			{
				_ptr = a_ref._ptr;
				if (_ptr)
					_ptr->objref_addRef();
			}

	public: ~ObjRef()
			{
				if (_ptr)
					_ptr->objref_release();
			}

	public: const ObjRef<T>& operator= (const ObjRef<T>& a_ref)
			{
				T* p = a_ref._ptr;
				if (p)
					p->objref_addRef();
				if (_ptr)
					_ptr->objref_release();
				_ptr = p;
				return *this;
			}
	public: const ObjRef<T>& operator= (T* a_ptr)
			{
				if (a_ptr)
					a_ptr->objref_addRef();
				if (_ptr)
					_ptr->objref_release();
				_ptr = a_ptr;
				return *this;
			}

	public: void attach(T* a_ptr)
			{
				if (_ptr)
					_ptr->objref_release();
				_ptr = a_ptr;
			}
	public: T* detach()
			{
				ObjRefElem* p = _ptr;
				_ptr = 0;
				return (T*)p;
			}
	public: void clear()
			{
				attach(0);
			}

	public: T* operator-> () const
			{
				return (T*)_ptr;
			}

	public: T& operator* ()
			{
				return *(T*)_ptr;
			}

	public: operator T* () const
			{
				return (T*)_ptr;
			}

	public: bool operator! () const
			{
				return (_ptr == 0);
			}

	public: bool operator== (T* a_ptr) const
			{
				return _ptr == a_ptr;
			}

	public: bool operator!= (T* a_ptr) const
			{
				return _ptr != a_ptr;
			}
};



class ObjCallbackInterface
{
	public: virtual ~ObjCallbackInterface() {}
	public: virtual void exec() = 0;
};
template<typename Par>
class ObjCallbackInterface1
{
	public: virtual ~ObjCallbackInterface1() {}
	public: virtual void exec(Par) = 0;
};
template<typename Par, typename Par2>
class ObjCallbackInterface2
{
	public: virtual ~ObjCallbackInterface2() {}
	public: virtual void exec(Par, Par2) = 0;
};
template<typename Par, typename Par2, typename Par3>
class ObjCallbackInterface3
{
	public: virtual ~ObjCallbackInterface3() {}
	public: virtual void exec(Par, Par2, Par3) = 0;
};


template<class T>
class ObjCallback : public ObjCallbackInterface
{
	public: typedef void (T::* Method)();
	protected:
		T*		_obj;
		Method	_method;

	public: ObjCallback() : _obj(0) {}
	public: ObjCallback(T* a_obj, Method a_method) : _obj(a_obj), _method(a_method) {}

	public: void set(T* a_obj, Method a_method)
			{
				_obj = a_obj;
				_method = a_method;
			}

	public: T* obj() const { return _obj; }
	public: bool isValid() const { return _obj != 0; }

	public: void exec()
			{
				if (_obj)
					(_obj->*_method)();
			}
};
template<class T, typename Par>
class ObjCallback1 : public ObjCallbackInterface1<Par>
{
	public: typedef void (T::* Method)(Par);
	protected:
		T*		_obj;
		Method	_method;

	public: ObjCallback1() : _obj(0) {}
	public: ObjCallback1(T* a_obj, Method a_method) : _obj(a_obj), _method(a_method) {}

	public: void set(T* a_obj, Method a_method)
			{
				_obj = a_obj;
				_method = a_method;
			}

	public: T* obj() const { return _obj; }
	public: bool isValid() const { return _obj != 0; }

	public: void exec(Par a_p)
			{
				if (_obj)
					(_obj->*_method)(a_p);
			}
};
template<class T, typename Par, typename Par2>
class ObjCallback2 : public ObjCallbackInterface2<Par,Par2>
{
	public: typedef void (T::* Method)(Par, Par2);
	protected:
		T*		_obj;
		Method	_method;

	public: ObjCallback2() : _obj(0) {}
	public: ObjCallback2(T* a_obj, Method a_method) : _obj(a_obj), _method(a_method) {}

	public: void set(T* a_obj, Method a_method)
			{
				_obj = a_obj;
				_method = a_method;
			}

	public: T* obj() const { return _obj; }
	public: bool isValid() const { return _obj != 0; }

	public: void exec(Par a_p, Par2 a_p2)
			{
				if (_obj)
					(_obj->*_method)(a_p, a_p2);
			}
};
template<class T, typename Par, typename Par2, typename Par3>
class ObjCallback3 : public ObjCallbackInterface3<Par,Par2,Par3>
{
	public: typedef void (T::* Method)(Par, Par2, Par3);
	protected:
		T*		_obj;
		Method	_method;

	public: ObjCallback3() : _obj(0) {}
	public: ObjCallback3(T* a_obj, Method a_method) : _obj(a_obj), _method(a_method) {}

	public: void set(T* a_obj, Method a_method)
			{
				_obj = a_obj;
				_method = a_method;
			}

	public: T* obj() const { return _obj; }
	public: bool isValid() const { return _obj != 0; }

	public: void exec(Par a_p, Par2 a_p2, Par3 a_p3)
			{
				if (_obj)
					(_obj->*_method)(a_p, a_p2, a_p3);
			}
};

#define newObjCb(_class, _instance, _method) \
	new ObjCallback<_class>((_instance), &_class::_method)

#define newObjCb1(_class, _instance, _method, _p1) \
	new ObjCallback1<_class,_p1>((_instance), &_class::_method)

#define newObjCb2(_class, _instance, _method, _p1, _p2) \
	new ObjCallback2<_class,_p1,_p2>((_instance), &_class::_method)

#define newObjCb3(_class, _instance, _method, _p1, _p2, _p3) \
	new ObjCallback3<_class,_p1,_p2,_p3>((_instance), &_class::_method)



struct Flags
{
	uint	flags;

	Flags() {}
	Flags(uint a_f) : flags(a_f) {}
	Flags(const Flags& a_f) : flags(a_f.flags) {}

	void operator= (uint a_f) { flags = a_f; }
	void operator= (const Flags& a_f) { flags = a_f.flags; }

	void set(uint a_f) { flags |= a_f; }
	void clear(uint a_f) { flags &= ~a_f; }
	void set(uint a_f, bool a_on)
		{
			if (a_on)
				flags |= a_f;
			else
				flags &= ~a_f;
		}

	bool has(uint a_f) const { return (flags & a_f); }

	operator uint () const { return flags; }

	bool operator== (uint a_f) const { return (flags == a_f); }
	bool operator== (const Flags& a_f) const { return flags == a_f.flags; }
};





#ifdef __GNUC__
# define ATTRIBUTE_FORMAT_PRINTF(_a,_b) __attribute__((format (printf, _a, _b)))
#else
# define ATTRIBUTE_FORMAT_PRINTF(_a,_b)
#endif


// log functions / macros --------------------------------------

extern void logFatal(const char* a_file, int a_line, const char* a_fmt, ...) ATTRIBUTE_FORMAT_PRINTF(3,4);
extern void logError(const char* a_file, int a_line, const char* a_fmt, ...) ATTRIBUTE_FORMAT_PRINTF(3,4);
extern void logWarn(const char* a_file, int a_line, const char* a_fmt, ...) ATTRIBUTE_FORMAT_PRINTF(3,4);
extern void logInfo(const char* a_fmt, ...) ATTRIBUTE_FORMAT_PRINTF(1,2);
extern void logDebug(const char* a_file, int a_line, uint32 a_mask, const char* a_fmt, ...) ATTRIBUTE_FORMAT_PRINTF(4,5);

#define LogFATAL(...) tmb::logFatal(__FILE__,__LINE__,__VA_ARGS__)

#define LogERROR(...) tmb::logError(__FILE__,__LINE__,__VA_ARGS__)

#define LogWARN(...) tmb::logWarn(__FILE__,__LINE__,__VA_ARGS__)

#define LogINFO(...) tmb::logInfo(__VA_ARGS__)



#ifdef DEBUG
# define LogDEBUG(...) tmb::logDebug(__FILE__,__LINE__,0,__VA_ARGS__)
# define LogDEBUGm(_m,...) tmb::logDebug(__FILE__,__LINE__,(_m),__VA_ARGS__)
#else
# define LogDEBUG(...)
# define LogDEBUGm(_m,...)
#endif


#define LogASSERT(_x, ...) \
	do { if ( !(_x) ) { LogERROR(__VA_ARGS__); } } while(0)

} // namespace

#endif // _TMB_LANG_GENERAL_H_
