// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include <dlh/mutex.hpp>
#include <dlh/assert.hpp>
#include <dlh/syscall.hpp>
#include <dlh/stream/output.hpp>

Mutex::Mutex() : var(FUTEX_UNLOCKED) {}

bool Mutex::lock(const struct timespec * __restrict__ at) {
	auto state = FUTEX_UNLOCKED;
	if (!__atomic_compare_exchange_n(&var, &state, FUTEX_LOCKED, false, __ATOMIC_RELEASE, __ATOMIC_RELAXED)) {
		if (state != FUTEX_LOCKED_WITH_WAITERS)
			state = __atomic_exchange_n(&var, FUTEX_LOCKED_WITH_WAITERS, __ATOMIC_RELEASE);

		while (state != FUTEX_UNLOCKED) {
			auto futex = Syscall::futex(reinterpret_cast<int*>(&var), FUTEX_WAIT, FUTEX_LOCKED_WITH_WAITERS, at, nullptr, 0);
			switch (futex.error()) {
				case ENONE:
				case EINTR:
				case EAGAIN:
					break;

				case ETIMEDOUT:
					return false;

				default:
					cerr << "Mutex::lock (futex) failed with " << futex.error_message() << endl;
					assert(false);
			}
			state = __atomic_exchange_n(&var, FUTEX_LOCKED_WITH_WAITERS, __ATOMIC_RELEASE);
		}
	}
	return true;
}

bool Mutex::trylock() {
	auto state = FUTEX_UNLOCKED;
	return __atomic_compare_exchange_n(&var, &state, FUTEX_LOCKED, false, __ATOMIC_RELEASE, __ATOMIC_RELAXED);
}

void Mutex::unlock() {
	if (__atomic_exchange_n(&var,  FUTEX_UNLOCKED, __ATOMIC_RELEASE) != FUTEX_LOCKED)
		if (Syscall::futex(reinterpret_cast<int*>(&var), FUTEX_WAKE, 1, nullptr, nullptr, 0).failed())
			assert(false);
}
