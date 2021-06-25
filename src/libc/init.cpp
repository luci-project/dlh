#include <dlh/types.hpp>
#include <dlh/unistd.hpp>
#include <dlh/utils/thread.hpp>

#include "internal/syscall.hpp"

char **environ;

/*! \brief Function pointer for initialization/finalization functions for global objects
 * required since GCC 4.7 and later.
 *
 * These symbols appear kind of magically due to the compiler
 */
extern void(*__preinit_array_start[]) ();
extern void(*__preinit_array_end[]) ();
extern void(*__init_array_start[]) ();
extern void(*__init_array_end[]) ();
extern void(*__fini_array_start[]) ();
extern void(*__fini_array_end[]) ();

extern "C" void _init() {};
extern "C" void _fini(){};

static Thread main_tcb;

__attribute__((__noinline__)) void __dlh_init(char **envp, char *name) {
	(void)name;
	environ = envp;

	syscall(SYS_arch_prctl, ARCH_SET_FS, reinterpret_cast<uintptr_t>(&main_tcb));

/*
	size_t i, *auxv, aux[AUX_CNT] = { 0 };
	for (i=0; envp[i]; i++);
	libc.auxv = auxv = (void *)(envp+i+1);
	for (i=0; auxv[i]; i+=2) if (auxv[i]<AUX_CNT) aux[auxv[i]] = auxv[i+1];
	__hwcap = aux[AT_HWCAP];
	if (aux[AT_SYSINFO]) __sysinfo = aux[AT_SYSINFO];
	libc.page_size = aux[AT_PAGESZ];

	if (!name) name = (void*)aux[AT_EXECFN];
	if (!name) name = "";
	__progname = __progname_full = name;
	for (i=0; name[i]; i++) if (name[i]=='/') __progname = name+i+1;

	__init_tls(aux);
	__init_ssp((void *)aux[AT_RANDOM]);

	if (aux[AT_UID]==aux[AT_EUID] && aux[AT_GID]==aux[AT_EGID]
		&& !aux[AT_SECURE]) return;

	struct pollfd pfd[3] = { {.fd=0}, {.fd=1}, {.fd=2} };
	int r =
#ifdef SYS_poll
	__syscall(SYS_poll, pfd, 3, 0);
#else
	__syscall(SYS_ppoll, pfd, 3, &(struct timespec){0}, 0, _NSIG/8);
#endif
	if (r<0) a_crash();
	for (i=0; i<3; i++) if (pfd[i].revents&POLLNVAL)
		if (__sys_open("/dev/null", O_RDWR)<0)
			a_crash();
	libc.secure = 1;
	*/
}

extern "C" __attribute__((__used__)) void __dlh_start_main(int (*main)(int,char **,char **), int argc, char **argv, void *atexit) {
	(void)atexit;
	char **envp = argv+argc+1;

	__dlh_init(envp, argv[0]);

	const unsigned int preinit_size = __preinit_array_end - __preinit_array_start;
	for (unsigned int i = 0; i != preinit_size; ++i) {
		(*__preinit_array_start[i])();
	}

	_init();

	const size_t size = __init_array_end - __init_array_start;
	for (size_t i = 0; i < size; i++) {
		(*__init_array_start[i])();
	}

	int r = main(argc, argv, envp);

	const unsigned int fini_size = __fini_array_end - __fini_array_start;
	for (unsigned int i = 0; i != fini_size; ++i) {
		(*__fini_array_start[i])();
	}

	_fini();

	exit(r);
}


asm(R"(
.globl _start
.type _start, @function
.align 16
_start:
	# Set base pointer to zero (ABI)
	xor %rbp, %rbp

	# 4th arg: ptr to register with atexit()
	mov %rdx, %rcx

	# 2nd arg: argument count
	pop %rsi

	# 3rd arg: argument array
	mov %rsp, %rdx

	# Align stack pointer
	andq $-16, %rsp

	# 1st arg: application entry
	mov $main, %rdi

	# call helper function
	call __dlh_start_main

	# Endless loop
1:	jmp 1b
)");


void __stack_chk_fail(void) {
	crash();
}
