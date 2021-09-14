#include <dlh/mem.hpp>
#include <dlh/log.hpp>
#include <dlh/math.hpp>
#include <dlh/mutex.hpp>
#include <dlh/assert.hpp>
#include <dlh/string.hpp>

#include "alloc_buddy.hpp"

namespace Memory {

#ifndef MMAP_ALLOC_THREASHOLD
// Threshold to use mmap instead of alloc
#define MMAP_ALLOC_THREASHOLD (32 * 0x1000)
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

	/*! \brief Start address for mmap
	 * The initial default address is an advice to the memory mapping for the start address
	 */



	/*! \brief Reserve additional memory
	 * \param size the requested memory size
	 * \return address of the start of the newly allocated memory
	 */


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
	static uintptr_t mmap_addr = 0x700000000000UL;

	uintptr_t ptr = 0;

	// Unmap if size == 0
	if (size == 0) {
		if (max_ptr != 0) {
			auto munmap = Syscall::munmap(mmap_addr, max_ptr - mmap_addr);
			if (munmap.failed())
				LOG_WARNING << "Allocator reserve cleanup using munmap of " << (max_ptr - mmap_addr) << " bytes at " << (void*)mmap_addr << " failed: " << munmap.error_message() << endl;
			max_ptr = 0;
		}
	} else {
		// Calculate size uising 1 MiB chunks
		const size_t block_size = 256 * 0x1000;
		size = Math::align_up(size, block_size);

		if (max_ptr == 0) {
			// Initial mapping
			if (auto mmap = Syscall::mmap(mmap_addr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)) {
				mmap_addr = ptr = mmap.value();;
				max_ptr = ptr + size;
			} else {
				LOG_WARNING << "Allocator reserve by mmap of " << size << " bytes at " << (void*)mmap_addr << " failed: " << mmap.error_message() << endl;
			}
			return ptr;
		} else {
			// Extend mapping

			/* In case we don't have mremap:
			if (auto mmap = Syscall::mmap(max_ptr, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, 0, 0)) {
				ptr = mmap.value();
				assert(max_ptr == ptr);
				max_ptr += size;
			} else {
				LOG_WARNING << "Allocator reserve by mmap of " << size << " bytes at " << (void*)max_ptr << " failed: " << mmap.error_message() << endl;
			}
			*/

			size_t old_size = max_ptr - mmap_addr;
			assert(old_size % block_size == 0);
			if (auto mremap = Syscall::mremap(mmap_addr, old_size, old_size + size, 0)) {
				assert(mmap_addr == mremap.value());
				ptr = max_ptr;
				max_ptr += size;
			} else {
				LOG_WARNING << "Allocator reserve by mremap of " << old_size << " bytes to " << (old_size + size) << " bytes at " << (void*)mmap_addr << " failed: " << mremap.error_message() << endl;
			}
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
		uintptr_t start = addr - __builtin_offsetof(MemoryMap, data);
		MemoryMap * old_info = reinterpret_cast<MemoryMap *>(start);
		assert(old_info->magic == MEMORYMAP_MAGIC);
		if (auto mremap = Syscall::mremap(start, old_info->size, new_size, MREMAP_MAYMOVE)) {
			MemoryMap * new_info = reinterpret_cast<MemoryMap *>(mremap.value());
			assert(new_info->magic == MEMORYMAP_MAGIC);
			new_info->size = new_size;
			return reinterpret_cast<uintptr_t>(new_info->data);
		} else {
			LOG_WARNING << "Allocator using mremap of " << old_info->size << " bytes to " << new_size << " bytes at " << (void*)start << " failed: " << mremap.error_message() << endl;
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
			LOG_WARNING << "Allocator using munmap of " << size << " bytes at " << (void*)start << " failed: " << munmap.error_message() << endl;
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
		} else
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
			} else if ((new_addr = MemoryMap::create(size)) && addr != 0) {
				// Copy contents from allocator to mapping
				copy(new_addr, addr, Math::min(size, old_size));
				// Remove old memory
				allocator.free(addr);
			}
		}
	} else
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
