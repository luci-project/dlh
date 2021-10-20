#include <dlh/syscall.hpp>

#ifdef DLH_LEGACY
extern "C" int *__errno_location();

template <typename T>
static inline T reterrno(const Syscall::ReturnValue<T> & v) {
	if (v.error() != ENONE)
		*__errno_location() = v.error();
	return v.value();
}

extern "C" int nanosleep(const struct timespec *req, struct timespec *rem) {
	return reterrno(Syscall::nanosleep(req, rem));
}

extern "C" unsigned sleep(unsigned seconds) {
	return reterrno(Syscall::sleep(seconds));
}

extern "C" int getrlimit(rlimit_t resource, struct rlimit *rlim) {
	return reterrno(Syscall::getrlimit(resource, rlim));
}

extern "C" int arch_prctl(arch_code_t code, unsigned long addr) {
	return reterrno(Syscall::arch_prctl(code, addr));
}

extern "C" int prctl(prctl_t option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) {
	return reterrno(Syscall::prctl(option, arg2, arg3, arg4, arg5));
}

extern "C" int sigaction(int sig, const struct sigaction * __restrict__ sa, struct sigaction * __restrict__ old) {
	return reterrno(Syscall::sigaction(sig, sa, old));
}

extern "C" int raise(signal_t sig) {
	return reterrno(Syscall::raise(sig));
}

extern "C" void *mmap(void *start, size_t len, int prot, int flags, int fd, long off) {
	return reinterpret_cast<void*>(reterrno(Syscall::mmap(reinterpret_cast<uintptr_t>(start), len, prot, flags, fd, off)));
}

extern "C" void *mremap(void *old_addr, size_t old_len, size_t new_len, int flags, void *new_addr) {
	return reinterpret_cast<void*>(reterrno(Syscall::mremap(reinterpret_cast<uintptr_t>(old_addr), old_len, new_len, flags, reinterpret_cast<uintptr_t>(new_addr))));
}

extern "C" int mprotect(void *addr, size_t len, int prot) {
	return reterrno(Syscall::mprotect(reinterpret_cast<uintptr_t>(addr), len, prot));
}

extern "C" int munmap(void *start, size_t len) {
	return reterrno(Syscall::munmap(reinterpret_cast<uintptr_t>(start), len));
}

extern "C" int msync(void *start, size_t len, int flags) {
	return reterrno(Syscall::msync(reinterpret_cast<uintptr_t>(start), len, flags));
}

extern "C" int access(const char *filename, int amode) {
	return reterrno(Syscall::access(filename, amode));
}

extern "C" ssize_t readlink(const char * __restrict__ path, char * __restrict__ buf, size_t bufsize) {
	return reterrno(Syscall::readlink(path, buf, bufsize));
}

extern "C" int open(const char *filename, int flags, int mode) {
	return reterrno(Syscall::open(filename, flags, mode));
}

extern "C" ssize_t read(int fd, void *buf, size_t count) {
	return reterrno(Syscall::read(fd, buf, count));
}

extern "C" ssize_t write(int fd, const void *buf, size_t size) {
	return reterrno(Syscall::write(fd, buf, size));
}

extern "C" int close(int fd) {
	return reterrno(Syscall::close(fd));
}

extern "C" ssize_t copy_file_range(int fd_in, off_t *off_in, int fd_out, off_t *off_out, size_t len, unsigned flags) {
	return reterrno(Syscall::copy_file_range(fd_in, off_in, fd_out, off_out, len, flags));
}

extern "C" int fcntl(int fd, fcntl_cmd_t cmd, unsigned long arg) {
	return reterrno(Syscall::fcntl(fd, cmd, arg));
}

extern "C" int fallocate(int fd, int mode, off_t base, off_t len) {
	return reterrno(Syscall::fallocate(fd, mode, base, len));
}

extern "C" int ftruncate(int fd, off_t length) {
	return reterrno(Syscall::ftruncate(fd, length));
}

extern "C" int fstat(int fd, struct stat *st) {
	return reterrno(Syscall::fstat(fd, st));
}

extern "C" int stat(const char * __restrict__ path, struct stat * __restrict__ buf) {
	return reterrno(Syscall::stat(path, buf));
}

extern "C" int lstat(const char * __restrict__ path, struct stat * __restrict__ buf) {
	return reterrno(Syscall::lstat(path, buf));
}

extern "C" int ioctl(int fd, int req, void * arg) {
	return reterrno(Syscall::ioctl(fd, req, arg));
}

extern "C" int memfd_create(const char *name, unsigned flags) {
	return reterrno(Syscall::memfd_create(name, flags));
}

extern "C" int inotify_init() {
	return reterrno(Syscall::inotify_init(0));
}

extern "C" int inotify_init1(int flags) {
	return reterrno(Syscall::inotify_init(flags));
}

extern "C" int inotify_add_watch(int fd, const char *pathname, uint32_t mask) {
	return reterrno(Syscall::inotify_add_watch(fd, pathname, mask));
}

extern "C" int inotify_rm_watch(int fd, int wd) {
	return reterrno(Syscall::inotify_rm_watch(fd, wd));
}

extern "C"  int futex(int * __restrict__ uaddr, futex_op_t futex_op, int val, const void * __restrict__ timeout, int * __restrict__ uaddr2, int val3) {
	return reterrno(Syscall::futex(uaddr, futex_op, val, timeout, uaddr2, val3));
}

extern "C" int brk(void *addr) {
	return reterrno(Syscall::brk(reinterpret_cast<uintptr_t>(addr)));
}

extern "C" void *sbrk(intptr_t inc) {
	return reinterpret_cast<void *>(reterrno(Syscall::sbrk(inc)));
}
#endif


extern "C" pid_t gettid() {
	return Syscall::gettid();
}

extern "C" pid_t getpid() {
	return Syscall::getpid();
}

extern "C" pid_t getppid() {
	return Syscall::getppid();
}

extern "C" [[noreturn]] void abort() {
	Syscall::abort();
}

extern "C" [[noreturn]] void crash() {
	Syscall::crash();
}

extern "C" [[noreturn]] void exit(int code) {
	Syscall::exit(code);
}
