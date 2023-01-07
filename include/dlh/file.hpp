#pragma once

#include <dlh/container/pair.hpp>
#include <dlh/container/vector.hpp>

namespace File {

bool exists(const char * path);

bool readable(const char * path);

bool writeable(const char * path);

bool executable(const char * path);

namespace contents {
char * get(const char * path, size_t & size);
inline char * get(const char * path) {
	size_t size;
	return get(path, size);
}

size_t set(const char * path, const char * data, size_t len, bool append = false);
size_t set(const char * path, const char * data, bool append = false);

}

Vector<const char *> lines(const char * path);

bool absolute(int fd, char * __restrict__ buffer, size_t bufferlen, size_t & pathlen);
inline bool absolute(int fd, char * __restrict__ buffer, size_t bufferlen) {
	size_t pathlen;
	return absolute(fd, buffer, bufferlen, pathlen);
}

bool absolute(const char * __restrict__ path, char * __restrict__ buffer, size_t bufferlen, size_t & pathlen);
inline bool absolute(const char * __restrict__ path, char * __restrict__ buffer, size_t bufferlen) {
	size_t pathlen;
	return absolute(path, buffer, bufferlen, pathlen);
}

Pair<const char *, const char *> pathsplit(char *path);

inline const char * dirname(char * path) {
	return pathsplit(path).first;
}

inline const char * basename(char * path) {
	return pathsplit(path).second;
}
}  // Namespace File
