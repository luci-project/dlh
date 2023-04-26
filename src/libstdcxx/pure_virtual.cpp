// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include <dlh/stream/output.hpp>
#include <dlh/syscall.hpp>

extern "C" [[noreturn]] void __cxa_pure_virtual() {
	cerr <<  "Pure virtual function was called -- this if obviously not valid!" << endl;
	Syscall::abort();
}
