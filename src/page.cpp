// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include <dlh/page.hpp>

#include <dlh/mem.hpp>
#include <dlh/syscall.hpp>

uintptr_t Page::dup() const {
	if (auto mmap = Syscall::mmap(NULL, length(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)) {
		return Memory::copy(mmap.value() + (addr % SIZE), addr, size);
	}
	return 0;
}
