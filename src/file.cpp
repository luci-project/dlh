// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include <dlh/file.hpp>

#include <dlh/log.hpp>
#include <dlh/string.hpp>
#include <dlh/syscall.hpp>
#include <dlh/stream/buffer.hpp>

namespace File {

bool exists(const char * path) {
	return Syscall::access(path, F_OK).success();
}

bool readable(const char * path) {
	return Syscall::access(path, R_OK).success();
}

bool writeable(const char * path) {
	return Syscall::access(path, W_OK).success();
}

bool executable(const char * path) {
	return Syscall::access(path, X_OK).success();
}

namespace contents {
static const struct {
	unsigned char bytes[6];
	uint8_t size;
	Format format;
} __attribute__((packed)) format_magic[] {
	{ { '0', '7', '0', '7', '0', '7' }, 6, FORMAT_CPIO },
	{ { '!', '<', 'a', 'r', 'c', 'h' }, 6, FORMAT_AR },
	{ { 'G', 'I', 'F', '8', '7', 'a' }, 6, FORMAT_GIF },
	{ { 'G', 'I', 'F', '8', '9', 'a' }, 6, FORMAT_GIF },
	{ { 'd', 'e', 'x', 0x0a, '0', '3' }, 6, FORMAT_DALVIK },
	{ { 0x89, 'P', 'N', 'G', 0x0d, 0x0a }, 6, FORMAT_PNG },
	{ { 0xfd, 0x37, 0x7a, 0x58, 0x5a, 0x00 }, 6, FORMAT_XZ },
	{ { '%', 'P', 'D', 'F', '-' }, 5, FORMAT_PDF },
	{ { 'x', 'a', 'r', '!' }, 4, FORMAT_XAR },
	{ { 00, 'a', 's', 'm' }, 4, FORMAT_WASM },
	{ { 0xed, 0xab, 0xee, 0xdb }, 4, FORMAT_RPM },
	{ { 0x4c, 0x4a, 0x49, 0x50 }, 4, FORMAT_LZ },
	{ { 0xfe, 0xed, 0xfa, 0xce }, 4, FORMAT_MACHO },
	{ { 0xce, 0xfa, 0xed, 0xfe }, 4, FORMAT_MACHO },
	{ { 0xfe, 0xed, 0xfa, 0xcf }, 4, FORMAT_MACHO },
	{ { 0xcf, 0xfa, 0xed, 0xfe }, 4, FORMAT_MACHO },
	{ { 0x50, 0x4b, 0x03, 0x04 }, 4, FORMAT_ZIP },
	{ { 0x50, 0x4b, 0x05, 0x06 }, 4, FORMAT_ZIP },
	{ { 0x50, 0x4b, 0x07, 0x08 }, 4, FORMAT_ZIP },
	{ { 0x00, 0x00, 0x01, 0x00 }, 4, FORMAT_ICO },
	{ { 'I', 'I', 0x2a, 0x00 }, 4, FORMAT_TIFF },
	{ { 'M', 'M', 0x00, 0x2a }, 4, FORMAT_TIFF },
	{ { 0xff, 0xd8, 0xff, 0xdb }, 4, FORMAT_JPEG },
	{ { 0xff, 0xd8, 0xff, 0xe0 }, 4, FORMAT_JPEG },
	{ { 0xff, 0xd8, 0xff, 0xe1 }, 4, FORMAT_JPEG },
	{ { 0xff, 0xd8, 0xff, 0xee }, 4, FORMAT_JPEG },
	{ { 0xff, 0x4f, 0xff, 0x51 }, 4, FORMAT_JPEG2000 },
	{ { 0x7f, 'E', 'L', 'F' },  4, FORMAT_ELF},
	{ { '%', '!', 'P', 'S' },  4, FORMAT_PS},
	{ { 'B', 'Z', 'h' }, 2, FORMAT_BZ2},
	{ { '#', '!' }, 2, FORMAT_SCRIPT},
	{ { 'B', 'M' }, 2, FORMAT_BMP},
	{ { 'x', 0x01 }, 2, FORMAT_ZLIB},
	{ { 'x', 0x5e }, 2, FORMAT_ZLIB},
	{ { 'x', 0x9c }, 2, FORMAT_ZLIB},
	{ { 'x', 0xda }, 2, FORMAT_ZLIB},
	{ { 'x', 0x20 }, 2, FORMAT_ZLIB},
	{ { 'x', 0x7d }, 2, FORMAT_ZLIB},
	{ { 'x', 0xbb }, 2, FORMAT_ZLIB},
	{ { 'x', 0xf9 }, 2, FORMAT_ZLIB},
	{ { 0x4d, 0x5a }, 2, FORMAT_EXE},
	{ { 0x5a, 0x4d }, 2, FORMAT_EXE},
	{ { 0x1f, 0x8b }, 2, FORMAT_GZ},
	{ { 0x1f, 0x9d }, 2, FORMAT_Z},
	{ { 0x1f, 0xa0 }, 2, FORMAT_Z},
	{ { 0xc9 }, 1, FORMAT_COM}
};

Format format(const char * buf, size_t size) {
	if (buf != nullptr && size > 0)
		for (const auto & m : format_magic)
			if (size >= m.size && m.size > 0) {
				bool match = true;
				for (size_t s = 0; s < m.size; s++) {
					if (m.bytes[s] != static_cast<unsigned char>(buf[s])) {
						match = false;
						break;
					}
				}
				if (match)
					return m.format;
			}
	return FORMAT_UNKNOWN;
}

Format format(const char * path) {
	if (path != nullptr) {
		if (auto fd = Syscall::open(path, O_RDONLY)) {
			char buf[10] = {};
			size_t read = 0;
			while (read < count(buf)) {
				if (auto r = Syscall::read(fd.value(), buf + read, count(buf) - read)) {
					if (r.value() == 0)
						break;
					else
						read += r.value();
				} else {
					LOG_WARNING << "Unable to read " << count(buf) << " bytes from " << path << ":" << r.error_message() << endl;
					break;
				}
			}
			Syscall::close(fd.value());
			return format(buf, read);
		} else {
			LOG_WARNING << "Unable to open " << path << " for reading:" << fd.error_message() << endl;
		}
	}
	return FORMAT_UNKNOWN;
}

const char * format_description(Format format) {
	switch (format) {
		case FORMAT_ELF: return "Executable file for Unix/Linux (Executable and Linkable Format)";
		case FORMAT_EXE: return "Executable file for DOS or Windows (EXE)";
		case FORMAT_COM: return "Executable file for CP/M or DOS (COM)";
		case FORMAT_MACHO: return "Executable file for Mac OS X (Mach object)";
		case FORMAT_DALVIK: return "Executable file for Android (Dalvik)";
		case FORMAT_WASM: return "Executable file for WebAssembly";
		case FORMAT_AR: return "Archive (for static libraries)";
		case FORMAT_CPIO: return "Archive (CPIO)";
		case FORMAT_RPM: return "Archive (RedHat Package Manager)";
		case FORMAT_XAR: return "Archive (extensible archive format)";
		case FORMAT_BZ2: return "Compressed file (Bzip2)";
		case FORMAT_GZ: return "Compressed file (gzip)";
		case FORMAT_LZ: return "Compressed file (lzip)";
		case FORMAT_XZ: return "Compressed file (XZ Utils)";
		case FORMAT_Z: return "Compressed file (LZH)";
		case FORMAT_ZLIB: return "Compressed file (zlib)";
		case FORMAT_ZIP: return "Compressed file (zip)";
		case FORMAT_GIF: return "Image file (Graphics Interchange Format)";
		case FORMAT_BMP: return "Image file (Bitmap)";
		case FORMAT_ICO: return "Image file (Computer Icon)";
		case FORMAT_TIFF: return "Image file (Tagged Image File Format)";
		case FORMAT_PNG: return "Image file (Portable Network Graphics)";
		case FORMAT_JPEG: return "Image file (JPEG)";
		case FORMAT_JPEG2000: return "Image file (JPEG2000)";
		case FORMAT_PS: return "Document file (Postscript)";
		case FORMAT_PDF: return "Document file (Portable Document Format)";
		case FORMAT_SCRIPT: return "script file";
		default: return "(unknown)";
	}
}

char * get(const char * path, size_t & size) {
	auto fd = Syscall::open(path, O_RDONLY);
	if (fd.failed()) {
		LOG_ERROR << "Opening file " << path << " failed: " << fd.error_message() << endl;
		return nullptr;
	}

	struct stat sb;
	if (auto fstat = Syscall::fstat(fd.value(), &sb)) {
		if ((size = sb.st_size) == 0)
			return nullptr;
	} else {
		LOG_ERROR << "Stat file " << path << " failed: " << fstat.error_message() << endl;
		Syscall::close(fd.value());
		return nullptr;
	}

	auto addr = Syscall::mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd.value(), 0);
	Syscall::close(fd.value());
	if (addr.failed()) {
		LOG_ERROR << "Mmap file " << path << " failed: " << addr.error_message() << endl;
		size = -1;
		return nullptr;
	} else {
		LOG_VERBOSE << "Mapped '" << path << "' (" << size << " bytes)" << endl;
		return reinterpret_cast<char *>(addr.value());
	}
}

size_t set(const char * path, const char * data, size_t len, bool append) {
	size_t written = 0;
	if (data == nullptr)
		len = 0;
	if (auto fd = Syscall::open(path, O_WRONLY | (append ? O_APPEND : O_TRUNC) | O_CREAT, 0644)) {
		while (written < len) {
			if (auto write = Syscall::write(fd.value(), data + written, len - written)) {
				written += write.value();
			} else {
				LOG_ERROR << "Write failed: " << write.error_message() << endl;
				break;
			}
		}
		Syscall::close(fd.value());
	} else {
		LOG_ERROR << "Opening file " << path << " failed: " << fd.error_message() << endl;
	}
	return written;
}

size_t set(const char * path, const char * data, bool append) {
	return set(path, data, data == nullptr ? 0 : String::len(data), append);
}

}  // namespace contents

Vector<const char *> lines(const char * path) {
	size_t size;
	return String::split_inplace(File::contents::get(path, size), '\n');
}

void __procfdname(char *buf, unsigned fd) {
	unsigned i = 0;
	for (; (buf[i] = "/proc/self/fd/"[i]) != 0; i++) {}
	if (fd == 0) {
		buf[i] = '0';
		buf[i+1] = 0;
		return;
	}
	unsigned j = fd;
	for (; j != 0; j/=10, i++) {}
	buf[i] = 0;
	for (; fd != 0; fd/=10)
		buf[--i] = static_cast<char>('0' + fd % 10);
}

bool absolute(int fd, char * __restrict__ buffer, size_t bufferlen, size_t & pathlen) {
	StringStream<32> procfd;
	procfd << "/proc/self/fd/" << fd;
	if (auto r = Syscall::readlink(procfd.str(), buffer, bufferlen - 1)) {
		pathlen = static_cast<size_t>(r.value());
		buffer[pathlen] = '\0';
		return true;
	} else {
		LOG_ERROR << "Unable to get absolute path: " << r.error_message() << endl;
		buffer[0] = '\0';
		pathlen = 0;
		return false;
	}
}

bool absolute(const char * __restrict__ path, char * __restrict__ buffer, size_t bufferlen, size_t & pathlen) {
	bool success = false;
	if (auto fd = Syscall::open(path, O_PATH|O_NONBLOCK|O_CLOEXEC|O_LARGEFILE)) {
		success = absolute(fd.value(), buffer, bufferlen, pathlen);
		Syscall::close(fd.value());
	} else {
		LOG_ERROR << "Unable to open path '" << path << "': " << fd.error_message() << endl;
	}
	return success;
}

Pair<const char *, const char *> pathsplit(char *path) {
	if (path == nullptr || *path == '\0') {
		return Pair<const char *, const char *>{".", "."};
	}
	size_t p = String::len(path) - 1;
	// remove trailing slashes
	for (; path[p] == '/'; p--)
		if (p == 0)
			// path = "/"
			return Pair<const char *, const char *>{path, path};
		else
			path[p] = '\0';

	// find basename (go back to last slahs)
	for (; path[p] != '/'; p--)
		if (p == 0)
			return Pair<const char *, const char *>{".", path};

	// We've got the basename
	char * basename = path + p + 1;
	path[p] = '\0';

	// remove trailing slahes (of basename)
	for (; p > 0 && path[p] == '/'; p--)
		path[p] = '\0';

	return Pair<const char *, const char *>{path, basename};
}

}  // namespace File
