/*
 * smartptrtest.cxx
 *
 * Regression tests for notboost::shared_ptr.
 *
 * Copyright (C) 2002-2010 Isaac W. Foraker (isaac at noscience dot net)
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
 */

#include <libtpt/smartptr.h>

#include <iostream>
#include <thread>
#include <vector>

// Counts live instances so tests can detect double-free / leaks without
// relying on a sanitizer being present.
struct Counted {
	static int live;
	Counted() { ++live; }
	~Counted() { --live; }
};
int Counted::live = 0;

bool test_self_assign_sole_owner();	// the H2 use-after-free regression
bool test_self_assign_shared();
bool test_normal_assign();
bool test_concurrent_sharing();		// the H2 refcount data-race regression

int main()
{
	bool result = false, r;

	r = test_self_assign_sole_owner();
	if (r)
		std::cout << "smart_ptr test 1 (self-assign sole owner): failed"
			<< std::endl;
	result |= r;

	r = test_self_assign_shared();
	if (r)
		std::cout << "smart_ptr test 2 (self-assign while shared): failed"
			<< std::endl;
	result |= r;

	r = test_normal_assign();
	if (r)
		std::cout << "smart_ptr test 3 (normal assign): failed" << std::endl;
	result |= r;

	r = test_concurrent_sharing();
	if (r)
		std::cout << "smart_ptr test 4 (concurrent sharing): failed"
			<< std::endl;
	result |= r;

	if (result)
		std::cout << "FAILED" << std::endl;
	else
		std::cout << "PASSED" << std::endl;

	return result;
}

/*
 * When a shared_ptr is the sole owner of its object, assigning it to itself
 * must not free and then re-reference the underlying control block.  Before
 * the self-assignment guard this was a use-after-free.
 */
bool test_self_assign_sole_owner()
{
	{
		notboost::shared_ptr<Counted> a(new Counted);
		if (Counted::live != 1)
			return true;

		// Silence self-assignment warnings while still exercising the path.
		notboost::shared_ptr<Counted>& alias = a;
		a = alias;

		// Object must survive and remain reachable.
		if (Counted::live != 1 || a.get() == 0)
			return true;
	}
	// Destruction must free exactly once.
	return Counted::live != 0;
}

/*
 * Self-assignment while the object is also owned by another shared_ptr must
 * leave both owners valid and free the object exactly once at the end.
 */
bool test_self_assign_shared()
{
	{
		notboost::shared_ptr<Counted> a(new Counted);
		notboost::shared_ptr<Counted> b(a);	// shared, refcount == 2
		if (Counted::live != 1)
			return true;

		notboost::shared_ptr<Counted>& alias = b;
		b = alias;

		if (Counted::live != 1 || a.get() == 0 || b.get() != a.get())
			return true;
	}
	return Counted::live != 0;
}

/*
 * A plain assignment must release the left-hand object and share the
 * right-hand one.
 */
bool test_normal_assign()
{
	{
		notboost::shared_ptr<Counted> a(new Counted);
		notboost::shared_ptr<Counted> b(new Counted);
		if (Counted::live != 2)
			return true;

		a = b;	// a's original object should be freed here

		if (Counted::live != 1 || a.get() != b.get())
			return true;
	}
	return Counted::live != 0;
}

/*
 * Copying and destroying a shared_ptr from many threads at once must not
 * race on the reference count.  The old hand-rolled counter was a plain
 * unsigned, so concurrent inc/dec could lose updates and drive the count to
 * zero while an owner was still live -- a premature free / double free.
 * std::shared_ptr uses an atomic control block, so the shared object must
 * survive for as long as the main thread holds its reference.
 *
 * A data race is probabilistic to trigger, so this hammers the count hard;
 * it reliably passes with the atomic control block and is a strong detector
 * under a thread sanitizer.  The main thread keeps one owner throughout, so
 * the pointed-to Counted is never legitimately destroyed until scope exit.
 */
bool test_concurrent_sharing()
{
	{
		notboost::shared_ptr<Counted> shared(new Counted);
		if (Counted::live != 1)
			return true;

		const int threadcount = 8;
		const int iterations = 100000;
		std::vector<std::thread> threads;
		for (int t = 0; t < threadcount; ++t)
		{
			threads.push_back(std::thread([&shared]() {
				for (int i = 0; i < iterations; ++i)
				{
					// Each copy bumps the shared refcount, each
					// destruction drops it -- concurrently across threads.
					notboost::shared_ptr<Counted> local = shared;
					if (local.get() == 0)
						std::abort();	// premature free observed
				}
			}));
		}
		std::vector<std::thread>::iterator it(threads.begin()),
			end(threads.end());
		for (; it != end; ++it)
			it->join();

		// The main thread never released its reference, so the object must
		// still be alive and singly owned.
		if (Counted::live != 1 || shared.get() == 0)
			return true;
	}
	// And freed exactly once on scope exit.
	return Counted::live != 0;
}
