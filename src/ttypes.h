/*
 * types.h
 *
 * Define special extended types
 *
 * $Id$
 *
 */

#ifndef __extendedtypes_types_h
#define __extendedtypes_types_h

#include <set>

namespace ExtendedTypes {

template<typename T = char> struct ChrSet {
	std::set< T > names;
	ChrSet(T*);
	ChrSet(const ChrSet<T>& cs) : names(cs.names) {}

	// Assignment operator
	ChrSet& operator=(const ChrSet<T>& cs)
		{ names = cs.names; return *this }

	// Boolean operator
	friend bool operator==(T a, const ChrSet<T>& b)
		{ return b.names.find(a) != b.names.end(); }
	friend bool operator==(const ChrSet<T>& a, T b)
		{ return a.names.find(b) != a.names.end(); }
	friend bool operator!=(T a, const ChrSet<T>& b)
		{ return b.names.find(a) == b.names.end(); }
	friend bool operator!=(const ChrSet<T>& a, T b)
		{ return a.names.find(b) == a.names.end(); }
};


template<typename T = char> struct ChrRange {
	T lower;
	T upper;
	ChrRange(T l, T u) : lower(l), upper(u) {}

	operator ChrSet<T> ();
	friend bool operator==(T a, const ChrSet<T>& b)
		{ return b.names.find(a) != b.names.end(); }
	friend bool operator==(const ChrSet<T>& a, T b)
		{ return a.names.find(b) != a.names.end(); }
};

} // end namespace ExtendedTypes

#include "ttypes.inl"

#endif // __extendedtypes_types_h
