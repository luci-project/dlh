// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

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

template <class T = Mutex>
class Guarded {
	T & _mutex;

 public:
	explicit Guarded(T & mutex)
	 : _mutex(mutex) {
		_mutex.lock();
	}

	~Guarded() {
		_mutex.unlock();
	}
};
