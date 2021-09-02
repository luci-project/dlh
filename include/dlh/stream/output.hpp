#pragma once

#include <dlh/syscall.hpp>
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
			auto r = Syscall::write(fd, this->_bufptr + l, this->_pos - l);
			if (!r.valid())
				break;
			else
				l += r.value();
		}
		this->_pos = 0;
	}

};

extern OutputStream<1024> cout, cerr;
