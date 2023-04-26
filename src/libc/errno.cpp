// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include <dlh/error.hpp>
#include <dlh/string.hpp>
#include <dlh/thread.hpp>
#include <dlh/syscall.hpp>

#ifdef DLH_LEGACY
extern "C" int *__errno_location() {
	return &(Thread::local_errno());
}

extern "C" char * strerror_r(int errnum, char *buf, size_t buflen) {
	String::copy(buf, __errno_string(errnum), buflen - 1);
	buf[buflen - 1] = '\0';
	return buf;
}

extern "C" char * strerror(int errnum) {
	static char buf[63];
	return strerror_r(errnum, buf, 63);
}

extern "C" void perror(const char *msg) {
	warn(msg, __errno_string(*__errno_location()));
}
#endif
