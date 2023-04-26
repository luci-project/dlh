// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include <dlh/thread.hpp>
#include <dlh/math.hpp>
#include <dlh/assert.hpp>
#include <dlh/string.hpp>
#include <dlh/syscall.hpp>
#include <dlh/systypes.hpp>
#include <dlh/stream/output.hpp>
#include "syscall.hpp"


extern "C" void __start_child(Thread * that, void* (*func)(void*), void * arg) {
	struct Thread * tcb;
	asm volatile("mov %%fs:0,%0" : "=r" (tcb));
	assert(tcb == that);

	assert(Thread::self() == that);

	void* r = func(arg);

	that->exit(r);
	Syscall::exit(0);
}

Thread * Thread::create(void* (*func)(void*), void * arg, bool detach, bool separate, bool hidden, size_t stack_size, size_t tls_size, DynamicThreadVector * dtv) {
	unsigned flags = CLONE_VM | CLONE_SETTLS | CLONE_FILES | CLONE_FS | CLONE_IO | CLONE_SYSVSEM | CLONE_PARENT_SETTID | CLONE_CHILD_CLEARTID;

	if (!separate)
		flags |= CLONE_THREAD | CLONE_SIGHAND;

	if (hidden)
		flags |= CLONE_UNTRACED;
	else
		flags |= CLONE_PTRACE;

	auto tls_size_aligned = Math::align(tls_size, 64);
	size_t map_size = tls_size_aligned + Math::align(sizeof(Thread), 64) + Math::align(stack_size, 16);
	if (auto mmap = Syscall::mmap(0, map_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0)) {
		register Thread * that __asm__("r8") = reinterpret_cast<Thread *>(mmap.value() + tls_size_aligned);
		if (new (that) Thread(dtv, mmap.value(), map_size, detach) == that) {
			that->multiple_threads = 1;
			void** top_of_stack = reinterpret_cast<void**>(mmap.value() + map_size - 16);
			*(--top_of_stack) = arg;
			*(--top_of_stack) = reinterpret_cast<void*>(func);
			*(--top_of_stack) = that;

			register pid_t * child_tid __asm__("r10") = &(that->tid);

			pid_t pid;
			asm volatile ("syscall\n\t"
			              "test %%eax,%%eax\n\t"
			              "jnz 1f\n\t"
			              "xor %%ebp,%%ebp\n\t"
			              "pop %%rdi\n\t"
			              "pop %%rsi\n\t"
			              "pop %%rdx\n\t"
			              "call __start_child\n\t"
			              "1:;"
			              : "=a"(pid) : "a"(SYS_clone), "D"(flags), "S"(top_of_stack), "d"(&(that->tid)), "r"(child_tid), "r"(that): "rcx", "r11", "memory");
			if (pid > 0) {
				assert(pid == that->tid);
				return that;
			}
		}
		// Clean up on failure
		Syscall::munmap(mmap.value(), map_size);
	}
	return nullptr;
}

void Thread::exit(void * result) {
	assert(self() == this);
	this->result = result;

/*
	in ASM:
	if (detached() && map_base != nullptr)
		munmap(mem, size);
	exit(0);
*/
	asm volatile ("test %%edx,%%edx\n\t"
	              "jz 1f\n\t"
	              "syscall\n\t"
	              "1:;\n\t"
	              "xor %%rdi,%%rdi\n\t"
	              "mov %4,%%rax\n\t"
	              "syscall\n\t"
	              "hlt\n\t"
	              :: "a"(SYS_munmap), "D"(map_base), "S"(map_size), "d"(detached() && map_base != 0), "i"(SYS_exit): "rcx", "r11", "memory");
}

int Thread::kill(signal_t sig) {
	if (!active())
		return ESRCH;
	else
		return static_cast<int>(-__syscall(SYS_tkill, tid, sig));
}

void Thread::detach() {
	if (!detached()) {
		// TODO: racy without locks...
		if (tid <= 0) {
			if (map_base != 0)
				Syscall::munmap(map_base, map_size);
		} else {
			this->joindid = this;
		}
	}
}


bool Thread::join(void ** result) {
	assert(!detached());
	if (tid != -1) {
		pid_t tmp;
		while ((tmp = tid) > 0)
			if (Syscall::futex(reinterpret_cast<int*>(&tid), FUTEX_WAIT, tmp, nullptr, nullptr, 0).value() != 0)
				return false;

		if (__atomic_exchange_n(&tid, -1, __ATOMIC_RELEASE) == 0) {
			if (result != nullptr)
				*result = this->result;
			if (!detached() && map_base != 0)
				Syscall::munmap(map_base, map_size);
			return true;
		}
	}
	return false;
}

void Thread::setup_guards(void* random) {
	Memory::copy(&stack_guard, random, sizeof(stack_guard));
	stack_guard &= ~static_cast<uintptr_t>(0xff);
	Memory::copy(&pointer_guard, reinterpret_cast<char*>(random) + sizeof(pointer_guard), sizeof(pointer_guard));
}
