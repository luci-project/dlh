#include <dlh/utils/file.hpp>
#include <dlh/errno.hpp>
#include <dlh/unistd.hpp>
#include <dlh/utils/log.hpp>
#include <dlh/utils/string.hpp>
#include <dlh/stream/buffer.hpp>

namespace File {

bool exists(const char * path) {
	return ::access(path, F_OK ) == 0;
}

bool readable(const char * path) {
	return ::access(path, R_OK ) == 0;
}

bool writeable(const char * path) {
	return ::access(path, W_OK ) == 0;
}

bool executable(const char * path) {
	return ::access(path, X_OK ) == 0;
}

char * contents(const char * path, size_t & size) {
	errno = 0;
	int fd = ::open(path, O_RDONLY);
	if (fd < 0) {
		LOG_ERROR << "Reading file " << path << " failed: " << strerror(errno) << endl;
		return nullptr;
	}

	struct stat sb;
	if (::fstat(fd, &sb) == -1) {
		LOG_ERROR << "Stat file " << path << " failed: " << strerror(errno) << endl;
		::close(fd);
		return nullptr;
	}
	size = sb.st_size;

	void * addr = ::mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	::close(fd);
	if (addr == MAP_FAILED) {
		LOG_ERROR << "Mmap file " << path << " failed: " << strerror(errno) << endl;
		return nullptr;
	} else {
		LOG_VERBOSE << "Mapped '" << path << "' (" << size << " bytes)" << endl;
		return reinterpret_cast<char *>(addr);
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
	int fd = open(path, O_PATH|O_NONBLOCK|O_CLOEXEC|O_LARGEFILE);
	bool success = false;
	if (fd == -1) {
		LOG_ERROR << "Unable to open path '" << path << "': " << strerror(errno) << endl;
	} else {
		StringStream<32> procfd;
		procfd << "/proc/self/fd/" << fd;
		errno = 0;
		auto r = readlink(procfd.str(), buffer, bufferlen - 1);
		if (r == -1) {
			LOG_ERROR << "Unable to get absolute path: " << strerror(errno) << endl;
			r = 0;
			pathlen = 0;
		} else {
			pathlen = static_cast<size_t>(r);
			success = true;
		}
		buffer[r] = '\0';

		close(fd);
	}
	return success;
}

}  // Namespace File
