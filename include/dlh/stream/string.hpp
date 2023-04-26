// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

#include <dlh/types.hpp>
#include <dlh/stream/buffer.hpp>

template<size_t BUFFERSZ>
class StringStream : public BufferStream {
 protected:
	char buffer[BUFFERSZ];

 public:
	/*! \brief Default constructor  */
	StringStream() : BufferStream(buffer, BUFFERSZ) {}

	/*! \brief Destructor */
	virtual ~StringStream() {}
};
