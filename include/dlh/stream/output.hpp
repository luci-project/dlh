#pragma once

#include <dlh/unistd.hpp>
#include <dlh/stdarg.hpp>
#include <dlh/stream/string.hpp>

template<size_t BUFFERSZ>
class OutputStream : public StringStream<BUFFERSZ> {
 protected:
	int fd;

 public:
	/*! \brief Default constructor  */
	explicit OutputStream(int fd) : fd(fd) {}

	/*! \brief Destructor */
	virtual ~OutputStream() {}

	/*! \brief Flush the buffer.
	 */
	virtual void flush() override {
		size_t l = 0;
		while (l < this->_pos) {
			ssize_t r = ::write(fd, this->_bufptr + l, this->_pos - l);
			if (r < 0)
				break;
			else
				l += r;
		}
		this->_pos = 0;
	}

};

extern OutputStream<1024> cout, cerr;

/*! \brief Wrapper for vprintf
 * \param format format string
 * \param ap value identifying a variable arguments list
 * \return Bytes written (after flush)
 */
inline int vprintf(const char *format, va_list ap) {
	return cout.format(format, ap);
}

/*! \brief Wrapper for printf
 * \param str Target buffer to store the formated string
 * \param size Size of the target buffer
 * \param format format string
 * \return Bytes written (after flush)
 */
inline int printf(const char *format, ...) {
	va_list args;
	va_start(args, format);
	size_t ret = vprintf(format, args);
	va_end(args);
	return ret;
}
