/*
 * object.h
 *
 * $Id$
 *
 */

/*
 * Copyright (C) 2002 Isaac W. Foraker (isaac@tazthecat.net)
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

#ifndef _libtpt_object_impl_h
#define _libtpt_object_impl_h

#ifdef _MSC_VER
#	pragma warning(disable:4290)
#endif

#include "token.h"
#include "smartptr.h"
#include "tptexcept.h"
#include <string>
#include <map>
#include <vector>

namespace TPT {

/**
 * The Object class is used to represent a generic object of type scalar,
 * array, or hash.
 *
 * Use Object::gettype() to determine the type of object before calling the
 * corresponding method: Object::scalar(), Object::array(), Object::hash().
 */
class Object {
public:
	// types of objects the Object may hold
	enum obj_types {
		type_notalloc = 0,
		type_scalar,
		type_array,
		type_hash,
		type_token
	};

	// Some typedefs
	typedef notboost::shared_ptr< Object > PtrType;
	typedef std::vector< PtrType > ArrayType;
	typedef std::map< std::string, PtrType > HashType;
	typedef Token<> TokenType;

	// Basic ctor
	Object() : type(type_notalloc) {}

	// Construct specified type of object
	explicit Object(obj_types t) throw(tptexception);

	// Copy ctors
	Object(const Object& obj) throw(tptexception);
	Object(const std::string& s);
	Object(const char* str);
	Object(const ArrayType& a);
	Object(const HashType& h);
	Object(const TokenType& tok);

	// dtor
	~Object() { deallocate(); }

	// Deallocate this object
	void deallocate();

	// Various assignments
	Object& operator=(const std::string& s) ;
	Object& operator=(const ArrayType& a);
	Object& operator=(const HashType& h);
	Object& operator=(const TokenType& tok);
	Object& operator=(const Object& obj) throw(tptexception);
	Object& operator=(obj_types t) throw(tptexception);

	// Boolean operator: true=object defined; false=object undefined
	operator bool() { return type != type_notalloc; }

	bool exists(const std::string& key);

	obj_types gettype() const { return type; }
	void settype(obj_types t) throw(tptexception);

	std::string& scalar() throw(tptexception);
	ArrayType& array() throw(tptexception);
	HashType& hash() throw(tptexception);
	TokenType& token() throw(tptexception);

private:
	void create(obj_types t) throw(tptexception);
	void createcopy(const Object& obj) throw(tptexception);

	obj_types type;
	union object_union {
		std::string* str;
		ArrayType* array;
		HashType* hash;
		TokenType* token;
	} u;
};


} // end namespace TPT

#endif // _libtpt_object_impl_h
