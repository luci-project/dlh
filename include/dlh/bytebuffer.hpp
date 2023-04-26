// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

#include <dlh/assert.hpp>
#include <dlh/types.hpp>

template<size_t CAPACITY>
class ByteBuffer {
	uint8_t _buffer[CAPACITY];
	size_t _size;

 public:
	constexpr ByteBuffer() : _size(0) {}

	constexpr const void * buffer() const {
		return _buffer;
	}

	constexpr size_t size() const {
		return _size;
	}

	constexpr size_t capacity() const {
		return CAPACITY;
	}

	template<typename T>
	constexpr size_t push(const T & v) {
		assert(_size + sizeof(T) < CAPACITY);
		*reinterpret_cast<T *>(_buffer + _size) = v;
		_size += sizeof(T);
		return sizeof(T);
	}

	template<typename T>
	constexpr T pop() {
		assert(_size >= sizeof(T));
		T * v = reinterpret_cast<T *>(_buffer + _size);
		_size -= sizeof(T);
		return *v;
	}

	constexpr void clear() {
		_size = 0;
	}
};
