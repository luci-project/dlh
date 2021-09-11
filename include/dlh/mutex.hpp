#pragma once

#include <dlh/systypes.hpp>

class Mutex {
	enum State : int {
		FUTEX_UNLOCKED,
		FUTEX_LOCKED,
		FUTEX_LOCKED_WITH_WAITERS
	} var;

 public:
	Mutex();

	bool lock(const struct timespec * __restrict__ at = nullptr);

	bool trylock();

	void unlock();
};

class Guarded {
	Mutex & _mutex;

 public:
	Guarded(Mutex & mutex) : _mutex(mutex) { _mutex.lock(); }
	~Guarded() { _mutex.unlock(); }
};
