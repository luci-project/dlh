#include <dlh/mem.hpp>
#include <dlh/math.hpp>
#include <dlh/mutex.hpp>
#include <dlh/assert.hpp>
#include <dlh/string.hpp>

#include "alloc_buddy.hpp"

namespace Memory {

#ifndef MIN_ALLOC_LOG2
// Minimum size to  alloc
// Default: 2^10 = 1024 Bytes
#define MIN_ALLOC_LOG2 10
#endif

#ifndef MAX_ALLOC_LOG2
// Maximum total space to alloc
// Default: 2^30 = 1GB
#define MAX_ALLOC_LOG2 30
#endif

#ifdef DLH_LEGACY
// Use program break based allocator (with 128 KiB chunks)
static Allocator::Buddy<MIN_ALLOC_LOG2, MAX_ALLOC_LOG2, 128 * 1024, true> allocator;
#else
// Use mmap based allocator (with 4 MiB chunks)
static Allocator::Buddy<MIN_ALLOC_LOG2, MAX_ALLOC_LOG2, 1024 * 4096, false> allocator;
#endif

static Mutex mutex;

uintptr_t alloc(size_t size) {
	if (size == 0) {
		return 0;
	} else {
		Guarded section(mutex);

		uintptr_t addr = allocator.malloc(size);

		// Ensure GLIBC compatibility
		assert(addr % 16 == 0);

		return addr;
	}
}

void free(uintptr_t addr) {
	if (addr != 0) {
		Guarded section(mutex);

		allocator.free(addr);
	}
}

uintptr_t realloc(uintptr_t addr, size_t size) {
	if (addr == 0 && size == 0)
		return 0;

	uintptr_t new_addr = 0;

	Guarded section(mutex);

	// Allocate (of size > 0)
	if (size > 0) {
		if (allocator.resize(addr, size))
			return addr;
		else if ((new_addr = allocator.malloc(size)) != 0 && addr != 0)
			// Copy contents
			copy(new_addr, addr, Math::min(size, allocator.size(addr)));
	}

	// Free old allocation
	if (addr != 0)
		allocator.free(addr);

	// Ensure GLIBC compatibility
	assert(new_addr % 16 == 0);
	return new_addr;
}

uintptr_t alloc_array(size_t nmemb, size_t size) {
	// Catch integer-multiplication overflow
	size_t bytes = nmemb * size;
	if (size == 0 || bytes / size != nmemb)
		return 0;

	// Initialize memory
	uintptr_t new_addr = alloc(bytes);
	if (new_addr != 0)
		set(new_addr, 0, bytes);

	return new_addr;
}

uintptr_t copy(uintptr_t dest, uintptr_t src, size_t size) {
	unsigned char * __restrict__ destination = reinterpret_cast<unsigned char*>(dest);
	unsigned char const * __restrict__ source = reinterpret_cast<unsigned char const*>(src);

	for(size_t i = 0; i != size; ++i) {
		destination[i] = source[i];
	}

	return dest;
}

uintptr_t move(uintptr_t dest, uintptr_t src, size_t size) {
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

uintptr_t set(uintptr_t dest, int pattern, size_t size) {
	unsigned char *destination = reinterpret_cast<unsigned char*>(dest);

	for(size_t i = 0; i != size; ++i) {
		destination[i] = static_cast<unsigned char>(pattern);
	}

	return dest;
}


int compare(uintptr_t s1, uintptr_t s2, size_t n) {
	const unsigned char * c1 = reinterpret_cast<const unsigned char*>(s1);
	const unsigned char * c2 = reinterpret_cast<const unsigned char*>(s2);

	for(size_t i = 0; i != n; ++i)
		if (c1[i] != c2[i])
			return static_cast<int>(c1[i]) - static_cast<int>(c2[i]);

	return 0;
}
}  // namespace Memory
