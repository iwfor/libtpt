/*
 * smartptr.h
 *
 * A shared pointer template implementation.
 *
 */

/*
 * Written by Isaac W. Foraker (isaac@tazthecat.net)
 *
 * Where's the copyright?
 *	The smart pointer types are fairly universal constructs, so it
 *	does not make any sense to copyright this code, since much prior
 *	art exists.  Feel free to use this class, rip it apart, write a
 *	better version, or just avoid it.
 *
 * What about the boost smart pointers?
 *	The boost::shared_ptr<T> was causing compiler errors and crashes
 *	on the Windows platform.  Until these issues can be resolved,
 *	my implementation of the shared pointer should be good enough to
 *	compile LibTPT on most C++ platforms.
 *
 */

#ifndef __notboost_smartptr_h
#define __notboost_smartptr_h

#include <memory>

namespace notboost {

template <typename T>
class refcounter {
private:
	unsigned count;
	T* obj;
public:
	refcounter() : count(0)	{ }
	explicit refcounter(T* p) : count(1), obj(p) { }
	~refcounter() { delete obj; }

	unsigned grab()		{ return ++count; }
	unsigned release()	{ return --count; }
	unsigned getcount()	{ return count; }
	T* get() 			{ return obj; }
};

template <typename T>
class shared_ptr {
private:
	mutable refcounter<T>* ptr;
public:

	typedef shared_ptr<T>* pointer;
	typedef const shared_ptr<T>* const_pointer;
	typedef shared_ptr<T>& reference;
	typedef const shared_ptr<T>& const_reference;

	shared_ptr<T>() : ptr(0) { }
	shared_ptr<T>(const shared_ptr<T>& sp) : ptr(sp.ptr) {
		if (ptr) ptr->grab(); }
	shared_ptr<T>(const T* p) : ptr(new refcounter<T>(const_cast<T*>(p)))
	{
		try {
			ptr = new refcounter<T>(const_cast<T*>(p));
		} catch (...) {
			delete p;
			throw;
		}
	}
	shared_ptr<T>(std::auto_ptr<T>& ap)
	{
		ptr = new refcounter<T>(ap.get());
		ap.release();
	}
	~shared_ptr<T>()
	{
		if (ptr && !ptr->release()) delete ptr;
	}
	T* get() const {
		return ptr ? ptr->get() : 0;
	}
	shared_ptr<T>& operator=(const shared_ptr<T>& p)
	{
		if (ptr && !ptr->release()) delete ptr;
		ptr = p.ptr;
		if (ptr) ptr->grab();
		return *this;
	}
	shared_ptr<T>& operator=(T* p)
	{
		std::auto_ptr<T> ap(p);
		if (ptr && !ptr->release()) delete ptr;
		ptr = 0;	// just in cast new throws
		if (p) ptr = new refcounter<T>(p);
		else ptr = 0;	// allow ptr to be cleared when p is 0
		ap.release();
		return *this;
	}
	shared_ptr<T>& operator=(std::auto_ptr<T>& ap)
	{
		if (ptr && !ptr->release()) delete ptr;
		ptr = 0;	// just in cast new throws
		if (p) ptr = new refcounter<T>(p);
		else ptr = 0;	// allow ptr to be cleared when p is 0
		ap.release();
		return *this;
	}
};

} // end namespace notboost

#endif // __notboost_smartptr_h
