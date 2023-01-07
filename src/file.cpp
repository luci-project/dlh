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
char * get(const char * path, size_t & size) {
	auto fd = Syscall::open(path, O_RDONLY);
	if (fd.failed()) {
		LOG_ERROR << "Opening file " << path << " failed: " << fd.error_message() << endl;
		return nullptr;
	}

	struct stat sb;
	if (auto fstat = Syscall::fstat(fd.value(), &sb)) {
		size = sb.st_size;
	} else {
		LOG_ERROR << "Stat file " << path << " failed: " << fstat.error_message() << endl;
		Syscall::close(fd.value());
		return nullptr;
	}

	auto addr = Syscall::mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd.value(), 0);
	Syscall::close(fd.value());
	if (addr.failed()) {
		LOG_ERROR << "Mmap file " << path << " failed: " << addr.error_message() << endl;
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
	unsigned i, j;
	for (i=0; (buf[i] = "/proc/self/fd/"[i]); i++);
	if (!fd) {
		buf[i] = '0';
		buf[i+1] = 0;
		return;
	}
	for (j=fd; j; j/=10, i++) {};
	buf[i] = 0;
	for (; fd; fd/=10) buf[--i] = '0' + fd%10;
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
		return { ".", "." };
	}
	size_t p = String::len(path) - 1;
	// remove trailing slashes
	for (; path[p] == '/'; p--)
		if (p == 0)
			// path = "/"
			return { path, path };
		else
			path[p] = '\0';

	// find basename (go back to last slahs)
	for (; path[p] != '/'; p--)
		if (p == 0)
			return { ".", path };

	// We've got the basename
	char * basename = path + p + 1;
	path[p] = '\0';

	// remove trailing slahes (of basename)
	for (; p > 0 && path[p] == '/'; p--)
		path[p] = '\0';

	return { path, basename };
}

}  // Namespace File
