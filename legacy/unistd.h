#pragma once

// Only DLH_LEGACY supports errno, hence, most of the functions here will only when legacy support is enabled!

// Based on musl libc (and rarely glibc)
#include <errno.h>
#include <dlh/types.hpp>
#include <dlh/systypes.hpp>

// Syscall Wrapper
extern "C" int nanosleep(const struct timespec *req, struct timespec *rem);
extern "C" unsigned sleep(unsigned seconds);

extern "C" pid_t gettid();
extern "C" pid_t getpid();
extern "C" pid_t getppid();

extern "C" int getrlimit(rlimit_t resource, struct rlimit *rlim);
extern "C" int arch_prctl(arch_code_t code, unsigned long addr);
extern "C" int prctl(prctl_t option, unsigned long arg2, unsigned long arg3 = 0, unsigned long arg4 = 0, unsigned long arg5 = 0);

extern "C" int sigaction(int sig, const struct sigaction * __restrict__ sa, struct sigaction * __restrict__ old);
extern "C" int raise(signal_t sig);
extern "C" pid_t wait(int *wstatus);
extern "C" pid_t waitpid(pid_t pid, int *wstatus, int options);
extern "C" [[noreturn]] void abort();
extern "C" [[noreturn]] void crash();
extern "C" [[noreturn]] void exit(int code);

extern "C" void *mmap(void *start, size_t len, int prot, int flags, int fd, long off);
extern "C" void *mremap(void *old_addr, size_t old_len, size_t new_len, int flags, void *new_addr = nullptr);
extern "C" int mprotect(void *addr, size_t len, int prot);
extern "C" int munmap(void *start, size_t len);
extern "C" int msync(void *start, size_t len, int flags);

extern "C" int access(const char *filename, int amode);
extern "C" ssize_t readlink(const char * __restrict__ path, char * __restrict__ buf, size_t bufsize);
extern "C" int open(const char *filename, int flags, int mode = 0);
extern "C" ssize_t read(int fd, void *buf, size_t count);
extern "C" ssize_t write(int fd, const void *buf, size_t size);
extern "C" int close(int fd);
extern "C" ssize_t copy_file_range(int fd_in, off_t *off_in, int fd_out, off_t *off_out, size_t len, unsigned flags = 0);
extern "C" int fcntl(int fd, fcntl_cmd_t cmd, unsigned long arg = 0);
extern "C" int fallocate(int fd, int mode, off_t base, off_t len);
extern "C" int ftruncate(int fd, off_t length);

extern "C" int fstat(int fd, struct stat *st);
extern "C" int stat(const char * __restrict__ path, struct stat * __restrict__ buf);
extern "C" int lstat(const char * __restrict__ path, struct stat * __restrict__ buf);

extern "C" int ioctl(int fd, int req, void * arg = nullptr);
extern "C" int memfd_create(const char *name, unsigned flags);

extern "C" int inotify_init();
extern "C" int inotify_init1(int flags);
extern "C" int inotify_add_watch(int fd, const char *pathname, uint32_t mask);
extern "C" int inotify_rm_watch(int fd, int wd);

extern "C" int pipe(int fd[2]);
extern "C" int pipe2(int fd[2], int flag);
extern "C" pid_t fork();
extern "C" int futex(int * __restrict__ uaddr, futex_op_t futex_op, int val, const void * __restrict__ timeout, int * __restrict__ uaddr2, int val3);

extern "C" int brk(void * addr) __attribute__((weak));
extern "C" void * sbrk(intptr_t inc) __attribute__((weak));

extern "C" long ptrace(int request, pid_t pid, void *addr, void *data);
