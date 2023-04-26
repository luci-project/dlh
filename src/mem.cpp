// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include <dlh/mem.hpp>
#include <dlh/log.hpp>
#include <dlh/math.hpp>
#include <dlh/mutex.hpp>
#include <dlh/assert.hpp>
#include <dlh/string.hpp>

#include "alloc_buddy.hpp"

namespace Memory {

#define PAGE_SIZE 0x1000UL

#ifndef MMAP_ALLOC_THREASHOLD
// Threshold to use mmap instead of alloc
#define MMAP_ALLOC_THREASHOLD (32 * PAGE_SIZE)
#endif

#ifndef MMAP_ALLOC_START
// Start address for the mmap base allocator
#define MMAP_ALLOC_START (0x700000000000UL)
#endif

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
static uintptr_t reserve_sbrk(size_t size, uintptr_t & max_ptr) {
	const size_t block_size = 128 * 1024;
	size = Math::align_up(size, block_size);


	if (auto sbrk = Syscall::sbrk(size)) {
		uintptr_t ptr = sbrk.value();
		assert(max_ptr == 0 || max_ptr == ptr);
		max_ptr = ptr + size;
		return ptr;
	} else {
		LOG_WARNING << "Allocator reserve by incrementing program break of " << size << " bytes failed: " << sbrk.error_message() << endl;
		return 0;
	}
}
static Allocator::Buddy<MIN_ALLOC_LOG2, MAX_ALLOC_LOG2, reserve_sbrk> allocator;
#else
// Use mmap based allocator
static uintptr_t reserve_mmap(size_t size, uintptr_t & max_ptr) {
	// Mapping address
	static uintptr_t mmap_addr = MMAP_ALLOC_START;

	uintptr_t ptr = 0;

	// Calculate size uising 1 MiB chunks
	const size_t block_size = 256 * PAGE_SIZE;
	size = Math::align_up(size, block_size);

	if (max_ptr == 0) {
		// Initial mapping
		if (auto mmap = Syscall::mmap(mmap_addr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)) {
			mmap_addr = ptr = mmap.value();
			max_ptr = ptr + size;
		} else {
			LOG_WARNING << "Allocator reserve by mmap of " << size << " bytes at " << reinterpret_cast<void*>(mmap_addr) << " failed: " << mmap.error_message() << endl;
		}
		return ptr;
	} else {
		// Extend mapping

		/* In case we don't have mremap:
		if (auto mmap = Syscall::mmap(max_ptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, 0, 0)) {
			ptr = mmap.value();
			assert(max_ptr == ptr);
			max_ptr += size;
		} else {
			LOG_WARNING << "Allocator reserve by mmap of " << size << " bytes at " << (void*)max_ptr << " failed: " << mmap.error_message() << endl;
		}
		*/

		size_t old_size = max_ptr - mmap_addr;
		size_t new_size = old_size + size;
		assert(old_size % block_size == 0);
		if (auto mremap = Syscall::mremap(mmap_addr, old_size, new_size, 0)) {
			assert(mmap_addr == mremap.value());
			ptr = max_ptr;
			max_ptr += size;
		} else {
			LOG_WARNING << "Allocator reserve by mremap of " << old_size << " bytes to " << new_size << " bytes at " << reinterpret_cast<void*>(mmap_addr) << " failed: " << mremap.error_message() << endl;
		}
	}

	return ptr;
}

static Allocator::Buddy<MIN_ALLOC_LOG2, MAX_ALLOC_LOG2, reserve_mmap> allocator;
#endif

static Mutex mutex;

#ifndef DLH_LEGACY
#define MEMORYMAP_MAGIC 0xDEADBEAFBADF00DUL
class MemoryMap {
	size_t size;
	unsigned long magic;
	char data[];

 public:
	static uintptr_t create(size_t size) {
		size += sizeof(MemoryMap);
		if (auto mmap = Syscall::mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)) {
			MemoryMap * info = reinterpret_cast<MemoryMap *>(mmap.value());
			info->magic = MEMORYMAP_MAGIC;
			info->size = size;
			auto addr = reinterpret_cast<uintptr_t>(info->data);
			// Ensure GLIBC compatibility
			assert(addr % 16 == 0);
			return addr;
		} else {
			LOG_WARNING << "Allocator using mmap of " << size << " bytes failed: " << mmap.error_message() << endl;
			return 0;
		}
	}

	static uintptr_t resize(uintptr_t addr, size_t new_size) {
		new_size += sizeof(MemoryMap);
		uintptr_t start = addr - __builtin_offsetof(MemoryMap, data);
		MemoryMap * old_info = reinterpret_cast<MemoryMap *>(start);
		assert(old_info->magic == MEMORYMAP_MAGIC);
		if (auto mremap = Syscall::mremap(start, old_info->size, new_size, MREMAP_MAYMOVE)) {
			MemoryMap * new_info = reinterpret_cast<MemoryMap *>(mremap.value());
			assert(new_info->magic == MEMORYMAP_MAGIC);
			new_info->size = new_size;
			return reinterpret_cast<uintptr_t>(new_info->data);
		} else {
			LOG_WARNING << "Allocator using mremap of " << old_info->size << " bytes to " << new_size << " bytes at " << reinterpret_cast<void*>(start) << " failed: " << mremap.error_message() << endl;
			return 0;
		}
	}

	static void remove(uintptr_t addr) {
		uintptr_t start = addr - __builtin_offsetof(MemoryMap, data);
		MemoryMap * info = reinterpret_cast<MemoryMap *>(start);
		assert(info->magic == MEMORYMAP_MAGIC);
		size_t size = info->size;
		auto munmap = Syscall::munmap(start, size);
		if (munmap.failed()) {
			LOG_WARNING << "Allocator using munmap of " << size << " bytes at " << reinterpret_cast<void*>(start) << " failed: " << munmap.error_message() << endl;
		}
	}
};
#endif

uintptr_t alloc(size_t size) {
	if (size == 0) {
		return 0;
#ifndef DLH_LEGACY
	} else if (size >= MMAP_ALLOC_THREASHOLD) {
		return MemoryMap::create(size);
#endif
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
#ifndef DLH_LEGACY
		if (allocator.size(addr) == 0) {
			MemoryMap::remove(addr);
		} else  // NOLINT
#endif
		 {
			Guarded section(mutex);

			allocator.free(addr);
		 }
	}
}

uintptr_t realloc(uintptr_t addr, size_t size) {
	if (addr == 0 && size == 0)
		return 0;

	size_t old_size = allocator.size(addr);
	uintptr_t new_addr = 0;

#ifndef DLH_LEGACY
	if ((addr != 0 && old_size == 0) || size >= MMAP_ALLOC_THREASHOLD) {
		if (size == 0) {
			MemoryMap::remove(addr);
		} else if (old_size == 0 && addr != 0) {
			new_addr = MemoryMap::resize(addr, size);
		} else {
			Guarded section(mutex);
			// First try resize in allocator (cheap)
			if (allocator.resize(addr, size)) {
				return addr;
			} else if ((new_addr = MemoryMap::create(size)) != 0 && addr != 0) {
				// Copy contents from allocator to mapping
				copy(new_addr, addr, Math::min(size, old_size));
				// Remove old memory
				allocator.free(addr);
			}
		}
	} else  // NOLINT
#endif
	 {
		assert(addr == 0 || old_size != 0);
		Guarded section(mutex);

		// Allocate (of size > 0)
		if (size > 0) {
			if (old_size != 0 && allocator.resize(addr, size))
				return addr;
			else if ((new_addr = allocator.malloc(size)) != 0 && addr != 0)
				// Copy contents
				copy(new_addr, addr, Math::min(size, old_size));
		}

		// Free old allocation
		if (addr != 0)
			allocator.free(addr);
	 }

	// Ensure GLIBC compatibility
	assert(new_addr % 16 == 0);
	return new_addr;
}

uintptr_t alloc_array(size_t nmemb, size_t size) {
	// Catch integer-multiplication overflow
	size_t bytes = nmemb * size;
	if (size == 0 || bytes / size != nmemb)
		return 0;

#ifndef DLH_LEGACY
	if (bytes >= MMAP_ALLOC_THREASHOLD)
		return MemoryMap::create(bytes);
#endif

	// Initialize memory
	uintptr_t new_addr = alloc(bytes);
	if (new_addr != 0)
		set(new_addr, 0, bytes);

	return new_addr;
}
}  // namespace Memory
