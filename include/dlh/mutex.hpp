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

	/*! \brief Lock
	 * \param at maximum waiting (relative)
	 * \return `false` if waiting time has expired, `true` otherwise (always if `at` is nullptr)
	 */
	bool lock(const struct timespec * __restrict__ at = nullptr);

	/*! \brief Try to lock (without blocking thread)
	 * \return `true` if lock could be aquired without blocking thread
	 */
	bool trylock();

	/*! \brief Unlock
	 */
	void unlock();
};

class Guarded {
	Mutex & _mutex;

 public:
	Guarded(Mutex & mutex) : _mutex(mutex) { _mutex.lock(); }
	~Guarded() { _mutex.unlock(); }
};
