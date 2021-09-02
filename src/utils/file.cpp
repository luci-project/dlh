#include <dlh/utils/file.hpp>

#include <dlh/syscall.hpp>
#include <dlh/utils/log.hpp>
#include <dlh/utils/string.hpp>
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

char * contents(const char * path, size_t & size) {
	errno = 0;
	auto fd = Syscall::open(path, O_RDONLY);
	if (!fd.valid()) {
		LOG_ERROR << "Reading file " << path << " failed: " << fd.error_message() << endl;
		return nullptr;
	}

	struct stat sb;
	auto fstat = Syscall::fstat(fd.value(), &sb);
	if (!fstat.success()) {
		LOG_ERROR << "Stat file " << path << " failed: " << fstat.error_message() << endl;
		Syscall::close(fd.value());
		return nullptr;
	}
	size = sb.st_size;

	auto addr = Syscall::mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd.value(), 0);
	Syscall::close(fd.value());
	if (!addr.valid()) {
		LOG_ERROR << "Mmap file " << path << " failed: " << addr.error_message() << endl;
		return nullptr;
	} else {
		LOG_VERBOSE << "Mapped '" << path << "' (" << size << " bytes)" << endl;
		return reinterpret_cast<char *>(addr.value());
	}
}

Vector<const char *> lines(const char * path) {
	size_t size;
	return String::split(File::contents(path, size), '\n');
}

void __procfdname(char *buf, unsigned fd) {
	unsigned i, j;
	for (i=0; (buf[i] = "/proc/self/fd/"[i]); i++);
	if (!fd) {
		buf[i] = '0';
		buf[i+1] = 0;
		return;
	}
	for (j=fd; j; j/=10, i++);
	buf[i] = 0;
	for (; fd; fd/=10) buf[--i] = '0' + fd%10;
}

bool absolute(const char * __restrict__ path, char * __restrict__ buffer, size_t bufferlen, size_t & pathlen) {
	auto fd = Syscall::open(path, O_PATH|O_NONBLOCK|O_CLOEXEC|O_LARGEFILE);
	bool success = false;
	if (!fd.valid()) {
		LOG_ERROR << "Unable to open path '" << path << "': " << fd.error_message() << endl;
	} else {
		StringStream<32> procfd;
		procfd << "/proc/self/fd/" << fd.value();
		auto r = Syscall::readlink(procfd.str(), buffer, bufferlen - 1);
		if (!r.valid()) {
			LOG_ERROR << "Unable to get absolute path: " << r.error_message() << endl;
			buffer[0] = '\0';
			pathlen = 0;
		} else {
			pathlen = static_cast<size_t>(r.value());
			buffer[pathlen] = '\0';
			success = true;
		}
		Syscall::close(fd.value());
	}
	return success;
}

}  // Namespace File
