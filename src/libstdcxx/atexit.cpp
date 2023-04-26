// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include <dlh/mem.hpp>
#include <dlh/assert.hpp>

static struct ExitFunc {
	void (*func)(void *);
	void * arg;
	void * dso_handle;
} * exit_func = nullptr;

static size_t exit_func_len = 0;
static size_t exit_func_cap = 0;

extern "C" int __cxa_atexit(void (*func)(void *), void * arg, void * dso_handle) {
	// TODO: Lock
	if (exit_func_cap >= exit_func_len) {
		exit_func_cap = exit_func_cap >= 32 ? exit_func_cap * 2 : 32;
		exit_func = Memory::realloc(exit_func, exit_func_cap);
		assert(exit_func != nullptr);
	}
	auto & f = exit_func[exit_func_len++];
	f.func = func;
	f.arg = arg;
	f.dso_handle = dso_handle;

	return 0;
}

static void call(void *p) {
	reinterpret_cast<void (*)(void)>(p)();
}

extern "C" int atexit(void (*func)(void)) {
	return __cxa_atexit(call, reinterpret_cast<void *>(func), 0);
}
