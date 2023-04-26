// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

#include <dlh/types.hpp>
#include <dlh/math.hpp>

namespace Memory {
/*! \brief Allocate a memory block
 * The memory is not initialized.
 *
 * \param size Requested size of memory in bytes.
 * \return Address of memory or `0` on error (no memory available) or if size
 *         was zero.
 */
uintptr_t alloc(size_t size);

template<typename T>
inline T* alloc(size_t size) {
	return reinterpret_cast<T*>(alloc(size));
}

/*! \brief Free an allocated memory block
 *
 * \param addr Address of an previously allocated memory block.
 */
void free(uintptr_t addr);

template<typename T>
inline void free(T* addr) {
	return free(reinterpret_cast<uintptr_t>(addr));
}


/*! \brief Change the size of an allocated memory block
 * The contents will be unchanged in the range from the start of the region up
 * to the minimum of the old and new sizes. If the new size is larger than the
 * old size, the added memory will not be initialized.
 *
 * \param addr Address of an previously allocated memory block.
 *             If `0`, then the call is equivalent to \ref alloc().
 * \param size New size of the memory block. If equal to zero, then the call
 *             is equivalent to free
 * \return Address of new memory block or `0` on error
 */
uintptr_t realloc(uintptr_t addr, size_t size);

template<typename T>
inline T* realloc(T* addr, size_t size) {
	return reinterpret_cast<T*>(realloc(reinterpret_cast<uintptr_t>(addr), size));
}

/*! \brief Allocate memory for an array of elements
 * The memory is set to zero.
 *
 * \param nmemb Number of elements
 * \param size Size of an element in bytes
 * \return address of the allocated memory or `0` if the request fails
 */
uintptr_t alloc_array(size_t nmemb, size_t size);

template<typename T>
inline T* alloc_array(size_t nmemb) {
	return reinterpret_cast<T*>(alloc_array(nmemb, sizeof(T)));
}

/*! \brief Copy a memory area
 * \ingroup string
 * \param dest destination buffer
 * \param src source buffer
 * \param size number of bytes to copy
 * \return Address of destination
 * \note The memory must not overlap!
 */
inline uintptr_t copy(uintptr_t dest, uintptr_t src, size_t size) {
	unsigned char * __restrict__ destination = reinterpret_cast<unsigned char*>(dest);
	unsigned char const * __restrict__ source = reinterpret_cast<unsigned char const*>(src);

	for(size_t i = 0; i != size; ++i) {
		destination[i] = source[i];
	}

	return dest;
}

/*! \brief Copy a memory area
 * \ingroup string
 * \param dest destination buffer
 * \param src source buffer
 * \param size number of bytes to copy
 * \return pointer to destination
 * \note The memory must not overlap!
 */
template<typename T, typename U>
inline T* copy(T * __restrict__ dest, U const * __restrict__ src, size_t size) {
	return reinterpret_cast<T*>(copy(reinterpret_cast<uintptr_t>(dest), reinterpret_cast<uintptr_t>(src), size));
}

/*! \brief Copy a memory area
 * while the source may overlap with the destination
 * \ingroup string
 * \param dest destination buffer
 * \param src source buffer
 * \param size number of bytes to copy
 * \return Address of destination
 */
inline uintptr_t move(uintptr_t dest, uintptr_t src, size_t size) {
	unsigned char * __restrict__ destination = reinterpret_cast<unsigned char*>(dest);
	unsigned char const * __restrict__ source = reinterpret_cast<unsigned char const*>(src);

	if(source > destination) {
		for(size_t i = 0; i != size; ++i)
			destination[i] = source[i];
	} else {
		for(size_t i = size; i != 0; --i)
			destination[i-1] = source[i-1];
	}

	return dest;
}

/*! \brief Copy a memory area
 * while the source may overlap with the destination
 * \ingroup string
 * \param dest destination buffer
 * \param src source buffer
 * \param size number of bytes to copy
 * \return pointer to destination
 */
template<typename T, typename U>
inline T* move(T * dest, U const * src, size_t size) {
	return reinterpret_cast<T*>(move(reinterpret_cast<uintptr_t>(dest), reinterpret_cast<uintptr_t>(src), size));
}

/*! \brief Fill a memory area with a pattern
 * \ingroup string
 * \param dest destination buffer
 * \param pattern single byte pattern
 * \param size number of bytes to fill with pattern
 * \return address of destination
 */
inline uintptr_t set(uintptr_t dest, int pattern, size_t size) {
	unsigned char *destination = reinterpret_cast<unsigned char*>(dest);

	for(size_t i = 0; i != size; ++i) {
		destination[i] = static_cast<unsigned char>(pattern);
	}

	return dest;
}

/*! \brief Fill a memory area with a pattern
 * \ingroup string
 * \param dest destination buffer
 * \param pattern single byte pattern
 * \param size number of bytes to fill with pattern
 * \return pointer to destination
 */
template<typename T>
inline T* set(T* dest, int pattern, size_t size) {
	return reinterpret_cast<T*>(set(reinterpret_cast<uintptr_t>(dest), pattern, size));
}

/*! \brief Compare a memory area
 * \ingroup string
 * \param s1 pointer to first element
 * \param s2 pointer to second element
 * \param size number of bytes to compare
 * \return an integer less than, equal to, or greater than zero if the given number of bytes of the first element are
 *          found, respectively, to be less than, to match, or be greater than second element
 */
inline int compare(uintptr_t s1, uintptr_t s2, size_t n) {
	const unsigned char * c1 = reinterpret_cast<const unsigned char*>(s1);
	const unsigned char * c2 = reinterpret_cast<const unsigned char*>(s2);

	for(size_t i = 0; i != n; ++i)
		if (c1[i] != c2[i])
			return static_cast<int>(c1[i]) - static_cast<int>(c2[i]);

	return 0;
}


/*! \brief Compare a memory area
 * \ingroup string
 * \param s1 pointer to first element
 * \param s2 pointer to second element
 * \param size number of bytes to compare
 * \return an integer less than, equal to, or greater than zero if the given number of bytes of the first element are
 *          found, respectively, to be less than, to match, or be greater than second element
 */
template<typename T, typename U>
inline int compare(const T * s1, const U * s2, size_t n) {
	return compare(reinterpret_cast<uintptr_t>(s1), reinterpret_cast<uintptr_t>(s2), n);
}

/*! \brief Compare a memory area
 * \ingroup string
 * \param s1 pointer to first element
 * \param s2 pointer to second element
 * \return an integer less than, equal to, or greater than zero if the given number of bytes of the first element are
 *          found, respectively, to be less than, to match, or be greater than second element
 */
template<typename T, typename U>
inline int compare(const T * s1, const U * s2) {
	int r = compare(s1, s2, Math::min(sizeof(T), sizeof(U)));
	return r == 0 ? sizeof(U) - sizeof(T) : r;
}

/*! \brief Compare a memory area
 * \ingroup string
 * \param s1 reference to first element
 * \param s2 reference to second element
 * \param size number of bytes to compare
 * \return an integer less than, equal to, or greater than zero if the given number of bytes of the first element are
 *          found, respectively, to be less than, to match, or be greater than second element
 */
template<typename T, typename U>
inline int compare(const T & s1, const T & s2, size_t n) {
	return compare(reinterpret_cast<uintptr_t>(&s1), reinterpret_cast<uintptr_t>(&s2), n);
}

/*! \brief Compare a memory area
 * \ingroup string
 * \param s1 reference to first element
 * \param s2 reference to second element
 * \return an integer less than, equal to, or greater than zero if the given number of bytes of the first element are
 *          found, respectively, to be less than, to match, or be greater than second element
 */
template<typename T, typename U>
inline int compare(const T & s1, const U & s2) {
	int r = compare(reinterpret_cast<uintptr_t>(&s1), reinterpret_cast<uintptr_t>(&s2), Math::min(sizeof(T), sizeof(U)));
	return r == 0 ? sizeof(U) - sizeof(T) : r;
}

}  // namespace Memory
