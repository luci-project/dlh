#pragma once

#include <dlh/container/vector.hpp>

namespace File {

bool exists(const char * path);

bool readable(const char * path);

bool writeable(const char * path);

bool executable(const char * path);

char * contents(const char * path, size_t & size);
inline char * contents(const char * path) {
	size_t size;
	return contents(path, size);
}

Vector<const char *> lines(const char * path);

bool absolute(const char * __restrict__ path, char * __restrict__ buffer, size_t bufferlen, size_t & pathlen);

inline bool absolute(const char * __restrict__ path, char * __restrict__ buffer, size_t bufferlen) {
	size_t pathlen;
	return absolute(path, buffer, bufferlen, pathlen);
}
}  // Namespace File