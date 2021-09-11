#pragma once

#include <dlh/types.hpp>

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
uintptr_t copy(uintptr_t dest, uintptr_t src, size_t size);

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
uintptr_t move(uintptr_t dest, uintptr_t src, size_t size);

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
uintptr_t set(uintptr_t dest, int pattern, size_t size);

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
int compare(uintptr_t s1, uintptr_t s2, size_t n);


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

}  // namespace Memory
