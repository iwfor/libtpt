/*
 * object.cxx
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

#include "object.h"

namespace TPTLib {

/*
 * Note: Only set "type" AFTER calling "new" to allocate memory since
 * new might throw an exception.
 *
 */

object_t::object_t(const std::string& str)
	: type(obj_not_alloc)
{
	u.str = new std::string(str);
	type = obj_scalar;
}

object_t::object_t(const ObjectArrayType& array)
	: type(obj_not_alloc)
{
	u.array = new ObjectArrayType(array);
	type = obj_array;
}

object_t::object_t(const ObjectHashType& hash)
	: type(obj_not_alloc)
{
	u.hash = new ObjectHashType(hash);
	type = obj_hash;
}

object_t& object_t::operator=(const std::string& str)
{
	deallocate();
	u.str = new std::string(str);
	type = obj_scalar;
	return *this;
}

object_t& object_t::operator=(const ObjectArrayType& array)
{
	deallocate();
	u.array = new ObjectArrayType(array);
	type = obj_array;
	return *this;
}

object_t& object_t::operator=(const ObjectHashType& hash)
{
	deallocate();
	u.hash = new ObjectHashType(hash);
	type = obj_hash;
	return *this;
}

object_t& object_t::operator=(const object_t& obj)
{
	deallocate();

	switch(obj.type) {
	case obj_scalar:
		u.str = new std::string(*obj.u.str);
		type = obj_scalar;
		break;
	case obj_array:
		u.array = new ObjectArrayType(*obj.u.array);
		type = obj_array;
		break;
	case obj_hash:
		u.hash = new ObjectHashType(*obj.u.hash);
		type = obj_hash;
		break;
	}

	return *this;
}

void object_t::deallocate() {
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


const std::string& object_t::getscalar()
{
	if (type != obj_scalar)
		throw std::exception("Object is not a scalar");
	return *u.str;
}

const object_t::ObjectArrayType& object_t::getarray()
{
	if (type != obj_array)
		throw std::exception("Object is not a array");
	return *u.array;
}

const object_t::ObjectHashType& object_t::gethash()
{
	if (type != obj_hash)
		throw std::exception("Object is not a hash");
	return *u.hash;
}


} // end namespace TPTLib
