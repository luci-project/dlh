#include <dlh/alloc.hpp>
#include <dlh/assert.hpp>
#include <dlh/string.hpp>
#include <dlh/utils/math.hpp>
#include <dlh/utils/mutex.hpp>

#include "internal/alloc_buddy.hpp"

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

static Allocator::Buddy<MIN_ALLOC_LOG2, MAX_ALLOC_LOG2> allocator;

static Mutex mutex;

void * malloc(size_t size) {
	if (size == 0) {
		return nullptr;
	} else {
		Guarded section(mutex);

		uintptr_t ptr = allocator.malloc(size);
		return reinterpret_cast<void*>(ptr);
	}
}

void free(void *ptr) {
	if (ptr != nullptr) {
		Guarded section(mutex);

		allocator.free(reinterpret_cast<uintptr_t>(ptr));
	}
}

void *realloc(void *ptr, size_t size) {
	if (ptr == nullptr && size == 0)
		return nullptr;

	void *new_ptr = nullptr;

	Guarded section(mutex);

	// Allocate (of size > 0)
	if (size > 0) {
		if (allocator.resize(reinterpret_cast<uintptr_t>(ptr), size))
			return ptr;
		else if ((new_ptr = reinterpret_cast<void*>(allocator.malloc(size))) != nullptr && ptr != nullptr)
			// Copy contents
			memcpy(new_ptr, ptr, Math::min(size, allocator.size(reinterpret_cast<uintptr_t>(ptr))));
	}

	// Free old allocation
	if (ptr != nullptr)
		allocator.free(reinterpret_cast<uintptr_t>(ptr));

	return new_ptr;
}

void * calloc(size_t nmemb, size_t size) {
	// Catch integer-multiplication overflow
	size_t bytes = nmemb * size;
	if (size == 0 || bytes / size != nmemb) {
		return nullptr;
	}

	// Initialize memory
	void *new_ptr = malloc(bytes);
	if (new_ptr != nullptr)
		memset(new_ptr, 0, bytes);

	return new_ptr;
}
