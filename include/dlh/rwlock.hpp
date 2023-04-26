// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

#include <dlh/mutex.hpp>

class RWLock {
	unsigned block = 0;
	Mutex reader, global;

 public:
	/*! \brief Lock for reading
	 */
	void read_lock() {
		reader.lock();
		if (++block == 1)
			global.lock();
		reader.unlock();
	}

	/*! \brief Try to lock for reading
	 * \return `true` if lock could be aquired without blocking thread
	 */
	bool read_trylock() {
		if (!reader.trylock())
			return false;

		bool r = true;
		if (++block == 1 && !global.trylock()) {
			block--;
			r = false;
		}
		reader.unlock();
		return r;
	}

	/*! \brief Unlock reader
	 */
	void read_unlock() {
		reader.lock();
		if (--block == 0)
			global.unlock();
		reader.unlock();
	}

	/*! \brief Lock for writing
	 */
	void write_lock() {
		global.lock();
	}

	/*! \brief Try to lock for writing
	 * \return `true` if lock could be aquired without blocking thread
	 */
	bool write_trylock() {
		return global.trylock();
	}

	/*! \brief Unlock writer
	 */
	void write_unlock() {
		global.unlock();
	}
};

class GuardedReader {
	RWLock & _rwlock;

 public:
	explicit GuardedReader(RWLock & rwlock)
	 : _rwlock(rwlock) {
		_rwlock.read_lock();
	}

	~GuardedReader() {
		_rwlock.read_unlock();
	}
};

class GuardedWriter {
	RWLock & _rwlock;

 public:
	explicit GuardedWriter(RWLock & rwlock)
	 : _rwlock(rwlock) {
		_rwlock.write_lock();
	}

	~GuardedWriter() {
		_rwlock.write_unlock();
	}
};
