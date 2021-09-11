#include <dlh/mem.hpp>
#include <dlh/assert.hpp>

void *operator new(size_t n) {
	uintptr_t addr = Memory::alloc(n);
	assert(addr != 0);
	return reinterpret_cast<void*>(addr);
}

void *operator new[](size_t n) {
	uintptr_t addr = Memory::alloc(n);
	assert(addr != 0);
	return reinterpret_cast<void*>(addr);
}

void operator delete[](void *ptr) {
	Memory::free(reinterpret_cast<uintptr_t>(ptr));
}

void operator delete[](void *ptr, size_t size) {
	(void) size;
	Memory::free(reinterpret_cast<uintptr_t>(ptr));
}

void operator delete(void *ptr) {
	Memory::free(reinterpret_cast<uintptr_t>(ptr));
}

void operator delete(void *ptr, size_t size) {
	(void) size;
	Memory::free(reinterpret_cast<uintptr_t>(ptr));
}
