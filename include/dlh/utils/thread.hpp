#pragma once

#include <dlh/utility.hpp>
#include <dlh/unistd.hpp>

struct ThreadControlBlock {
	ThreadControlBlock * tcb;
	union DynamicThreadVector {
		size_t counter;
		struct dtv_pointer {
			void *val;
			void *to_free;
		} pointer;

		constexpr DynamicThreadVector(void * val = reinterpret_cast<void*>(-1)) : pointer{val, 0} {}

		inline bool allocated() const {
			return pointer.val != reinterpret_cast<void*>(-1);
		}
	} * dtv;
	void * selfptr;
	int multiple_threads = 1;
	int gscope_flag = 0;
	uintptr_t sysinfo = 0;
	uintptr_t stack_guard = 0x2badc0de;
	uintptr_t pointer_guard = 0x1badc0de;
	void * __padding[81] = { nullptr };

	constexpr ThreadControlBlock(DynamicThreadVector * dtv, void * selfptr = nullptr) : tcb(this), dtv(dtv), selfptr(selfptr) {}
};

static_assert(sizeof(ThreadControlBlock) == 0x2c0, "Wrong ThreadControlBlock size");

class Thread : public ThreadControlBlock {
	bool detached = false;
	int tls_errno = 0;
	int exit_code = 0;
	pid_t tid = -1;
	void *map_base;
	size_t map_size;

 public:
	constexpr Thread(DynamicThreadVector * dtv = nullptr, void * base = nullptr, size_t size = 0, bool detach = false) : ThreadControlBlock(dtv, this), detached(detach), map_base(base), map_size(size) {}

	static Thread * create(int (*func)(void*), void * arg = nullptr, bool detach = false, size_t stack_size = 1048576, size_t tls_size = 0xd40, DynamicThreadVector * dtv = nullptr, bool hidden = false);

	pid_t id() const {
		return tid;
	}

	void detach();

	void exit(int ret = 0);

	int kill(signal_t sig);

	bool join(int & exit_code);

	inline bool join() {
		int e;
		return join(e);
	}


	inline bool active() const {
		return tid > 0;
	}

	static inline Thread * self() {
		struct Thread *s;
		asm volatile("mov %%fs:%c1,%0"
		             : "=r" (s)
		             : "i" (__builtin_offsetof(ThreadControlBlock, selfptr))
		);
		return s;
	}

	static inline int & local_errno() {
		return self()->tls_errno;
	}
};