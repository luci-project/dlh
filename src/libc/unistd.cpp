#include <dlh/unistd.hpp>

#include "internal/syscall.hpp"

//TODO use sysconf
#define PAGE_SIZE 4096

extern "C" int nanosleep(const struct timespec *req, struct timespec *rem) {
	return syscall(SYS_nanosleep, req, rem);
}

extern "C" unsigned sleep(unsigned seconds) {
	struct timespec tv = { .tv_sec = seconds, .tv_nsec = 0 };
	if (nanosleep(&tv, &tv))
		return tv.tv_sec;
	return 0;
}

extern "C" pid_t gettid() {
	return syscall(SYS_gettid);
}

extern "C" pid_t getpid() {
	return syscall(SYS_getpid);
}

extern "C" pid_t getppid() {
	return syscall(SYS_getppid);
}

extern "C" int getrlimit(rlimit_t resource, struct rlimit *rlim) {
	unsigned long k_rlim[2];
	int ret = syscall(SYS_prlimit64, 0, resource, 0, rlim);
	if (!ret || errno != ENOSYS)
		return ret;
	if (syscall(SYS_getrlimit, resource, k_rlim) < 0)
		return -1;
	rlim->rlim_cur = k_rlim[0] == -1UL ? (~0ULL) : k_rlim[0];
	rlim->rlim_max = k_rlim[1] == -1UL ? (~0ULL) : k_rlim[1];
	return 0;
}

extern "C" int arch_prctl(arch_code_t code, unsigned long addr) {
	return syscall(SYS_arch_prctl, code, addr);
}

extern "C" int prctl(prctl_t option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) {
	return syscall(SYS_prctl, option, arg2, arg3, arg4, arg5);
}

asm(R"(
.type __restore,@function
.align 16
__restore:
	movl $15, %eax
	syscall
)");
extern "C" void __restore();

extern "C" int sigaction(int sig, const struct sigaction * __restrict__ sa, struct sigaction * __restrict__ old) {
	//__syscall(SYS_rt_sigprocmask, SIG_UNBLOCK, 3UL << 32, 0, 8);
	struct sigaction tmp = *sa;
	tmp.sa_flags |= SA_RESTORER;
	tmp.sa_restorer = __restore;
	return syscall(SYS_rt_sigaction, sig, &tmp, old, sizeof(tmp.sa_mask[0]));
}

extern "C" int raise(signal_t sig) {
	// TODO
	unsigned long set, mask = { 0xfffffffc7fffffff };
	__syscall(SYS_rt_sigprocmask, SIG_BLOCK, &mask, &set, 8);
	int ret = syscall(SYS_tgkill, getpid(), gettid(), sig);
	__syscall(SYS_rt_sigprocmask, SIG_SETMASK, &set, 0, 8);
	return ret;
}

extern "C" [[noreturn]] void abort() {
	raise(SIGABRT);
	raise(SIGKILL);
	crash();
}

extern "C" [[noreturn]] void crash() {
	asm volatile( "hlt" : : : "memory" );
	*(volatile char *)0=0;
	while(1) {}
}

extern "C" [[noreturn]] void exit(int code) {
	syscall(SYS_exit, (long)code);
	__builtin_unreachable();
}


extern "C" void *mmap(void *start, size_t len, int prot, int flags, int fd, long off) {
	return (void *)syscall(SYS_mmap, start, len, prot, flags, fd, off);
}

extern "C" int mprotect(void *addr, size_t len, int prot) {
	size_t start = (size_t)addr & -PAGE_SIZE;
	size_t end = (size_t)((char *)addr + len + PAGE_SIZE - 1) & -PAGE_SIZE;
	return syscall(SYS_mprotect, start, end-start, prot);
}

extern "C" int munmap(void *start, size_t len) {
	return syscall(SYS_munmap, start, len);
}

extern "C" int msync(void *start, size_t len, int flags) {
	return syscall(SYS_msync, start, len, flags);
}



extern "C" int access(const char *filename, int amode) {
	int r = __syscall(SYS_access, filename, amode);
	if (r == -ENOSYS)
		r = __syscall(SYS_faccessat, AT_FDCWD, filename, amode, 0);
	return __syscall_ret(r);
}

extern "C" ssize_t readlink(const char * __restrict__ path, char * __restrict__ buf, size_t bufsize) {
	return syscall(SYS_readlink, path, buf, bufsize);
}

extern "C" int open(const char *filename, int flags, int mode) {
	int fd = __syscall(SYS_open, filename, flags, mode);
	if (fd >= 0 && (flags & O_CLOEXEC))
		fcntl(fd, F_SETFD, FD_CLOEXEC);

	return __syscall_ret(fd);
}

extern "C" ssize_t read(int fd, void *buf, size_t count) {
	return syscall(SYS_read, fd, buf, count);
}

extern "C" ssize_t write(int fd, const void *buf, size_t size) {
	return syscall(SYS_write, fd, buf, size);
}

extern "C" int close(int fd) {
	return syscall(SYS_close, fd);
}

extern "C" int fcntl(int fd, fcntl_cmd_t cmd, unsigned long arg) {
	switch (cmd) {
		case F_GETOWN:
		case F_DUPFD_CLOEXEC:
			return __syscall_ret(-ENOSYS);
		default:
			return syscall(SYS_fcntl, fd, cmd, arg);
	}
}

extern "C" int fallocate(int fd, int mode, off_t base, off_t len) {
	return syscall(SYS_fallocate, fd, mode, base, len);
}

extern "C" int ftruncate(int fd, off_t length) {
	return syscall(SYS_ftruncate, fd, length);
}


extern "C" int fstat(int fd, struct stat *st) {
	return syscall(SYS_fstat, fd, st);
}

extern "C" int stat(const char * __restrict__ path, struct stat * __restrict__ buf) {
	return syscall(SYS_stat, path, buf);
}

extern "C" int lstat(const char * __restrict__ path, struct stat * __restrict__ buf) {
	return syscall(SYS_lstat, path, buf);
}


extern "C" int memfd_create(const char *name, unsigned flags) {
	return syscall(SYS_memfd_create, name, flags);
}


extern "C" int inotify_init() {
	return inotify_init1(0);
}

extern "C" int inotify_init1(int flags) {
	int r = __syscall(SYS_inotify_init1, flags);
	if (r == -ENOSYS && !flags)
		r = __syscall(SYS_inotify_init);
	return __syscall_ret(r);
}

extern "C" int inotify_add_watch(int fd, const char *pathname, uint32_t mask) {
	return syscall(SYS_inotify_add_watch, fd, pathname, mask);
}

extern "C" int inotify_rm_watch(int fd, int wd) {
	return syscall(SYS_inotify_rm_watch, fd, wd);
}

extern "C"  int futex(int * __restrict__ uaddr, futex_op_t futex_op, int val, const void * __restrict__ timeout, int * __restrict__ uaddr2, int val3) {
	return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}


static void * curbrk = 0;

extern "C" int brk(void *addr) {
	curbrk = (void *)syscall(SYS_brk, addr);
	return curbrk < addr ? __syscall_ret(-ENOMEM) : 0;
}

extern "C" void *sbrk(intptr_t inc) {
	if (curbrk == NULL)
		if (brk (0) < 0)
			return (void *) -1;

	void * o = curbrk;
	if (inc == 0)
		return o;
	else if ((inc > 0 && ((uintptr_t) o + (uintptr_t) inc < (uintptr_t) o)) ||
	         (inc < 0 && ((uintptr_t) o < (uintptr_t) -inc)))
		return  (void*)__syscall_ret(-ENOMEM);

	return brk((void*)((uintptr_t)o + inc)) < 0 ? (void *) -1 : o;
}
