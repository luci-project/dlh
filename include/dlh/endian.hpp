// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

#include "types.hpp"

/*! \brief Helper functions for dealing with endianness
 *
 * \see [Wikipedia Endianess](https://en.wikipedia.org/wiki/Endianness)
 */
namespace Endian {

/*! \brief Generic swap of endianness
 *
 * \tparam T Type of value to be swapped
 * \param v value to be swapped
 * \return Swapped value
 */
template <typename T>
constexpr T swap(T v) {
	union {
		T v;
		unsigned char b[sizeof(T)];
	} s, d;
	s.v = v;
	for (size_t i = 0; i < sizeof(T); i++) {
		d.b[i] = s.b[sizeof(T) - i - 1];
	}
	return d.v;
}

/*! \brief Fast swap of endianness for 16 bit values
 *
 * \param v value to be swapped
 * \return Swapped value
 */
template <>
constexpr uint16_t swap(uint16_t v) {
	return (v << 8) | (v >> 8 );
}

/*! \brief Fast swap of endianness for signed 16 bit values
 *
 * \param v value to be swapped
 * \return Swapped value
 */
template <>
constexpr int16_t swap(int16_t v) {
	return (v << 8) | ((v >> 8) & 0xFF);
}

/*! \brief Fast swap of endianness for 32 bit values
 *
 * \param v value to be swapped
 * \return Swapped value
 */
template <>
constexpr uint32_t swap(uint32_t v) {
	v = ((v << 8) & 0xFF00FF00) | ((v >> 8) & 0xFF00FF);
	return (v << 16) | (v >> 16);
}

/*! \brief Fast swap of endianness for signed 32 bit values
 *
 * \param v value to be swapped
 * \return Swapped value
 */
template <>
constexpr int32_t swap(int32_t v) {
	v = ((v << 8) & 0xFF00FF00) | ((v >> 8) & 0xFF00FF);
	return (v << 16) | ((v >> 16) & 0xFFFF);
}

/*! \brief Fast swap of endianness for 64 bit values
 *
 * \param v value to be swapped
 * \return Swapped value
 */
template <>
constexpr uint64_t swap(uint64_t v) {
	v = ((v << 8) & 0xFF00FF00FF00FF00ULL) | ((v >> 8) & 0x00FF00FF00FF00FFULL);
	v = ((v << 16) & 0xFFFF0000FFFF0000ULL) | ((v >> 16) & 0x0000FFFF0000FFFFULL);
	return (v << 32) | (v >> 32);
}

/*! \brief Fast swap of endianness for signed 64 bit values
 *
 * \param v value to be swapped
 * \return Swapped value
 */
template <>
constexpr int64_t swap(int64_t v) {
	v = ((v << 8) & 0xFF00FF00FF00FF00ULL) | ((v >> 8) & 0x00FF00FF00FF00FFULL);
	v = ((v << 16) & 0xFFFF0000FFFF0000ULL) | ((v >> 16) & 0x0000FFFF0000FFFFULL);
	return (v << 32) | ((v >> 32) & 0xFFFFFFFFULL);
}

}  // namespace Endian
