#include <dlh/utils/thread.hpp>
#include <dlh/utils/math.hpp>
#include <dlh/assert.hpp>
#include <dlh/string.hpp>
#include <dlh/syscall.hpp>
#include <dlh/stream/output.hpp>
#include "../libc/internal/syscall.hpp"

// Clone
#define CLONE_VM             0x00000100	/* Set if VM shared between processes.  */
#define CLONE_FS             0x00000200	/* Set if fs info shared between processes.  */
#define CLONE_FILES          0x00000400	/* Set if open files shared between processes.  */
#define CLONE_SIGHAND        0x00000800	/* Set if signal handlers shared.  */
#define CLONE_PIDFD          0x00001000	/* Set if a pidfd should be placed in parent.  */
#define CLONE_PTRACE         0x00002000	/* Set if tracing continues on the child.  */
#define CLONE_VFORK          0x00004000	/* Set if the parent wants the child to wake it up on mm_release.  */
#define CLONE_PARENT         0x00008000	/* Set if we want to have the same parent as the cloner.  */
#define CLONE_THREAD         0x00010000	/* Set to add to same thread group.  */
#define CLONE_NEWNS          0x00020000	/* Set to create new namespace.  */
#define CLONE_SYSVSEM        0x00040000	/* Set to shared SVID SEM_UNDO semantics.  */
#define CLONE_SETTLS         0x00080000	/* Set TLS info.  */
#define CLONE_PARENT_SETTID  0x00100000	/* Store TID in userlevel buffer before MM copy.  */
#define CLONE_CHILD_CLEARTID 0x00200000	/* Register exit futex and memory location to clear.  */
#define CLONE_DETACHED       0x00400000	/* Create clone detached.  */
#define CLONE_UNTRACED       0x00800000	/* Set if the tracing process can't force CLONE_PTRACE on this clone.  */
#define CLONE_CHILD_SETTID   0x01000000	/* Store TID in userlevel buffer in the child.  */
#define CLONE_NEWCGROUP      0x02000000	/* New cgroup namespace.  */
#define CLONE_NEWUTS         0x04000000	/* New utsname group.  */
#define CLONE_NEWIPC         0x08000000	/* New ipcs.  */
#define CLONE_NEWUSER        0x10000000	/* New user namespace.  */
#define CLONE_NEWPID         0x20000000	/* New pid namespace.  */
#define CLONE_NEWNET         0x40000000	/* New network namespace.  */
#define CLONE_IO             0x80000000	/* Clone I/O context.  */


extern "C" void __start_child(Thread * that, int (*func)(void*), void * arg) {
	struct Thread * tcb;
	asm volatile("mov %%fs:0,%0" : "=r" (tcb));
	assert(tcb == that);

	assert(Thread::self() == that);

	int r = func(arg);

	that->exit(r);
	Syscall::exit(r);
}

Thread * Thread::create(int (*func)(void*), void * arg, bool detach, size_t stack_size, size_t tls_size, DynamicThreadVector * dtv, bool hidden) {
	unsigned flags = /* CLONE_THREAD | CLONE_SIGHAND | */ CLONE_VM | CLONE_SETTLS | CLONE_FILES | CLONE_FS | CLONE_IO | CLONE_SYSVSEM | CLONE_PARENT_SETTID | CLONE_CHILD_CLEARTID;

	if (hidden)
		flags |= CLONE_UNTRACED;

	auto tls_size_aligned = Math::align(tls_size, 16);
	size_t map_size = tls_size_aligned + Math::align(sizeof(Thread), 16) + Math::align(stack_size, 16);
	auto mmap = Syscall::mmap(0, map_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
	if (!mmap.success())
		return nullptr;


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
	return nullptr;
}

void Thread::exit(int ret) {
	assert(self() == this);
	this->exit_code = ret;

/*
	in ASM:
	if (detached && map_base != nullptr)
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
	              :: "a"(SYS_munmap), "D"(map_base), "S"(map_size), "d"(detached && map_base != 0), "i"(SYS_exit): "rcx", "r11", "memory");
}

int Thread::kill(signal_t sig) {
	if (!active())
		return ESRCH;
	else
		return -__syscall(SYS_tkill, tid, sig);
}

void Thread::detach() {
	if (!detached) {
		// TODO: racy without locks...
		if (tid <= 0) {
			if (map_base != 0)
				Syscall::munmap(map_base, map_size);
		} else {
			detached = true;
		}
	}
}


bool Thread::join(int & exit_code) {
	assert(!detached);
	if (tid != -1) {
		pid_t tmp;
		while ((tmp = tid) > 0)
			if (Syscall::futex((int*)&tid, FUTEX_WAIT, tmp, nullptr, NULL, 0).value() != 0)
				return false;

		if (__atomic_exchange_n(&tid, -1, __ATOMIC_RELEASE) == 0) {
			exit_code = this->exit_code;
			if (!detached && map_base != 0)
				Syscall::munmap(map_base, map_size);
			return true;
		}
	}
	return false;
}

void Thread::setup_guards(void* random) {
	memcpy(&stack_guard, random, sizeof(stack_guard));
	stack_guard &= ~(uintptr_t) 0xff;
	memcpy(&pointer_guard, (char*)random + sizeof(pointer_guard), sizeof(pointer_guard));
}
