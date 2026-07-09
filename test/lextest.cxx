/*
 * lextest.cxx
 *
 * Tests for lexer edge cases: Lex::unget must not underflow the buffer index
 * or column counter when asked to push back a token larger than the amount
 * of input consumed so far.
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

#include "lexical.h"			// internal lexer header (src/lib)
#include <libtpt/buffer.h>
#include <libtpt/token.h>

#include <iostream>

bool test_unget_clamps_to_start();
bool test_unget_normal();

int main()
{
	bool result = false, r;

	r = test_unget_clamps_to_start();
	if (r)
		std::cout << "lex test 1 (unget underflow clamp): failed" << std::endl;
	result |= r;

	r = test_unget_normal();
	if (r)
		std::cout << "lex test 2 (unget normal rewind): failed" << std::endl;
	result |= r;

	std::cout << (result ? "FAILED" : "PASSED") << std::endl;
	return result;
}

/*
 * Ungetting a token whose length exceeds the current offset must clamp to the
 * start of the buffer (index 0), not underflow buf_.offset() - i to a huge
 * index (which would leave the position unchanged and corrupt the column).
 */
bool test_unget_clamps_to_start()
{
	TPT::Buffer buf("abcde", 5);
	TPT::Lex lex(buf);

	// Consume two characters; offset is now 2.
	lex.safeget();
	lex.safeget();
	if (lex.index() != 2)
		return true;

	TPT::Token<> t;
	t.value = "0123456789";		// length 10, larger than the offset of 2
	lex.unget(t);

	// Clamped to the start of the buffer.
	return lex.index() != 0;
}

/*
 * A normal unget (token no larger than the offset) rewinds by the token's
 * length.
 */
bool test_unget_normal()
{
	TPT::Buffer buf("abcdef", 6);
	TPT::Lex lex(buf);

	// Consume four characters; offset is now 4.
	lex.safeget();
	lex.safeget();
	lex.safeget();
	lex.safeget();
	if (lex.index() != 4)
		return true;

	TPT::Token<> t;
	t.value = "xy";				// length 2, no larger than the offset
	lex.unget(t);

	return lex.index() != 2;	// rewound by 2 -> offset 2
}
