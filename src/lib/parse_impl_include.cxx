/*
 * parse_impl.cxx
 *
 * Parser Implementation
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

#include "conf.h"
#include "parse_impl.h"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cctype>
#include <cstdlib>
#ifdef _WIN32
#	include <cstdlib>	// _fullpath, _MAX_PATH
#else
#	include <climits>	// PATH_MAX
#endif

namespace TPT {

// Path helpers used to enforce the confined-include policy.  These are file
// local because the containment check is the only place they are needed.
namespace {

// True if the file at path can be opened for reading.
bool file_readable(const std::string& path)
{
	std::ifstream f(path.c_str(), std::ios::in | std::ios::binary);
	return f.is_open();
}

// True if path is absolute (POSIX root, UNC/backslash root, or drive letter).
bool is_absolute(const std::string& p)
{
	if (p.empty())
		return false;
	if (p[0] == '/' || p[0] == '\\')
		return true;
#ifdef _WIN32
	if (p.size() >= 2 && std::isalpha(static_cast<unsigned char>(p[0]))
			&& p[1] == ':')
		return true;
#endif
	return false;
}

// True if any path component is exactly "..".
bool has_parent_ref(const std::string& p)
{
	std::string::size_type i = 0, n = p.size();
	while (i < n)
	{
		std::string::size_type j = i;
		while (j < n && p[j] != '/' && p[j] != '\\')
			++j;
		if ((j - i) == 2 && p[i] == '.' && p[i + 1] == '.')
			return true;
		i = (j < n) ? j + 1 : j;
	}
	return false;
}

// Resolve path to an absolute, symlink-free canonical form.  Returns false
// if the path does not exist (POSIX) or cannot be resolved.  The output uses
// forward slashes, and is lowercased on Windows where the file system is
// case-insensitive, so containment can be compared textually.
bool canonicalize(const std::string& in, std::string& out)
{
#ifdef _WIN32
	char buf[_MAX_PATH];
	if (!_fullpath(buf, in.c_str(), _MAX_PATH))
		return false;
	out = buf;
	for (std::string::iterator i(out.begin()); i != out.end(); ++i)
	{
		if (*i == '\\')
			*i = '/';
		else
			*i = static_cast<char>(std::tolower(
				static_cast<unsigned char>(*i)));
	}
	return true;
#else
	char buf[PATH_MAX];
	if (!realpath(in.c_str(), buf))
		return false;
	out = buf;
	return true;
#endif
}

// True if canonical path canonpath lies within canonical root canonroot.
// The comparison is component-aware (via a trailing separator) so that a
// sibling like "/srv/tmpl-evil" is not treated as inside "/srv/tmpl".
bool within_root(const std::string& canonroot, const std::string& canonpath)
{
	if (canonpath == canonroot)
		return true;
	std::string prefix(canonroot);
	if (!prefix.empty() && prefix[prefix.size() - 1] != '/')
		prefix += '/';
	return (canonpath.size() > prefix.size()) &&
		(canonpath.compare(0, prefix.size(), prefix) == 0);
}

} // end anonymous namespace


/*
 * Resolve a requested include name to a safe, openable path under the active
 * security policy.
 *
 * @return	false on success (resolved holds the path to open);
 * @return	true on failure (an error has been recorded)
 */
bool Parser_Impl::resolveincludepath(const std::string& requested,
	std::string& resolved)
{
	if (ctx.mode == include_disabled)
	{
		recorderror("Security Error: file inclusion is disabled");
		return true;
	}

	if (requested.empty())
	{
		recorderror("Error: empty include path");
		return true;
	}

	if (ctx.mode == include_unrestricted)
	{
		// Legacy behavior: search the roots, then try the raw path.
		IncludeList::iterator it(ctx.roots.begin()),
			end(ctx.roots.end());
		for (; it != end; ++it)
		{
			std::string path(*it);
			path += '/';
			path += requested;
			if (file_readable(path))
			{
				resolved = path;
				return false;
			}
		}
		if (file_readable(requested))
		{
			resolved = requested;
			return false;
		}
		recorderror("File Error: Could not read " + requested);
		return true;
	}

	// Confined mode (the default): the request must be relative and must
	// resolve to a real file inside one of the include roots.
	if (is_absolute(requested) || has_parent_ref(requested))
	{
		recorderror("Security Error: include path must be relative and "
			"contained within an include root: " + requested);
		return true;
	}

	// With no roots configured, confine includes to the current directory.
	IncludeList roots(ctx.roots);
	if (roots.empty())
		roots.push_back(".");

	IncludeList::iterator it(roots.begin()), end(roots.end());
	for (; it != end; ++it)
	{
		std::string canonroot;
		if (!canonicalize(*it, canonroot))
			continue;	// root does not exist; skip it
		std::string candidate(*it);
		candidate += '/';
		candidate += requested;
		std::string canonpath;
		// canonicalize() confirms the file exists and resolves symlinks,
		// so a link inside a root cannot point the include outside it.
		if (canonicalize(candidate, canonpath) &&
			file_readable(canonpath) &&
			within_root(canonroot, canonpath))
		{
			resolved = canonpath;
			return false;
		}
	}

	recorderror("Security Error: include not found within an include "
		"root: " + requested);
	return true;
}


/*
 * Guard against runaway recursion when following @include directives.
 *
 * @return	true (and records an error) if this include must not proceed;
 * @return	false if it is safe to include canonpath
 */
bool Parser_Impl::checkincludeguard(const std::string& canonpath)
{
	if (ctx.stack.size() >= ctx.maxdepth)
	{
		recorderror("Error: maximum @include depth exceeded");
		return true;
	}
	IncludeList::iterator it(ctx.stack.begin()), end(ctx.stack.end());
	for (; it != end; ++it)
	{
		if (*it == canonpath)
		{
			recorderror("Error: @include cycle detected: " + canonpath);
			return true;
		}
	}
	return false;
}


namespace {
// RAII helper so the shared include stack is always popped, even if parsing
// the included file throws.
struct incstackguard {
	IncludeList& stack;
	incstackguard(IncludeList& s, const std::string& p) : stack(s)
	{ stack.push_back(p); }
	~incstackguard() { stack.pop_back(); }
};
} // end anonymous namespace


void Parser_Impl::parse_include(std::ostream* os)
{
	Object params;
	if (getparamlist(params))
		return;
	Object::ArrayType& pl = params.array();

	if (pl.size() != 1)
	{
		recorderror("Error: @include takes exactly 1 parameter");
		return;
	}

	Object& obj = *pl[0].get();
	if (obj.gettype() != Object::type_scalar)
	{
		recorderror("Error: @include expects string");
		return;
	}

	std::string path;
	if (resolveincludepath(obj.scalar(), path))
		return;		// resolver recorded the error

	// Reject include cycles and excessive nesting before recursing.
	if (checkincludeguard(path))
		return;

	incstackguard guard(ctx.stack, path);

	// Create another Impl which inherits the shared parse state (symbols,
	// macros, functions, and include context) to process the include.
	Buffer buf(path.c_str());
	if (!buf)
	{
		recorderror("File Error: Could not read " + obj.scalar());
		return;
	}
	Parser_Impl incl(buf, symbols, macros, funcs, ctx);
	if (incl.pass1(os))
	{
		// copy incl's error list, if any
		ErrorList::iterator it(incl.errlist.begin()), end(incl.errlist.end());
		for (; it != end; ++it)
			errlist.push_back(*it);
	}
}


void Parser_Impl::parse_includetext(std::ostream* os)
{
	Object params;
	if (getparamlist(params))
		return;
	Object::ArrayType& pl = params.array();

	if (pl.size() != 1)
	{
		recorderror("Error: @includetext takes exactly 1 parameter");
		return;
	}

	Object& obj = *pl[0].get();
	if (obj.gettype() != Object::type_scalar)
	{
		recorderror("Error: @includetext expects string");
		return;
	}

	std::string path;
	if (resolveincludepath(obj.scalar(), path))
		return;		// resolver recorded the error

	// @includetext emits raw text and does not recurse into the parser, so
	// no cycle/depth guard is needed here.
	Buffer buf(path.c_str());
	if (!buf)
	{
		recorderror("File Error: Could not read " + obj.scalar());
		return;
	}
	while (buf)
		*os << buf.getnextchar();
}

} // end namespace TPT
