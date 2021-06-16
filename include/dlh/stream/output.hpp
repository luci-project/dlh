#pragma once

#include <dlh/unistd.hpp>
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
