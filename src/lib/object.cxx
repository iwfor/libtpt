/*
 * object.cxx
 *
 * $Id$
 *
 */

/*
 * Copyright (C) 2002-2003 Isaac W. Foraker (isaac@tazthecat.net)
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

#include "conf.h"
#include <libtpt/object.h>

#include <iostream>

namespace TPT {

/*
 * Note: Only set "type" AFTER calling "new" to allocate memory since
 * new might throw an exception.
 *
 */


/**
 * Construct an object of the specified type.
 *
 * @param	t		The type of object to be set
 *
 */
Object::Object(obj_types t) throw(tptexception)
	: type(type_notalloc)
{
	create(t);
}


/**
 * Construct a copy of another object.
 *
 * @param	t		The type of object to be set
 *
 */
Object::Object(const Object& obj) throw(tptexception)
	: type(type_notalloc)
{
	createcopy(obj);
}

/**
 * Construct an object of a scalar std::string.
 *
 * @param	str			std::string to be copied into this object.
 * @return	nothing
 *
 */
Object::Object(const std::string& s)
	: type(type_notalloc)
{
	u.str = new std::string(s);
	type = type_scalar;
}

/**
 * Construct an object of a scalar std::string from a char*.
 *
 * @param	str			std::string to be copied into this object.
 * @return	nothing
 *
 */
Object::Object(const char* str)
	: type(type_notalloc)
{
	u.str = new std::string(str);
	type = type_scalar;
}

/**
 * Construct an object of a array of objects.
 *
 * @param	array		Array to be copied into this object.
 * @return	nothing
 *
 */
Object::Object(const ArrayType& a)
	: type(type_notalloc)
{
	u.array = new ArrayType(a);
	type = type_array;
}

/**
 * Construct an object of a hash of objects.
 *
 * @param	hash		Hash to be copied into this object.
 * @return	nothing
 *
 */
Object::Object(const HashType& h)
	: type(type_notalloc)
{
	u.hash = new HashType(h);
	type = type_hash;
}


/**
 * Construct an object of a token of objects.
 *
 * @param	token		Token to be copied into this object.
 * @return	nothing
 *
 */
Object::Object(const TokenType& tok)
	: type(type_notalloc)
{
	u.token = new TokenType(tok);
	type = type_token;
}


/**
 * Copy a scalar string to this object.
 *
 * @param	str			std::string to be copied into this object.
 * @return	reference to this object.
 *
 */
Object& Object::operator=(const std::string& s)
{
	if (type == type_scalar)
		*u.str = s;
	else
	{
		deallocate();
		u.str = new std::string(s);
		type = type_scalar;
	}
	return *this;
}

/**
 * Copy an array of objects to this object.
 *
 * @param	array		Array to be copied into this object.
 * @return	reference to this object.
 *
 */
Object& Object::operator=(const ArrayType& a)
{
	if (type == type_array)
		*u.array = a;
	else
	{
		deallocate();
		u.array = new ArrayType(a);
		type = type_array;
	}
	return *this;
}

/**
 * Copy a hash of objects to this object.
 *
 * @param	hash		Hash to be copied into this object.
 * @return	reference to this object.
 *
 */
Object& Object::operator=(const HashType& h)
{
	if (type == type_hash)
		*u.hash = h;
	else
	{
		deallocate();
		u.hash = new HashType(h);
		type = type_hash;
	}
	return *this;
}


/**
 * Copy a token of objects to this object.
 *
 * @param	token		Token to be copied into this object.
 * @return	reference to this object.
 *
 */
Object& Object::operator=(const TokenType& tok)
{
	if (type == type_token)
		*u.token = tok;
	else
	{
		deallocate();
		u.token = new TokenType(tok);
		type = type_token;
	}
	return *this;
}


/**
 * Copy another object to this object.
 *
 * @param	obj		Object to be copied into this object.
 * @return	reference to this object.
 *
 */
Object& Object::operator=(const Object& obj) throw(tptexception)
{
	deallocate();
	createcopy(obj);

	return *this;
}


/**
 * Change an object's type to an empty object of the specified type.
 *
 * @param	t		The type of object to be set
 * @return	false on success;
 * @return	true on success;
 *
 */
Object& Object::operator=(obj_types t) throw(tptexception)
{
	deallocate();
	create(t);
	return *this;
}



/**
 * Deallocate this object and any objects it cointains.
 *
 * @param	none
 * @return	nothing
 *
 */
void Object::deallocate() {
	switch(type) {
	case type_scalar:
		delete u.str;
		break;
	case type_array:
		delete u.array;
		break;
	case type_hash:
		delete u.hash;
		break;
	case type_token:
		delete u.token;
		break;
	case type_notalloc:
		return;
	}
	type = type_notalloc;
}

/**
 * Change an object's type to an empty object of the specified type.
 *
 * @param	t		The type of object to be set
 * @return	false on success;
 * @return	true on success;
 *
 */
void Object::settype(obj_types t) throw(tptexception)
{
	deallocate();
	create(t);
}

/**
 * Get this object's string
 *
 * @param	none
 * @return	Reference to this object's string.
 * @except	tptexception
 *
 */
std::string& Object::scalar() throw(tptexception)
{
	if (type != type_scalar)
		throw tptexception("Object is not a scalar");
	return *u.str;
}

/**
 * Get this object's array of objects
 *
 * @param	none
 * @return	Reference to this object's array of object.
 * @except	tptexception
 *
 */
Object::ArrayType& Object::array() throw(tptexception)
{
	if (type != type_array)
		throw tptexception("Object is not a array");
	return *u.array;
}

/**
 * Get this object's hash of objects
 *
 * @param	none
 * @return	Reference to this object's hash of objects.
 * @except	tptexception
 *
 */
Object::HashType& Object::hash() throw(tptexception)
{
	if (type != type_hash)
		throw tptexception("Object is not a hash");
	return *u.hash;
}


/**
 * Get this object's token
 *
 * @param	none
 * @return	Reference to this object's token.
 * @except	tptexception
 *
 */
Object::TokenType& Object::token() throw(tptexception)
{
	if (type != type_token)
		throw tptexception("Object is not a token");
	return *u.token;
}


/**
 * Tell if a key exists in a hash table.
 *
 * @param	key			Key in hash
 * @return	true if key exists;
 * @return	false if key does not exist or object not a hash.
 *
 */
bool Object::exists(const std::string& key)
{
	if (type != type_hash)
		return false;
	return u.hash->find(key) != u.hash->end();
}

/**
 * Create an object of a specific type
 *
 */
void Object::create(obj_types t) throw(tptexception)
{
	switch (t) {
	case type_scalar:
		u.str = new std::string;
		break;
	case type_array:
		u.array = new ArrayType;
		break;
	case type_hash:
		u.hash = new HashType;
		break;
	case type_token:
		u.token = new TokenType;
		break;
	case type_notalloc:
		break;
	default:
		throw tptexception("Invalid object type");
	}
	type = t;
}

/**
 * Create a copy of another object
 *
 */
void Object::createcopy(const Object& obj) throw(tptexception)
{
	switch (obj.type) {
	case type_scalar:
		u.str = new std::string(*obj.u.str);
		break;
	case type_array:
		u.array = new ArrayType(*obj.u.array);
		break;
	case type_hash:
		u.hash = new HashType(*obj.u.hash);
		break;
	case type_token:
		u.token = new TokenType(*obj.u.token);
		break;
	case type_notalloc:
		break;
	default:
		throw tptexception("Invalid object type");
	}
	type = obj.type;
}

} // end namespace TPT
