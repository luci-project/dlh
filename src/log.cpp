#include <dlh/log.hpp>
#include <dlh/error.hpp>
#include <dlh/syscall.hpp>

bool Log::output(int fd) {
	auto fcntl = Syscall::fcntl(fd, F_GETFD);
	if (fcntl.value() != -1 || fcntl.error() != EBADF) {
		this->fd = fd;
		return true;
	} else {
		return false;
	}
}

bool Log::output(const char * file, bool truncate) {
	if (file != nullptr) {
		auto fd = Syscall::open(file, O_CREAT | O_WRONLY | O_CLOEXEC | (truncate ? O_TRUNC : O_APPEND));
		if (fd.valid()) {
			if (this->fd > 2)
				Syscall::close(this->fd);
			this->fd = fd.value();
			return true;
		}
	}
	return false;
}

void Log::flush() {
	if (severity <= limit && severity != Level::NONE && _pos > 0) {
		if (fd <= 2)
			*this << "\e[0m";
		OutputStream::flush();
	} else {
		// Drop message
		_pos = 0;
	}
}

static const char * level_name[] = {
	nullptr,
	"\e[41;30;1m FATAL \e[0;40;31m",
	"\e[41;30m ERROR \e[40;31m",
	"\e[43;30mWARNING\e[40;33m",
	"\e[47;30;1m INFO  \e[0;40;37m",
	"\e[47;30mVERBOSE\e[40;37m",
	"\e[7;1m DEBUG \e[0;40m",
	"\e[7m TRACE \e[0;40m"
};

Log& Log::entry(Level level, const char * file, unsigned line, const char * module) {
	flush();
	severity = level;
	if (level <= limit && level > NONE) {
		reset();
		// STDOUT / STDERR with ANSI color codes
		if (fd <= 2) {
			*this << level_name[level];
			size_t p = _pos;
			*this << ' ';
			if (module != nullptr)
				*this << module << ':';
			if (file != nullptr) {
				*this << file;
				if (line > 0)
					*this << ':' << line;
				*this << ' ';
			}
			// Pad file name
			while (_pos - p < 38)
				*this << '.';
			*this << "\e[49m ";
		} else {
			// For files
			*this << level << '\t' << file << '\t' << line << '\t';
		}
	}
	return *this;
}

Log logger;
