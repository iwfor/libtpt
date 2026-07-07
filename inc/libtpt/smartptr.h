/*
 * smartptr.h
 *
 * A shared pointer template implementation.
 *
 * Written by Isaac W. Foraker (isaac at noscience dot net)
 *
 * Where's the copyright?
 *	The smart pointer types are fairly universal constructs, so it
 *	does not make any sense to copyright this code, since much prior
 *	art exists.  Feel free to use this class, rip it apart, write a
 *	better version, or just avoid it.
 *
 * Implementation note:
 *	This is now a thin wrapper around std::shared_ptr.  The standard
 *	library provides a thread-safe (atomically reference-counted)
 *	control block, which the previous hand-rolled implementation did
 *	not: its plain unsigned counter raced when a pointer graph was
 *	shared across threads, allowing double-free or leaks.  Delegating
 *	to std::shared_ptr fixes that.
 *
 *	The only reason this wrapper still exists is to preserve two
 *	conveniences the rest of LibTPT relies on and that std::shared_ptr
 *	deliberately does not offer:
 *	  - implicit construction from a raw T* (e.g. container.push_back(new T))
 *	  - assignment from a raw T* (e.g. map[key] = new T)
 *	std::shared_ptr makes the raw-pointer constructor explicit and has
 *	no raw-pointer assignment, to discourage accidental ownership bugs.
 *	We re-add both so existing call sites compile unchanged.
 *
 */

#ifndef include_notboost_smartptr_h
#define include_notboost_smartptr_h

#include <memory>

namespace notboost {

template <typename T>
class shared_ptr : public std::shared_ptr<T> {
	typedef std::shared_ptr<T> base_type;
public:

	typedef shared_ptr<T>* pointer;
	typedef const shared_ptr<T>* const_pointer;
	typedef shared_ptr<T>& reference;
	typedef const shared_ptr<T>& const_reference;

	shared_ptr() {}

	// Implicitly take ownership of a raw pointer (std::shared_ptr makes
	// this explicit; we keep it implicit for LibTPT's call sites).
	shared_ptr(T* p) : base_type(p) {}

	// Copy and move construction/assignment, get(), operator*,
	// operator->, and the (explicit) bool conversion are all inherited
	// from std::shared_ptr, including its atomic reference counting.
	// Assigning another shared_ptr uses the compiler-generated copy/move
	// assignment, which correctly manages the shared control block and is
	// self-assignment safe.

	// Assign ownership of a raw pointer (not provided by std::shared_ptr).
	shared_ptr& operator=(T* p)
	{
		base_type::reset(p);
		return *this;
	}
};

} // end namespace notboost

#endif // include_notboost_smartptr_h
