/*
 * object.h
 *
 * $Id$
 *
 */

/*
 * Copyright (C) 2002 Isaac W. Foraker (isaac*nospam*@tazthecat.net)
 * All Rights Reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of the Author nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __tptlib_object_impl_h
#define __tptlib_object_impl_h

#include <string>
#include <map>
#include <vector>

namespace TPTLib {

/*
 * enumeration of types of objects the object_t structure may hold.
 */
enum object_types {
	obj_not_alloc = 0,
	obj_scalar,
	obj_array,
	obj_hash
};

struct object_t {
	typedef std::vector< object_t > ObjectArrayType;
	typedef std::map< std::string, object_t > ObjectHashType;

	object_types type;
	union {
		std::string* str;
		ObjectArrayType* array;
		ObjectHashType* hash;
	} u;

	// Basic ctor
	object_t() : type(obj_not_alloc) {}

	// Copy ctors
	explicit object_t(const std::string& str);
	explicit object_t(const ObjectArrayType& array);
	explicit object_t(const ObjectHashType& hash);

	// dtor
	~object_t() { unalloc(); }

	// Various assignments
	object_t& operator=(const std::string& str);
	object_t& operator=(const ObjectArrayType& array);
	object_t& operator=(const ObjectHashType& hash);
	object_t& operator=(const object_t& obj);

	// Boolean operator: true=object defined; false=object undefined
	operator bool() { return type != obj_not_alloc; }

	void unalloc() {
		switch(type) {
		case obj_scalar:
			delete u.str;
			break;
		case obj_array:
			delete u.array;
			break;
		case obj_hash:
			delete u.hash;
			break;
		}
		type = obj_not_alloc;
	}
};


} // end namespace TPTLib

#endif __tptlib_object_impl_h
