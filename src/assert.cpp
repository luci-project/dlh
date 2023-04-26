// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include <dlh/assert.hpp>

#include <dlh/syscall.hpp>
#include <dlh/stream/output.hpp>

[[noreturn]] void __assert_fail(const char * exp, const char * file, int line, const char * func) {
	// Do not use assertions in OutputStream!
	cerr << "Assertion '" << exp << "' failed (" << func << " @ " << file
	     << ":" << dec << line << ")" << endl;

	Syscall::abort();
}
