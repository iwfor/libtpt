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

#ifndef _notboost_smartptr_h
#define _notboost_smartptr_h

#include <memory>
#include <iostream>

namespace notboost {

template <typename T>
struct sp_refcount_s {
	unsigned count;
	T* obj;
	sp_refcount_s() : count(0)	{ }
	explicit sp_refcount_s(T* p) : count(1), obj(p) { }
	~sp_refcount_s() { if (obj) delete obj; }
	unsigned release()	{ return --count; }
};

template <typename T>
class shared_ptr {
private:
	mutable sp_refcount_s<T>* ptr;
public:

	typedef shared_ptr<T>* pointer;
	typedef const shared_ptr<T>* const_pointer;
	typedef shared_ptr<T>& reference;
	typedef const shared_ptr<T>& const_reference;

	shared_ptr<T>() : ptr(0) { }
	shared_ptr<T>(const shared_ptr<T>& sp) : ptr(sp.ptr)
	{ if (ptr) ++ptr->count; }
	shared_ptr<T>(const T* p)
	{
		try {
			ptr = new sp_refcount_s<T>(const_cast<T*>(p));
		} catch (...) {
			delete p;
			throw;
		}
	}
	shared_ptr<T>(std::auto_ptr<T>& ap)
	{
		ptr = new sp_refcount_s<T>(ap.get());
		ap.release();
	}
	~shared_ptr<T>()
	{
		if (ptr && !--ptr->count) delete ptr;
	}
	T* get() const {
		return ptr ? ptr->obj : 0;
	}
	shared_ptr<T>& operator=(const shared_ptr<T>& p)
	{
		if (ptr && !--ptr->count) delete ptr;
		ptr = p.ptr;
		if (ptr) ++ptr->count;
		return *this;
	}
	shared_ptr<T>& operator=(T* p)
	{
		std::auto_ptr<T> ap(p);
		if (ptr && !--ptr->count) delete ptr;
		ptr = 0;	// just in cast new throws
		if (p) ptr = new sp_refcount_s<T>(p);
		else ptr = 0;	// allow ptr to be cleared when p is 0
		ap.release();
		return *this;
	}
	shared_ptr<T>& operator=(std::auto_ptr<T>& ap)
	{
		if (ptr && !--ptr->count) delete ptr;
		ptr = 0;	// just in cast new throws
		if (p) ptr = new sp_refcount_s<T>(p);
		else ptr = 0;	// allow ptr to be cleared when p is 0
		ap.release();
		return *this;
	}
};

} // end namespace notboost

#endif // _notboost_smartptr_h
