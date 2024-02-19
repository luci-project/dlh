// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

#include <dlh/systypes.hpp>
#include <dlh/assert.hpp>
#include <dlh/mutex.hpp>

/*! \brief Recursive Mutex
 * Please note: It will not work accross fork
 */
class MutexRecursive {
	Mutex mutex;
	pid_t owner;
	uint64_t recursion;

	bool check() {
		pid_t tid = Syscall::gettid();
		assert(tid > 0);
		if ((owner == tid || (owner == 0 && tid == Syscall::getpid())) && recursion < UINT64_MAX) {
			assert(recursion > 0);
			recursion++;
			return true;
		} else {
			return false;
		}
	}

	void assign() {
		assert(owner == -1);
		assert(recursion == 0);
		recursion = 1;
		owner = Syscall::gettid();
		assert(owner > 0);
		// Single thread hack (required for fork)
		auto pid = Syscall::getpid();
		if (owner == pid)
			owner = 0;
	}

 public:
	MutexRecursive() : owner(-1), recursion(0) {}

	/*! \brief Lock
	 * \param at maximum waiting (relative)
	 * \return `false` if waiting time has expired or not able to get thread id, `true` otherwise
	 */
	bool lock(const struct timespec * __restrict__ at = nullptr) {
		if (check()) {
			return true;
		} else if (mutex.lock(at)) {
			assign();
			return true;
		}

		return false;
	}

	/*! \brief Try to lock (without blocking thread)
	 * \return `true` if lock could be aquired without blocking thread
	 */
	bool trylock() {
		if (check()) {
			return true;
		} else if (mutex.trylock()) {
			assign();
			return true;
		}
		return false;
	}

	/*! \brief Unlock
	 * \note Must be done by owner thread
	 */
	void unlock() {
		assert(recursion > 0);
		assert(owner == 0 || owner == Syscall::gettid());
		if (--recursion == 0) {
			owner = -1;
			mutex.unlock();
		}
	}
};
