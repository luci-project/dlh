#include <dlh/mem.hpp>

#ifdef DLH_LEGACY
extern "C" void * malloc(size_t size) {
	return Memory::alloc<void*>(size);
}

extern "C" void free(void *ptr) {
	Memory::free(ptr);
}

extern "C" void *realloc(void *ptr, size_t size) {
	return Memory::realloc(ptr, size);
}

extern "C" void * calloc(size_t nmemb, size_t size) {
	return reinterpret_cast<void*>(Memory::alloc_array(nmemb, size));
}
#endif
