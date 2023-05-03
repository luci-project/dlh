// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include <dlh/string.hpp>
#include <dlh/mem.hpp>

#ifdef DLH_LEGACY

extern "C" char *strchrnul(const char *s, int c) {
	return const_cast<char*>(String::find_or_end(s, c));
}

extern "C" char *strchr(const char *s, int c) {
	return const_cast<char*>(String::find(s, c));
}

extern "C" char *strstr(const char *haystack, const char *needle) {
	return const_cast<char*>(String::find(haystack, needle));
}

extern "C" char *strrchr(const char *s, int c) {
	return const_cast<char*>(String::find_last(s, c));
}

extern "C" int strcmp(const char *s1, const char *s2) {
	return String::compare(s1, s2);
}

extern "C" int strncmp(const char *s1, const char *s2, size_t n) {
	return String::compare(s1, s2, n);
}

extern "C" size_t strlen(const char *s) {
	return String::len(s);
}

extern "C" char * strcpy(char *dest, const char *src) { //NOLINT
	return String::copy(dest, src);
}

extern "C" char * strncpy(char *dest, const char *src, size_t n) {
	return String::copy(dest, src, n, true);
}

extern "C" char * strdup(const char *s) {
	return String::duplicate(s);
}

extern "C" char * strndup(const char *s, size_t n) {
	return String::duplicate(s, n);
}
#endif

extern "C" void* memcpy(void * __restrict__ dest, void const * __restrict__ src, size_t size) {
	return Memory::copy(dest, src, size);
}

extern "C" void* memmove(void * dest, void const * src, size_t size) {
	return Memory::move(dest, src, size);
}

extern "C" void* memset(void *dest, int pattern, size_t size) {
	return Memory::set(dest, pattern, size);
}

extern "C" int memcmp(const void * s1, const void * s2, size_t n) {
	return Memory::compare(s1, s2, n);
}