#pragma once

// Based on musl libc (and rarely glibc)
#include <dlh/errno.hpp>
#include <dlh/types.hpp>

extern char **environ;

#define NAME_MAX 4096
#define PATH_MAX 4096

// Architecture-specific state (arch_prctl)
typedef enum : int {
	ARCH_SET_GS = 0x1001,
	ARCH_SET_FS = 0x1002,
	ARCH_GET_FS = 0x1003,
	ARCH_GET_GS = 0x1004
} arch_code_t;


#define PR_UNALIGN_NOPRINT	1	/* silently fix up unaligned user accesses */
#define PR_UNALIGN_SIGBUS	2	/* generate SIGBUS on unaligned user access */
#define PR_FPEMU_NOPRINT	1	/* silently emulate fp operations accesses */
#define PR_FPEMU_SIGFPE	2	/* don't emulate fp operations, send SIGFPE instead */
#define PR_FP_EXC_SW_ENABLE	0x80	/* Use FPEXC for FP exception enables */
#define PR_FP_EXC_DIV		0x010000	/* floating point divide by zero */
#define PR_FP_EXC_OVF		0x020000	/* floating point overflow */
#define PR_FP_EXC_UND		0x040000	/* floating point underflow */
#define PR_FP_EXC_RES		0x080000	/* floating point inexact result */
#define PR_FP_EXC_INV		0x100000	/* floating point invalid operation */
#define PR_FP_EXC_DISABLED	0	/* FP exceptions disabled */
#define PR_FP_EXC_NONRECOV	1	/* async non-recoverable exc. mode */
#define PR_FP_EXC_ASYNC	2	/* async recoverable exception mode */
#define PR_FP_EXC_PRECISE	3	/* precise exception mode */
#define PR_TIMING_STATISTICAL  0       /* Normal, traditional, statistical process timing */
#define PR_TIMING_TIMESTAMP    1       /* Accurate timestamp based process timing */
#define PR_ENDIAN_BIG		0
#define PR_ENDIAN_LITTLE	1	/* True little endian mode */
#define PR_ENDIAN_PPC_LITTLE	2	/* "PowerPC" pseudo little endian */
#define PR_TSC_ENABLE		1	/* allow the use of the timestamp counter */
#define PR_TSC_SIGSEGV		2	/* throw a SIGSEGV instead of reading the TSC */
#define PR_MCE_KILL_CLEAR   0
#define PR_MCE_KILL_SET     1
#define PR_MCE_KILL_LATE    0
#define PR_MCE_KILL_EARLY   1
#define PR_MCE_KILL_DEFAULT 2
#define PR_SET_MM_START_CODE		1
#define PR_SET_MM_END_CODE		2
#define PR_SET_MM_START_DATA		3
#define PR_SET_MM_END_DATA		4
#define PR_SET_MM_START_STACK		5
#define PR_SET_MM_START_BRK		6
#define PR_SET_MM_BRK			7
#define PR_SET_MM_ARG_START		8
#define PR_SET_MM_ARG_END		9
#define PR_SET_MM_ENV_START		10
#define PR_SET_MM_ENV_END		11
#define PR_SET_MM_AUXV			12
#define PR_SET_MM_EXE_FILE		13
#define PR_SET_MM_MAP			14
#define PR_SET_MM_MAP_SIZE		15
#define PR_SET_PTRACER_ANY ((unsigned long)-1)
#define PR_SPEC_STORE_BYPASS		0
#define PR_SPEC_INDIRECT_BRANCH	1
#define PR_SPEC_NOT_AFFECTED		0
#define PR_SPEC_PRCTL			(1UL << 0)
#define PR_SPEC_ENABLE			(1UL << 1)
#define PR_SPEC_DISABLE		(1UL << 2)
#define PR_SPEC_FORCE_DISABLE		(1UL << 3)
#define PR_SPEC_DISABLE_NOEXEC		(1UL << 4)
#define PR_PAC_APIAKEY			(1UL << 0)
#define PR_PAC_APIBKEY			(1UL << 1)
#define PR_PAC_APDAKEY			(1UL << 2)
#define PR_PAC_APDBKEY			(1UL << 3)
#define PR_PAC_APGAKEY			(1UL << 4)
#define PR_TAGGED_ADDR_ENABLE		(1UL << 0)
#define PR_FP_MODE_FR		(1 << 0)	/* 64b FP registers */
#define PR_FP_MODE_FRE		(1 << 1)	/* 32b compatibility */
#define PR_CAP_AMBIENT_IS_SET		1
#define PR_CAP_AMBIENT_RAISE		2
#define PR_CAP_AMBIENT_LOWER		3
#define PR_CAP_AMBIENT_CLEAR_ALL	4
#define PR_SVE_SET_VL_ONEXEC		(1 << 18) /* defer effect until exec */
#define PR_SVE_VL_LEN_MASK		0xffff
#define PR_SVE_VL_INHERIT		(1 << 17) /* inherit across exec */


struct prctl_mm_map {
	uint64_t start_code;
	uint64_t end_code;
	uint64_t start_data;
	uint64_t end_data;
	uint64_t start_brk;
	uint64_t brk;
	uint64_t start_stack;
	uint64_t arg_start;
	uint64_t arg_end;
	uint64_t env_start;
	uint64_t env_end;
	uint64_t *auxv;
	uint64_t auxv_size;
	uint64_t exe_fd;
};


typedef enum : int {
	PR_SET_PDEATHSIG = 1,
	PR_GET_PDEATHSIG = 2,
	PR_GET_DUMPABLE = 3,
	PR_SET_DUMPABLE = 4,
	PR_GET_UNALIGN = 5,
	PR_SET_UNALIGN = 6,
	PR_GET_KEEPCAPS = 7,
	PR_SET_KEEPCAPS = 8,
	PR_GET_FPEMU = 9,
	PR_SET_FPEMU = 10,
	PR_GET_FPEXC = 11,
	PR_SET_FPEXC = 12,
	PR_GET_TIMING = 13,
	PR_SET_TIMING = 14,
	PR_SET_NAME = 15,
	PR_GET_NAME = 16,
	PR_GET_ENDIAN = 19,
	PR_SET_ENDIAN = 20,
	PR_GET_SECCOMP = 21,
	PR_SET_SECCOMP = 22,
	PR_CAPBSET_READ = 23,
	PR_CAPBSET_DROP = 24,
	PR_GET_TSC = 25,
	PR_SET_TSC = 26,
	PR_GET_SECUREBITS = 27,
	PR_SET_SECUREBITS = 28,
	PR_SET_TIMERSLACK = 29,
	PR_GET_TIMERSLACK = 30,
	PR_TASK_PERF_EVENTS_DISABLE = 31,
	PR_TASK_PERF_EVENTS_ENABLE = 32,
	PR_MCE_KILL = 33,
	PR_MCE_KILL_GET = 34,
	PR_SET_MM = 35,
	PR_SET_CHILD_SUBREAPER = 36,
	PR_GET_CHILD_SUBREAPER = 37,
	PR_SET_NO_NEW_PRIVS = 38,
	PR_GET_NO_NEW_PRIVS = 39,
	PR_GET_TID_ADDRESS = 40,
	PR_SET_THP_DISABLE = 41,
	PR_GET_THP_DISABLE = 42,
	PR_MPX_ENABLE_MANAGEMENT = 43,
	PR_MPX_DISABLE_MANAGEMENT = 44,
	PR_SET_FP_MODE = 45,
	PR_GET_FP_MODE = 46,
	PR_CAP_AMBIENT = 47,
	PR_SVE_SET_VL = 50,
	PR_SVE_GET_VL = 51,
	PR_GET_SPECULATION_CTRL = 52,
	PR_SET_SPECULATION_CTRL = 53,
	PR_PAC_RESET_KEYS = 54,
	PR_SET_TAGGED_ADDR_CTRL = 55,
	PR_GET_TAGGED_ADDR_CTRL = 56,
	PR_SET_PTRACER = 0x59616d61
} prctl_t;


// access
enum : int {
	F_OK = 0,
	X_OK = 1,
	W_OK = 2,
	R_OK = 4
};


// Ressource Limits
typedef enum : int {
	RLIMIT_CPU        =  0,	/* CPU time in sec */
	RLIMIT_FSIZE      =  1,	/* Maximum filesize */
	RLIMIT_DATA       =  2,	/* max data size */
	RLIMIT_STACK      =  3,	/* max stack size */
	RLIMIT_CORE       =  4,	/* max core file size */
	RLIMIT_RSS        =  5,	/* max resident set size */
	RLIMIT_NPROC      =  6,	/* max number of processes */
	RLIMIT_NOFILE     =  7,	/* max number of open files */
	RLIMIT_MEMLOCK    =  8,	/* max locked-in-memory address space */
	RLIMIT_AS         =  9,	/* address space limit */
	RLIMIT_LOCKS      = 10,	/* maximum file locks held */
	RLIMIT_SIGPENDING = 11,	/* max number of pending signals */
	RLIMIT_MSGQUEUE   = 12,	/* maximum bytes in POSIX mqueues */
	RLIMIT_NICE       = 13,	/* max nice prio allowed to raise to 0-39 for nice level 19 .. -20 */
	RLIMIT_RTPRIO     = 14,	/* maximum realtime priority */
	RLIMIT_RTTIME     = 15	/* timeout for RT tasks in us */
} rlimit_t;


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
#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100
#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010
#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001

#define S_IFMT   00170000
#define S_IFSOCK  0140000
#define S_IFLNK   0120000
#define S_IFREG   0100000
#define S_IFBLK   0060000
#define S_IFDIR   0040000
#define S_IFCHR   0020000
#define S_IFIFO   0010000
#define S_ISUID   0004000
#define S_ISGID   0002000
#define S_ISVTX   0001000

#define S_ISLNK(m)  (((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m)  (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)  (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)  (((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)

struct stat {
	unsigned long   st_dev;
	unsigned long   st_ino;
	unsigned long   st_nlink;
	unsigned        st_mode;
	unsigned        st_uid;
	unsigned        st_gid;
	unsigned        __pad0;
	unsigned long   st_rdev;
	off_t           st_size;
	long            st_blksize;
	long            st_blocks;
	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
	long            __unused[3];
};


// MemFD
#define MFD_CLOEXEC       0x0001U
#define MFD_ALLOW_SEALING 0x0002U
#define MFD_HUGETLB       0x0004U

// Memory Mapping (m*)
#define PROT_NONE       0x0
#define PROT_READ       0x1
#define PROT_WRITE      0x2
#define PROT_EXEC       0x4

#define PROT_GROWSDOWN  0x01000000
#define PROT_GROWSUP    0x02000000


#define MAP_FAILED          ((void*)-1)

#define MAP_SHARED          0x01
#define MAP_PRIVATE         0x02
#define MAP_FIXED           0x10
#define MAP_ANONYMOUS       0x20

#define MAP_POPULATE        0x008000	/* populate (prefault) pagetables */
#define MAP_NONBLOCK        0x010000	/* do not block on IO */
#define MAP_STACK           0x020000	/* give out an address that is best suited for process/thread stacks */
#define MAP_HUGETLB         0x040000	/* create a huge page mapping */
#define MAP_SYNC            0x080000	/* perform synchronous page faults for the mapping */
#define MAP_FIXED_NOREPLACE 0x100000	/* MAP_FIXED which doesn't unmap underlying mapping */
#define MAP_UNINITIALIZED   0x4000000	/* For anonymous mmap, memory could be uninitialized */


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

#define SA_NOCLDSTOP 0x00000001u
#define SA_NOCLDWAIT 0x00000002u
#define SA_SIGINFO   0x00000004u
#define SA_ONSTACK   0x08000000u
#define SA_RESTART   0x10000000u
#define SA_NODEFER   0x40000000u
#define SA_RESETHAND 0x80000000u
#define SA_NOMASK    SA_NODEFER
#define SA_ONESHOT   SA_RESETHAND
#define SA_RESTORER  0x04000000

struct sigaction {
	void (*sa_handler)(int);
	unsigned long sa_flags;
	void (*sa_restorer)(void);
	unsigned long sa_mask[1024 / (8 * sizeof(unsigned long int))];
};

#define SIG_DFL	((void (*)(int))0)	/* default signal handling */
#define SIG_IGN	((void (*)(int))1)	/* ignore signal */
#define SIG_ERR	((void (*)(int))-1)	/* error return from signal */


// inotify
#define IN_CLOEXEC       O_CLOEXEC
#define IN_NONBLOCK      O_NONBLOCK
#define IN_ACCESS        0x00000001	/* File was accessed.  */
#define IN_MODIFY        0x00000002	/* File was modified.  */
#define IN_ATTRIB        0x00000004	/* Metadata changed.  */
#define IN_CLOSE_WRITE   0x00000008	/* Writtable file was closed.  */
#define IN_CLOSE_NOWRITE 0x00000010	/* Unwrittable file closed.  */
#define IN_CLOSE         (IN_CLOSE_WRITE | IN_CLOSE_NOWRITE) /* Close.  */
#define IN_OPEN          0x00000020	/* File was opened.  */
#define IN_MOVED_FROM    0x00000040	/* File was moved from X.  */
#define IN_MOVED_TO      0x00000080	/* File was moved to Y.  */
#define IN_MOVE          (IN_MOVED_FROM | IN_MOVED_TO) /* Moves.  */
#define IN_CREATE        0x00000100	/* Subfile was created.  */
#define IN_DELETE        0x00000200	/* Subfile was deleted.  */
#define IN_DELETE_SELF   0x00000400	/* Self was deleted.  */
#define IN_MOVE_SELF     0x00000800	/* Self was moved.  */
#define IN_UNMOUNT       0x00002000	/* Backing fs was unmounted.  */
#define IN_Q_OVERFLOW    0x00004000	/* Event queued overflowed.  */
#define IN_IGNORED       0x00008000	/* File was ignored.  */
#define IN_CLOSE         (IN_CLOSE_WRITE | IN_CLOSE_NOWRITE)	/* Close.  */
#define IN_MOVE          (IN_MOVED_FROM | IN_MOVED_TO)		/* Moves.  */
#define IN_ONLYDIR       0x01000000	/* Only watch the path if it is a directory.  */
#define IN_DONT_FOLLOW   0x02000000	/* Do not follow a sym link.  */
#define IN_EXCL_UNLINK   0x04000000	/* Exclude events on unlinked objects.  */
#define IN_MASK_CREATE   0x10000000	/* Only create watches.  */
#define IN_MASK_ADD      0x20000000	/* Add to the mask of an already existing watch.  */
#define IN_ISDIR         0x40000000	/* Event occurred against dir.  */
#define IN_ONESHOT       0x80000000	/* Only send event once.  */

/* Structure describing an inotify event.  */
struct inotify_event {
	int wd;
	uint32_t mask;
	uint32_t cookie;
	uint32_t len;
	char name[];
};

// Exit
#define	EXIT_FAILURE  1
#define	EXIT_SUCCESS  0


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
extern "C" [[noreturn]] void abort();
extern "C" [[noreturn]] void crash();
extern "C" [[noreturn]] void exit(int code);

extern "C" void *mmap(void *start, size_t len, int prot, int flags, int fd, long off);
extern "C" int mprotect(void *addr, size_t len, int prot);
extern "C" int munmap(void *start, size_t len);
extern "C" int msync(void *start, size_t len, int flags);

extern "C" int access(const char *filename, int amode);
extern "C" int open(const char *filename, int flags, int mode = 0);
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
