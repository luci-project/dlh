// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include <dlh/mem.hpp>

#ifdef DLH_LEGACY
extern "C" void * malloc(size_t size) {
	return Memory::alloc<void*>(size);
}

extern "C" void free(void *ptr) {
	Memory::free(ptr);
}

extern "C" void *realloc(void *ptr, size_t size) {
	return Memory::realloc(ptr, size);
}

extern "C" void * calloc(size_t nmemb, size_t size) {
	return reinterpret_cast<void*>(Memory::alloc_array(nmemb, size));
}

#include <dlh/random.hpp>
static Random random;
extern "C" int rand() {
	return static_cast<int>(random.number());
}

extern "C" void srand(unsigned int seed) {
	random.set(seed);
}
#endif
