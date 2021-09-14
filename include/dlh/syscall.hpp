#pragma once

#include <dlh/error.hpp>
#include <dlh/types.hpp>
#include <dlh/systypes.hpp>

// Handy helpers for errors
void warn(const char * msg);
void warn(const char * msg, const char * detail);
[[noreturn]] void die(const char * msg = nullptr, const char * detail = nullptr);

/*! \brief Syscalls with errno in return structure */
namespace Syscall {

/*! \brief Return Value with Errno */
template<typename T>
class ReturnValue {
	T _v;
	Errno _e;

 public:
	ReturnValue() : _e(ENONE) {}
	ReturnValue(Errno e) :  _v((T)(-1)), _e(e) {}
	ReturnValue(T v, Errno e = ENONE) :  _v(v), _e(e) {}
	ReturnValue(const ReturnValue& other) : _v(other._v), _e(other._e) {}
	ReturnValue(ReturnValue&& other) = default;

	inline T* operator->() { return &_v; }
	inline const T* operator->() const { return &_v; }
	inline T& operator*() & { return _v; }
	inline const T& operator*() const & { return _v; }
	inline bool success() const { return _e == ENONE; }
	inline bool failed() const { return _e != ENONE; }
	inline Errno error() const { return _e; }
	inline const char * error_message() const { return __errno_string(_e); }
	inline void warn_on_error(const char * msg = nullptr) const { if (!success()) warn(msg, error_message()); }
	inline void exit_on_error(const char * msg = nullptr) const { if (!success()) die(msg, error_message()); }
	inline void die_on_error(const char * msg = nullptr) const { if (!success()) die(msg, error_message()); }
	inline T& value() & { return _v; }
	inline const T& value() const & { return _v; }
	inline T& value_or_exit(const char * msg = nullptr) & { exit_on_error(msg); return value(); }
	inline const T& value_or_exit(const char * msg = nullptr) const & { exit_on_error(msg); return value(); }
	inline T& value_or_die(const char * msg = nullptr) & { die_on_error(msg); return value(); }
	inline const T& value_or_die(const char * msg = nullptr) const & { die_on_error(msg); return value(); }
	inline T& value_or_default(const T& v) & { return success() ? value() : v; }
	inline const T& value_or_default(const T& v) const & { return success() ? value() : v; }
	inline operator bool() const { return success(); }
	template<typename O> ReturnValue & operator=(const ReturnValue<O>& other) { _e = other._e; _v = other._v; return *this; }
	ReturnValue & operator=(const T& other) { _v = other; return *this; }
	template<typename O> bool operator==(const ReturnValue<O>& other) const { return _e == other._e && _v == other._v; }
	template<typename O> bool operator==(const O& other) const { return _e == ENONE && _v == other; }
	template<typename O> bool operator!=(const ReturnValue<O>& other) const { return _e != other._e && _v != other._v; }
	template<typename O> bool operator!=(const O& other) const { return _e != ENONE || _v != other; }
};


ReturnValue<int> nanosleep(const struct timespec *req, struct timespec *rem);
ReturnValue<unsigned> sleep(unsigned seconds);

pid_t gettid();
pid_t getpid();
pid_t getppid();

ReturnValue<int> getrlimit(rlimit_t resource, struct rlimit *rlim);
ReturnValue<int> arch_prctl(arch_code_t code, unsigned long addr);
ReturnValue<int> prctl(prctl_t option, unsigned long arg2, unsigned long arg3 = 0, unsigned long arg4 = 0, unsigned long arg5 = 0);

ReturnValue<int> sigaction(int sig, const struct sigaction * __restrict__ sa, struct sigaction * __restrict__ old);
ReturnValue<int> raise(signal_t sig);

[[noreturn]] void abort();
[[noreturn]] void crash();
[[noreturn]] void exit(int code);

ReturnValue<uintptr_t> mmap(uintptr_t start, size_t len, int prot, int flags, int fd, long off);
ReturnValue<uintptr_t> mremap(uintptr_t old_addr, size_t old_len, size_t new_len, int flags, uintptr_t new_addr = 0);
ReturnValue<int> mprotect(uintptr_t start, size_t len, int prot);
ReturnValue<int> munmap(uintptr_t start, size_t len);
ReturnValue<int> msync(uintptr_t start, size_t len, int flags);

ReturnValue<int> access(const char *filename, int amode);
ReturnValue<ssize_t> readlink(const char * __restrict__ path, char * __restrict__ buf, size_t bufsize);
ReturnValue<int> open(const char *filename, int flags, int mode = 0);
ReturnValue<ssize_t> read(int fd, void *buf, size_t count);
ReturnValue<ssize_t> write(int fd, const void *buf, size_t size);
ReturnValue<int> close(int fd);
ReturnValue<int> fcntl(int fd, fcntl_cmd_t cmd, unsigned long arg = 0);
ReturnValue<int> fallocate(int fd, int mode, off_t base, off_t len);
ReturnValue<int> ftruncate(int fd, off_t length);

ReturnValue<int> fstat(int fd, struct stat *st);
ReturnValue<int> stat(const char * __restrict__ path, struct stat * __restrict__ buf);
ReturnValue<int> lstat(const char * __restrict__ path, struct stat * __restrict__ buf);

ReturnValue<int> userfaultfd(int flags);
ReturnValue<int> ioctl(int fd, int req, void * arg = nullptr);
ReturnValue<int> memfd_create(const char *name, unsigned flags);

ReturnValue<int> inotify_init(int flags = 0);
ReturnValue<int> inotify_add_watch(int fd, const char *pathname, uint32_t mask);
ReturnValue<int> inotify_rm_watch(int fd, int wd);

ReturnValue<int> futex(int * __restrict__ uaddr, futex_op_t futex_op, int val, const void * __restrict__ timeout, int * __restrict__ uaddr2, int val3);

ReturnValue<int> brk(uintptr_t addr);
ReturnValue<uintptr_t> sbrk(intptr_t inc);
}  // namespace Syscall
