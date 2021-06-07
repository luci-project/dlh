#pragma once

// Based on musl libc (and rarely glibc)
#include <dlh/errno.hpp>
#include <dlh/types.hpp>

// Architecture-specific state (arch_prctl)
typedef enum : int {
	ARCH_SET_GS = 0x1001,
	ARCH_SET_FS = 0x1002,
	ARCH_GET_FS = 0x1003,
	ARCH_GET_GS = 0x1004
} arch_code_t;

// access
enum : int {
	F_OK = 0,
	X_OK = 1,
	W_OK = 2,
	R_OK = 4
};

// futex
struct timespec {
	long int tv_sec, tv_nsec;
};

typedef enum : int {
	FUTEX_WAIT            =  0,
	FUTEX_WAKE            =  1,
	FUTEX_FD              =  2,
	FUTEX_REQUEUE         =  3,
	FUTEX_CMP_REQUEUE     =  4,
	FUTEX_WAKE_OP         =  5,
	FUTEX_LOCK_PI         =  6,
	FUTEX_UNLOCK_PI       =  7,
	FUTEX_TRYLOCK_PI      =  8,
	FUTEX_WAIT_BITSET     =  9,
	FUTEX_WAKE_BITSET     = 10,
	FUTEX_WAIT_REQUEUE_PI = 11,
	FUTEX_CMP_REQUEUE_PI  = 12
} futex_op_t;

// Process ID function (get*id)
typedef int pid_t;

// Ressource limit (getrlimit)
struct rlimit {
	unsigned long rlim_cur;
	unsigned long rlim_max;
};


// File function (f*)
typedef long int off_t;
#define O_RDONLY           00
#define O_WRONLY           01
#define O_RDWR             02
#define O_CREAT          0100
#define O_EXCL           0200
#define O_TRUNC         01000
#define O_APPEND        02000
#define O_NONBLOCK      04000

#define O_SYNC       04010000
#define O_ASYNC        020000
#define O_LARGEFILE   0100000
#define O_DIRECTORY   0200000
#define O_NOFOLLOW    0400000
#define O_CLOEXEC    02000000
#define O_DIRECT       040000
#define O_NOATIME    01000000
#define O_PATH      010000000
#define O_DSYNC        010000
#define O_TMPFILE  (020000000 | __O_DIRECTORY)

// (f*at)
#define AT_FDCWD                -100
#define AT_SYMLINK_NOFOLLOW    0x100
#define AT_REMOVEDIR           0x200
#define AT_SYMLINK_FOLLOW      0x400
#define AT_NO_AUTOMOUNT        0x800
#define AT_EMPTY_PATH         0x1000
#define AT_STATX_SYNC_TYPE    0x6000
#define AT_STATX_SYNC_AS_STAT 0x0000
#define AT_STATX_FORCE_SYNC   0x2000
#define AT_STATX_DONT_SYNC    0x4000
#define AT_RECURSIVE          0x8000
#define AT_EACCESS             0x200

// (fcntl)
typedef enum : int {
	F_DUPFD            =    0,
	F_GETFD            =    1,
	F_SETFD            =    2,
	F_GETFL            =    3,
	F_SETFL            =    4,
	F_SETOWN           =    8,
	F_GETOWN           =    9,
	F_SETSIG           =   10,
	F_GETSIG           =   11,
	F_SETOWN_EX        =   15,
	F_GETOWN_EX        =   16,
	F_SETLEASE         = 1024,
	F_GETLEASE         = 1025,
	F_NOTIFY           = 1026,
	F_SETPIPE_SZ       = 1031,
	F_GETPIPE_SZ       = 1032,
	F_ADD_SEALS        = 1033,
	F_GET_SEALS        = 1034,
	F_GET_RW_HINT      = 1035,
	F_SET_RW_HINT      = 1036,
	F_GET_FILE_RW_HINT = 1037,
	F_SET_FILE_RW_HINT = 1038,
	F_DUPFD_CLOEXEC    = 1030,
} fcntl_cmd_t;

#define FD_CLOEXEC 1
#define F_RDLCK  0
#define F_WRLCK  1
#define F_UNLCK  2

#define F_SEAL_SEAL         0x0001
#define F_SEAL_SHRINK       0x0002
#define F_SEAL_GROW         0x0004
#define F_SEAL_WRITE        0x0008
#define F_SEAL_FUTURE_WRITE 0x0010

// File stat (*stat)
struct stat {
	unsigned long   st_dev;
	unsigned long   st_ino;
	unsigned        st_mode;
	unsigned long   st_nlink;
	unsigned        st_uid;
	unsigned        st_gid;
	unsigned long   st_rdev;
	off_t           st_size;
	long            st_blksize;
	long            st_blocks;
	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
};

// Memory Mapping (m*)
#define PROT_NONE       0x0
#define PROT_READ       0x1
#define PROT_WRITE      0x2
#define PROT_EXEC       0x4

#define PROT_GROWSDOWN  0x01000000
#define PROT_GROWSUP    0x02000000

#define MAP_FAILED    ((void*)-1)

#define MAP_SHARED    0x01
#define MAP_PRIVATE   0x02
#define MAP_FIXED     0x10
#define MAP_ANONYMOUS 0x20

#define MS_ASYNC      1
#define MS_INVALIDATE 2
#define MS_SYNC       4

// Signals
typedef enum : int {
	SIGHUP    =  1,
	SIGINT    =  2,
	SIGQUIT   =  3,
	SIGILL    =  4,
	SIGTRAP   =  5,
	SIGABRT   =  6,
	SIGIOT    =  6,
	SIGBUS    =  7,
	SIGFPE    =  8,
	SIGKILL   =  9,
	SIGUSR1   = 10,
	SIGSEGV   = 11,
	SIGUSR2   = 12,
	SIGPIPE   = 13,
	SIGALRM   = 14,
	SIGTERM   = 15,
	SIGSTKFLT = 16,
	SIGCHLD   = 17,
	SIGCONT   = 18,
	SIGSTOP   = 19,
	SIGTSTP   = 20,
	SIGTTIN   = 21,
	SIGTTOU   = 22,
	SIGURG    = 23,
	SIGXCPU   = 24,
	SIGXFSZ   = 25,
	SIGVTALRM = 26,
	SIGPROF   = 27,
	SIGWINCH  = 28,
	SIGIO     = 29,
	SIGPOLL   = 29,
	SIGPWR    = 30,
	SIGSYS    = 31,
	SIGUNUSED = 31,
} signal_t;

// Signal handling
#define SIG_BLOCK          0	/* for blocking signals */
#define SIG_UNBLOCK        1	/* for unblocking signals */
#define SIG_SETMASK        2	/* for setting the signal mask */




// Syscall Wrapper
extern "C" pid_t gettid();
extern "C" pid_t getpid();
extern "C" pid_t getppid();

extern "C" int getrlimit(int resource, struct rlimit *rlim);
extern "C" int arch_prctl(arch_code_t code, unsigned long addr);

extern "C" int raise(signal_t sig);
extern "C" [[noreturn]] void abort();
extern "C" [[noreturn]] void exit(int code);

extern "C" void *mmap(void *start, size_t len, int prot, int flags, int fd, long off);
extern "C" int mprotect(void *addr, size_t len, int prot);
extern "C" int munmap(void *start, size_t len);
extern "C" int msync(void *start, size_t len, int flags);

extern "C" int access(const char *filename, int amode);
extern "C" int open(const char *filename, int flags);
extern "C" ssize_t read(int fd, void *buf, size_t count);
extern "C" ssize_t write(int fd, const void *buf, size_t size);
extern "C" int close(int fd);
extern "C" int fcntl(int fd, fcntl_cmd_t cmd, unsigned long arg = 0);
extern "C" int fallocate(int fd, int mode, off_t base, off_t len);
extern "C" int ftruncate(int fd, off_t length);

extern "C" int fstat(int fd, struct stat *st);
extern "C" int stat(const char * __restrict__ path, struct stat * __restrict__ buf);
extern "C" int lstat(const char * __restrict__ path, struct stat * __restrict__ buf);

extern "C" int memfd_create(const char *name, unsigned flags);

extern "C" int inotify_init();
extern "C" int inotify_init1(int flags);
extern "C" int inotify_add_watch(int fd, const char *pathname, uint32_t mask);
extern "C" int inotify_rm_watch(int fd, int wd);

extern "C" int futex(int * __restrict__ uaddr, futex_op_t futex_op, int val, const void * __restrict__ timeout, int * __restrict__ uaddr2, int val3);

extern "C" int brk(void * addr) __attribute__((weak));
extern "C" void * sbrk(intptr_t inc) __attribute__((weak));
