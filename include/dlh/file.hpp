#pragma once

#include <dlh/container/pair.hpp>
#include <dlh/container/vector.hpp>

namespace File {

bool exists(const char * path);

bool readable(const char * path);

bool writeable(const char * path);

bool executable(const char * path);

namespace contents {
enum Format : uint8_t {
	FORMAT_UNKNOWN,
	// Executables
	FORMAT_ELF,
	FORMAT_EXE,
	FORMAT_COM,
	FORMAT_MACHO,
	FORMAT_DALVIK,
	FORMAT_WASM,
	// Archives
	FORMAT_AR,
	FORMAT_CPIO,
	FORMAT_RPM,
	FORMAT_XAR,
	// Compressed
	FORMAT_BZ2,
	FORMAT_GZ,
	FORMAT_LZ,
	FORMAT_XZ,
	FORMAT_Z,
	FORMAT_ZLIB,
	FORMAT_ZIP,
	// Images (and documents)
	FORMAT_GIF,
	FORMAT_BMP,
	FORMAT_ICO,
	FORMAT_TIFF,
	FORMAT_PNG,
	FORMAT_JPEG,
	FORMAT_JPEG2000,
	FORMAT_PS,
	FORMAT_PDF,
	// other
	FORMAT_SCRIPT,
};

Format format(const char * buf, size_t size);

Format format(const char * path);

const char * format_description(Format format);

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
