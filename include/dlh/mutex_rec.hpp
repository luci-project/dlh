#pragma once

#include <dlh/systypes.hpp>
#include <dlh/mutex.hpp>

class MutexRecursive {
	Mutex mutex;
	pid_t owner;
	uint64_t recursion;

 public:
	MutexRecursive() : owner(-1), recursion(0) {};

	/*! \brief Lock
	 * \param at maximum waiting (relative)
	 * \return `false` if waiting time has expired or not able to get thread id, `true` otherwise
	 */
	bool lock(const struct timespec * __restrict__ at = nullptr) {
		if (auto me = Syscall::gettid()) {
			if (owner == me && recursion < UINT64_MAX) {
				recursion++;
				return true;
			} else if (mutex.lock(at)) {
				recursion = 1;
				owner = me;
				return true;
			}
		}
		return false;
	}

	/*! \brief Try to lock (without blocking thread)
	 * \return `true` if lock could be aquired without blocking thread
	 */
	bool trylock() {
		if (auto me = Syscall::gettid()) {
			if (owner == me && recursion < UINT64_MAX) {
				recursion++;
				return true;
			} else if (mutex.trylock()) {
				recursion = 1;
				owner = me;
				return true;
			}
		}
		return false;
	}

	/*! \brief Unlock
	 * \note Must be done by owner thread
	 */
	void unlock() {
		if (--recursion == 0) {
			owner = -1;
			mutex.unlock();
		}
	}
};
