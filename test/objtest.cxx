/*
 * objtest
 *
 * $Date$
 * $Revision$
 *
 */

// Access the internals of the object module
#include "../src/lib/conf.h"
#include <libtpt/object.h>
#include <iostream>
#include <map>

using TPT::Object;

void test1();

int main(int argc, char* argv[])
{
	try {
		test1();
	} catch(const std::exception& e) {
		std::cout << "Exception " << e.what() << std::endl;
	} catch(...) {
		std::cout << "Unknown exception" << std::endl;
	}

	return 0;
}

void test1()
{
	Object obj(Object::type_hash);
	Object::HashType& hash = obj.hash();
	std::cout << "Allocating new object" << std::endl;

	Object::PtrType temp1(new Object("Test"));
	Object::PtrType temp2(temp1);
	temp1 = new Object("Test");
	temp2 = temp1;
	temp1 = new Object("Test");
	Object::PtrType temp3(temp2);
	temp2 = temp1;
	Object::PtrType pobj(new Object("Test"));
	hash["var"] = pobj;
	std::cout << "Type: " << pobj.get()->gettype() << std::endl;
	std::cout << "Read: " << hash["var"].get()->scalar() << std::endl;
	hash["var"] = new Object("Value");
	std::cout << "Type: " << hash["var"].get()->gettype() << std::endl;
	std::cout << "Read: " << hash["var"].get()->scalar() << std::endl;
	hash["test1"] = new Object(Object::type_array);
	hash["test2"] = new Object(Object::type_hash);
	hash["test3"] = new Object;
}
