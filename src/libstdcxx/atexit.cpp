#include <dlh/mem.hpp>
#include <dlh/assert.hpp>

static struct ExitFunc {
	void (*func)(void *);
	void * arg;
	void * dso_handle;
} * exit_func = NULL;

static size_t exit_func_len = 0;
static size_t exit_func_cap = 0;

extern "C" int __cxa_atexit(void (*func)(void *), void * arg, void * dso_handle) {
	// TODO: Lock
	if (exit_func_cap >= exit_func_len) {
		exit_func_cap = exit_func_cap >= 32 ? exit_func_cap * 2 : 32;
		exit_func = Memory::realloc(exit_func, exit_func_cap);
		assert(exit_func != NULL);
	}
	auto & f = exit_func[exit_func_len++];
	f.func = func;
	f.arg = arg;
	f.dso_handle = dso_handle;

	return 0;
}

static void call(void *p) {
	((void (*)(void))(uintptr_t)p)();
}

extern "C" int atexit(void (*func)(void)) {
	return __cxa_atexit(call, (void *)(uintptr_t)func, 0);
}
