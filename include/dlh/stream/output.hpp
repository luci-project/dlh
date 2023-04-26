// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

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
	void flush() override {
		size_t l = 0;
		while (l < this->_pos)
			if (auto r = Syscall::write(fd, this->_bufptr + l, this->_pos - l))
				l += r.value();
			else
				break;
		this->_pos = 0;
	}
};

extern OutputStream<1024> cout, cerr;
