#include <dlh/syscall.hpp>
#include <dlh/log.hpp>

#include "syscall.hpp"

namespace Syscall {
template <typename T>
static inline ReturnValue<T> retval(unsigned long r) {
	if (r > -4096UL)
		return { (Errno)(-r) };
	else
		return { (T)(r), ENONE };
}

//TODO use sysconf
#define PAGE_SIZE 4096

ReturnValue<int> nanosleep(const struct timespec *req, struct timespec *rem) {
	return retval<int>(__syscall(SYS_nanosleep, req, rem));
}

ReturnValue<unsigned> sleep(unsigned seconds) {
	struct timespec tv = { .tv_sec = seconds, .tv_nsec = 0 };
	auto r = nanosleep(&tv, &tv);
	return ReturnValue<unsigned>(r ? 0 : tv.tv_sec, r.error());
}


pid_t gettid() {
	return __syscall(SYS_gettid);
}

pid_t getpid() {
	return __syscall(SYS_getpid);
}

pid_t getppid() {
	return __syscall(SYS_getppid);
}



ReturnValue<int> getrlimit(rlimit_t resource, struct rlimit *rlim) {
	unsigned long k_rlim[2];
	auto ret = retval<int>(__syscall(SYS_prlimit64, 0, resource, 0, rlim));
	if (ret.success() || ret.error() != ENOSYS) {
		return ret;
	} else if (__syscall(SYS_getrlimit, resource, k_rlim) < 0) {
		ret = -1;
		return ret;
	} else {
		rlim->rlim_cur = k_rlim[0] == -1UL ? (~0ULL) : k_rlim[0];
		rlim->rlim_max = k_rlim[1] == -1UL ? (~0ULL) : k_rlim[1];
		return { 0 };
	}
}

ReturnValue<int> arch_prctl(arch_code_t code, unsigned long addr) {
	return retval<int>(__syscall(SYS_arch_prctl, code, addr));
}

ReturnValue<int> prctl(prctl_t option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) {
	return retval<int>(__syscall(SYS_prctl, option, arg2, arg3, arg4, arg5));
}


asm(R"(
.type __restore,@function
.align 16
__restore:
	movl $15, %eax
	syscall
)");
extern "C" void __restore();

ReturnValue<int> sigaction(int sig, const struct sigaction * __restrict__ sa, struct sigaction * __restrict__ old) {
	//__syscall(SYS_rt_sigprocmask, SIG_UNBLOCK, 3UL << 32, 0, 8);
	struct sigaction tmp = *sa;
	tmp.sa_flags |= SA_RESTORER;
	tmp.sa_restorer = __restore;
	return retval<int>(__syscall(SYS_rt_sigaction, sig, &tmp, old, sizeof(tmp.sa_mask[0])));
}

ReturnValue<int> raise(signal_t sig) {
	// TODO
	unsigned long set, mask = { 0xfffffffc7fffffff };
	__syscall(SYS_rt_sigprocmask, SIG_BLOCK, &mask, &set, 8);
	auto ret = __syscall(SYS_tgkill, getpid(), gettid(), sig);
	__syscall(SYS_rt_sigprocmask, SIG_SETMASK, &set, 0, 8);
	return retval<int>(ret);
}


[[noreturn]] void abort() {
	raise(SIGABRT);
	raise(SIGKILL);
	crash();
}

[[noreturn]] void crash() {
	asm volatile( "hlt" : : : "memory" );
	*(volatile char *)0=0;
	while(1) {}
	__builtin_unreachable();
}

[[noreturn]] void exit(int code) {
	__syscall(SYS_exit, (long)code);
	__builtin_unreachable();
}

ReturnValue<uintptr_t> mmap(uintptr_t start, size_t len, int prot, int flags, int fd, long off) {
	return retval<uintptr_t>(__syscall(SYS_mmap, start, len, prot, flags, fd, off));
}

ReturnValue<uintptr_t> mremap(uintptr_t old_addr, size_t old_len, size_t new_len, int flags, uintptr_t new_addr) {
	if (new_len >= PTRDIFF_MAX)
		return { ENOMEM };
	return retval<uintptr_t>(__syscall(SYS_mremap, old_addr, old_len, new_len, flags, new_addr));
}

ReturnValue<int> mprotect(uintptr_t addr, size_t len, int prot) {
	size_t start = addr & -PAGE_SIZE;
	size_t end = reinterpret_cast<size_t>(reinterpret_cast<char *>(addr) + len + PAGE_SIZE - 1) & -PAGE_SIZE;
	return retval<int>(__syscall(SYS_mprotect, start, end-start, prot));
}

ReturnValue<int> munmap(uintptr_t start, size_t len) {
	return retval<int>(__syscall(SYS_munmap, start, len));
}

ReturnValue<int> msync(uintptr_t start, size_t len, int flags) {
	return retval<int>(__syscall(SYS_msync, start, len, flags));
}


ReturnValue<int> access(const char *filename, int amode) {
	int r = __syscall(SYS_access, filename, amode);
	if (r == -ENOSYS)
		r = __syscall(SYS_faccessat, AT_FDCWD, filename, amode, 0);
	return retval<int>(r);
}

ReturnValue<ssize_t> readlink(const char * __restrict__ path, char * __restrict__ buf, size_t bufsize) {
	return retval<ssize_t>(__syscall(SYS_readlink, path, buf, bufsize));
}


ReturnValue<int> open(const char *filename, int flags, int mode) {
	int fd = __syscall(SYS_open, filename, flags, mode);
	if (fd >= 0 && (flags & O_CLOEXEC))
		fcntl(fd, F_SETFD, FD_CLOEXEC);

	return retval<int>(fd);
}

ReturnValue<ssize_t> read(int fd, void *buf, size_t count) {
	return retval<ssize_t>(__syscall(SYS_read, fd, buf, count));
}

ReturnValue<ssize_t> write(int fd, const void *buf, size_t size) {
	return retval<ssize_t>(__syscall(SYS_write, fd, buf, size));
}

ReturnValue<int> close(int fd) {
	return retval<int>(__syscall(SYS_close, fd));
}

ReturnValue<int> fcntl(int fd, fcntl_cmd_t cmd, unsigned long arg) {
	switch (cmd) {
		case F_GETOWN:
		case F_DUPFD_CLOEXEC:
			return retval<int>(-ENOSYS);
		default:
			return retval<int>(__syscall(SYS_fcntl, fd, cmd, arg));
	}
}

ReturnValue<int> fallocate(int fd, int mode, off_t base, off_t len) {
	return retval<int>(__syscall(SYS_fallocate, fd, mode, base, len));
}

ReturnValue<int> ftruncate(int fd, off_t length) {
	return retval<int>(__syscall(SYS_ftruncate, fd, length));
}


ReturnValue<int> fstat(int fd, struct stat *st) {
	return retval<int>(__syscall(SYS_fstat, fd, st));
}

ReturnValue<int> stat(const char * __restrict__ path, struct stat * __restrict__ buf) {
	return retval<int>(__syscall(SYS_stat, path, buf));
}

ReturnValue<int> lstat(const char * __restrict__ path, struct stat * __restrict__ buf) {
	return retval<int>(__syscall(SYS_lstat, path, buf));
}


ReturnValue<int> userfaultfd(int flags) {
	return retval<int>(__syscall(SYS_userfaultfd, flags));
}

ReturnValue<int> ioctl(int fd, int req, void * arg) {
	return retval<int>(__syscall(SYS_ioctl, fd, req, arg));
}

ReturnValue<int> memfd_create(const char *name, unsigned flags) {
	return retval<int>(__syscall(SYS_memfd_create, name, flags));
}


ReturnValue<int> inotify_init(int flags) {
	int r = __syscall(SYS_inotify_init1, flags);
	if (r == -ENOSYS && !flags)
		r = __syscall(SYS_inotify_init);
	return retval<int>(r);
}

ReturnValue<int> inotify_add_watch(int fd, const char *pathname, uint32_t mask) {
	return retval<int>(__syscall(SYS_inotify_add_watch, fd, pathname, mask));
}

ReturnValue<int> inotify_rm_watch(int fd, int wd) {
	return retval<int>(__syscall(SYS_inotify_rm_watch, fd, wd));
}


ReturnValue<int> futex(int * __restrict__ uaddr, futex_op_t futex_op, int val, const void * __restrict__ timeout, int * __restrict__ uaddr2, int val3) {
	return retval<int>(__syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3));
}


static uintptr_t curbrk = 0;

ReturnValue<int> brk(uintptr_t addr) {
	if ((curbrk = __syscall(SYS_brk, addr)) < addr)
		return { ENOMEM };
	else
		return { 0 };
}

ReturnValue<uintptr_t> sbrk(intptr_t inc) {
	if (curbrk == 0 && brk(0).failed())
		return { ENOMEM };

	uintptr_t o = curbrk;
	if (inc == 0)
		return { o };
	else if ((inc > 0 && (o + inc < o)) ||
	         (inc < 0 && (o < static_cast<uintptr_t>(-inc))))
		return { ENOMEM };

	if (brk(o + inc).success())
		return { o };
	else
		return { ENOMEM };
}
}  // namespace Syscall


void warn(const char * msg) {
	const int error_fd = 2;
	if (msg != nullptr) {
		size_t len = 0;
		__syscall(SYS_write, error_fd, msg, len);
	}
}

void warn(const char * msg, const char * detail) {
	if (msg == nullptr && detail == nullptr) {
		warn("(error)\n");
	} else {
		warn(msg);
		if (msg != nullptr && detail != nullptr)
			warn(": ");
		warn(detail);
		warn("\n");
	}
}

[[noreturn]] void die(const char * msg, const char * detail) {
	warn(msg, detail);
	Syscall::exit(EXIT_FAILURE);
	Syscall::crash();
}
