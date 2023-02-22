#pragma once

// TODO: Auto generate from system headers

#include <dlh/types.hpp>

extern char **environ;

#define NAME_MAX 4096
#define PATH_MAX 4096

// Time
typedef long int time_t;
struct timespec {
	time_t      tv_sec;   // Number of whole seconds of elapsed time
	long int    tv_nsec;  // Number of nanosecods of rest of elapsed time minus tv_sec. Always less than one billion.

	unsigned long nanotimestamp() const {
		return 1'000'000'000UL * tv_sec + tv_nsec;
	}

	unsigned long microtimestamp() const {
		return 1'000'000UL * tv_sec + (tv_nsec + 500) / 1'000;
	}

	unsigned long millitimestamp() const {
		return 1'000UL * tv_sec + (tv_nsec + 500'000) / 1'000'000;
	}

	unsigned long timestamp() const {
		return 1UL * tv_sec;
	}
};

typedef enum : int {
	CLOCK_REALTIME           = 0,
	CLOCK_MONOTONIC          = 1,
	CLOCK_PROCESS_CPUTIME_ID = 2,
	CLOCK_THREAD_CPUTIME_ID  = 3,
	CLOCK_MONOTONIC_RAW      = 4,
	CLOCK_REALTIME_COARSE    = 5,
	CLOCK_MONOTONIC_COARSE   = 6,
	CLOCK_BOOTTIME           = 7,
	CLOCK_REALTIME_ALARM     = 8,
	CLOCK_BOOTTIME_ALARM     = 9
} clockid_t;

// getcpu
struct getcpu_cache {
	unsigned long blob[128 / sizeof(long)];
};

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

// futex
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

// Macros for wait
#define WEXITSTATUS(status) (((status) & 0xff00) >> 8)
#define WTERMSIG(status)    ((status) & 0x7f)
#define WSTOPSIG(status)    WEXITSTATUS(status)
#define WIFEXITED(status)   (WTERMSIG(status) == 0)
#define WIFSIGNALED(status) (((signed char) (((status) & 0x7f) + 1) >> 1) > 0)
#define	WIFSTOPPED(status)  (((status) & 0xff) == 0x7f)

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

#define MREMAP_MAYMOVE  1
#define MREMAP_FIXED    2

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

// Signal stack
#define MINSIGSTKSZ	2048
#define SIGSTKSZ	8192

enum {
	SS_ONSTACK = 1,
	SS_DISABLE
};


struct sigstack {
	void* ss_sp;     /* Base address of stack */
	int ss_flags;    /* Flags */
	size_t ss_size;  /* Number of bytes in stack */
};

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

struct siginfo {
	int si_signo;
	int si_errno;
	int si_code;
	int __pad0;
	int _sifields[28];  /* incomplete (for now) */
};

struct ucontext;
struct sigaction {
	union {
		void (*sa_handler)(int) = nullptr;
		void (*sa_sigaction)(int, siginfo *, ucontext *);
	};
	unsigned long sa_flags = 0;
	void (*sa_restorer)(void) = nullptr;
	unsigned long sa_mask[1024 / (8 * sizeof(unsigned long int))] = {};
};

#define SIG_DFL	((void (*)(int))0)	/* default signal handling */
#define SIG_IGN	((void (*)(int))1)	/* ignore signal */
#define SIG_ERR	((void (*)(int))-1)	/* error return from signal */

// Context
enum REGS {
	REG_R8      =  0,
	REG_R9      =  1,
	REG_R10     =  2,
	REG_R11     =  3,
	REG_R12     =  4,
	REG_R13     =  5,
	REG_R14     =  6,
	REG_R15     =  7,
	REG_RDI     =  8,
	REG_RSI     =  9,
	REG_RBP     = 10,
	REG_RBX     = 11,
	REG_RDX     = 12,
	REG_RAX     = 13,
	REG_RCX     = 14,
	REG_RSP     = 15,
	REG_RIP     = 16,
	REG_EFL     = 17,
	REG_CSGSFS  = 18,  /* Actually short cs, gs, fs, __pad0.  */
	REG_ERR     = 19,
	REG_TRAPNO  = 20,
	REG_OLDMASK = 21,
	REG_CR2     = 22,
	REG_COUNT   = 23
};

struct fpstate {  /* 64-bit FXSAVE format.  */
	uint16_t cwd;
	uint16_t swd;
	uint16_t ftw;
	uint16_t fop;
	uint64_t rip;
	uint64_t rdp;
	uint32_t mxcsr;
	uint32_t mxcr_mask;
	struct {
		unsigned short significand[4];
		unsigned short exponent;
		unsigned short __glibc_reserved1[3];
	} st[8];
	struct {
		uint32_t element[4];
	} xmm[16];
	uint32_t __glibc_reserved1[24];
};

struct mcontext {
	uintptr_t gregs[REG_COUNT];
	fpstate * fpregs;
	uintptr_t __reserved1[8];
};

struct ucontext {
	unsigned long int uc_flags;
	ucontext *uc_link;
	sigstack uc_stack;
	mcontext uc_mcontext;
	unsigned long uc_sigmask;
	fpstate __fpregs_mem;
	uintptr_t __ssp[4];
};


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

// Poll
struct pollfd {
	int fd;         /* File descriptor to poll */
	short events;   /* Types of events poller cares about */
	short revents;  /* Types of events that actually occurred */
};
#define POLLIN      0x0001  /* There is data to read */
#define POLLPRI     0x0002  /* There is urgent data to read */
#define POLLOUT     0x0004  /* Writing now will not block */
#define POLLERR     0x0008  /* Error condition */
#define POLLHUP     0x0010  /* Hung up */
#define POLLNVAL    0x0020  /* Invalid polling request */
#define POLLRDNORM  0x0040  /* Normal data may be read */
#define POLLRDBAND  0x0080  /* Priority data may be read */
#define POLLWRNORM  0x0100  /* Writing now will not block */
#define POLLWRBAND  0x0200  /* Priority data may be written */
#define POLLREMOVE  0x1000
#define POLLRDHUP   0x2000


// Ptrace
typedef enum {
	PTRACE_TRACEME = 0,
	PTRACE_PEEKTEXT = 1,
	PTRACE_PEEKDATA = 2,
	PTRACE_PEEKUSER = 3,
	PTRACE_POKETEXT = 4,
	PTRACE_POKEDATA = 5,
	PTRACE_POKEUSER = 6,
	PTRACE_CONT = 7,
	PTRACE_KILL = 8,
	PTRACE_SINGLESTEP = 9,
	PTRACE_GETREGS = 12,
	PTRACE_SETREGS = 13,
	PTRACE_GETFPREGS = 14,
	PTRACE_SETFPREGS = 15,
	PTRACE_ATTACH = 16,
	PTRACE_DETACH = 17,
	PTRACE_GETFPXREGS = 18,
	PTRACE_SETFPXREGS = 19,
	PTRACE_SYSCALL = 24,
	PTRACE_GET_THREAD_AREA = 25,
	PTRACE_SET_THREAD_AREA = 26,
	PTRACE_ARCH_PRCTL = 30,
	PTRACE_SYSEMU = 31,
	PTRACE_SYSEMU_SINGLESTEP = 32,
	PTRACE_SINGLEBLOCK = 33,
	PTRACE_SETOPTIONS = 0x4200,
	PTRACE_GETEVENTMSG = 0x4201,
	PTRACE_GETSIGINFO = 0x4202,
	PTRACE_SETSIGINFO = 0x4203,
	PTRACE_GETREGSET = 0x4204,
	PTRACE_SETREGSET = 0x4205,
	PTRACE_SEIZE = 0x4206,
	PTRACE_INTERRUPT = 0x4207,
	PTRACE_LISTEN = 0x4208,
	PTRACE_PEEKSIGINFO = 0x4209,
	PTRACE_GETSIGMASK = 0x420a,
	PTRACE_SETSIGMASK = 0x420b,
	PTRACE_SECCOMP_GET_FILTER = 0x420c,
	PTRACE_SECCOMP_GET_METADATA = 0x420d,
	PTRACE_GET_SYSCALL_INFO = 0x420e
} ptrace_request_t;

typedef enum {
	PTRACE_O_TRACESYSGOOD    = 0x00000001,
	PTRACE_O_TRACEFORK       = 0x00000002,
	PTRACE_O_TRACEVFORK      = 0x00000004,
	PTRACE_O_TRACECLONE      = 0x00000008,
	PTRACE_O_TRACEEXEC       = 0x00000010,
	PTRACE_O_TRACEVFORKDONE  = 0x00000020,
	PTRACE_O_TRACEEXIT       = 0x00000040,
	PTRACE_O_TRACESECCOMP    = 0x00000080,
	PTRACE_O_EXITKILL        = 0x00100000,
	PTRACE_O_SUSPEND_SECCOMP = 0x00200000,
	PTRACE_O_MASK            = 0x003000ff
} ptrace_setoptions_t;

typedef enum {
	PTRACE_EVENT_FORK = 1,
	PTRACE_EVENT_VFORK = 2,
	PTRACE_EVENT_CLONE = 3,
	PTRACE_EVENT_EXEC = 4,
	PTRACE_EVENT_VFORK_DONE = 5,
	PTRACE_EVENT_EXIT = 6,
	PTRACE_EVENT_SECCOMP = 7,
	PTRACE_EVENT_STOP = 128
} ptrace_eventcodes_t;

typedef enum {
	PTRACE_SYSCALL_INFO_NONE = 0,
	PTRACE_SYSCALL_INFO_ENTRY = 1,
	PTRACE_SYSCALL_INFO_EXIT = 2,
	PTRACE_SYSCALL_INFO_SECCOMP = 3
} ptrace_get_syscall_info_op_t;


struct user_regs_struct {
	__extension__ unsigned long long int r15;
	__extension__ unsigned long long int r14;
	__extension__ unsigned long long int r13;
	__extension__ unsigned long long int r12;
	__extension__ unsigned long long int rbp;
	__extension__ unsigned long long int rbx;
	__extension__ unsigned long long int r11;
	__extension__ unsigned long long int r10;
	__extension__ unsigned long long int r9;
	__extension__ unsigned long long int r8;
	__extension__ unsigned long long int rax;
	__extension__ unsigned long long int rcx;
	__extension__ unsigned long long int rdx;
	__extension__ unsigned long long int rsi;
	__extension__ unsigned long long int rdi;
	__extension__ unsigned long long int orig_rax;
	__extension__ unsigned long long int rip;
	__extension__ unsigned long long int cs;
	__extension__ unsigned long long int eflags;
	__extension__ unsigned long long int rsp;
	__extension__ unsigned long long int ss;
	__extension__ unsigned long long int fs_base;
	__extension__ unsigned long long int gs_base;
	__extension__ unsigned long long int ds;
	__extension__ unsigned long long int es;
	__extension__ unsigned long long int fs;
	__extension__ unsigned long long int gs;
};


/* Userfault FD */
#define UFFD_API 0xAAUL
#define UFFD_API_FEATURES (UFFD_FEATURE_EVENT_FORK | UFFD_FEATURE_EVENT_REMAP | UFFD_FEATURE_EVENT_REMOVE | UFFD_FEATURE_EVENT_UNMAP | UFFD_FEATURE_MISSING_HUGETLBFS |	UFFD_FEATURE_MISSING_SHMEM | UFFD_FEATURE_SIGBUS | UFFD_FEATURE_THREAD_ID)
#define UFFD_API_IOCTLS (1UL << _UFFDIO_REGISTER | 1UL << _UFFDIO_UNREGISTER | 1UL << _UFFDIO_API)
#define UFFD_API_RANGE_IOCTLS (1UL << _UFFDIO_WAKE | 1UL << _UFFDIO_COPY | 1UL << _UFFDIO_ZEROPAGE)
#define UFFD_API_RANGE_IOCTLS_BASIC (1UL << _UFFDIO_WAKE | 1UL << _UFFDIO_COPY)

#define _UFFDIO_REGISTER    (0x00)
#define _UFFDIO_UNREGISTER  (0x01)
#define _UFFDIO_WAKE        (0x02)
#define _UFFDIO_COPY        (0x03)
#define _UFFDIO_ZEROPAGE    (0x04)
#define _UFFDIO_API         (0x3F)

/* userfaultfd ioctl ids */
typedef enum : unsigned long {
	UFFDIO            = 0xAA,
	UFFDIO_API        = 0xC018AA3F,
	UFFDIO_REGISTER   = 0xC020AA00,
	UFFDIO_UNREGISTER = 0x8010AA01,
	UFFDIO_WAKE       = 0x8010AA02,
	UFFDIO_COPY       = 0xC028AA03,
	UFFDIO_ZEROPAGE   = 0xC020AA04
} uffdio_t;

/* read() structure */
struct uffd_msg {
	enum Events : uint8_t {
		UFFD_EVENT_PAGEFAULT = 0x12,
		UFFD_EVENT_FORK      = 0x13,
		UFFD_EVENT_REMAP     = 0x14,
		UFFD_EVENT_REMOVE    = 0x15,
		UFFD_EVENT_UNMAP     = 0x16
	} event;

	uint8_t reserved1;
	uint16_t reserved2;
	uint32_t reserved3;

	union {
		struct {
			uint64_t flags;
			uint64_t address;
			union {
				uint32_t ptid;
			} feat;
		} pagefault;

		struct {
			uint32_t ufd;
		} fork;

		struct {
			uint64_t from;
			uint64_t to;
			uint64_t len;
		} remap;

		struct {
			uint64_t start;
			uint64_t end;
		} remove;

		struct {
			/* unused reserved fields */
			uint64_t reserved1;
			uint64_t reserved2;
			uint64_t reserved3;
		} reserved;
	} arg;
} __attribute__((packed));

/* flags for UFFD_EVENT_PAGEFAULT */
#define UFFD_PAGEFAULT_FLAG_WRITE  (1<<0)	/* If this was a write fault */
#define UFFD_PAGEFAULT_FLAG_WP     (1<<1)	/* If reason is VM_UFFD_WP */

#define UFFD_FEATURE_PAGEFAULT_FLAG_WP  (1<<0)
#define UFFD_FEATURE_EVENT_FORK	        (1<<1)
#define UFFD_FEATURE_EVENT_REMAP        (1<<2)
#define UFFD_FEATURE_EVENT_REMOVE       (1<<3)
#define UFFD_FEATURE_MISSING_HUGETLBFS  (1<<4)
#define UFFD_FEATURE_MISSING_SHMEM      (1<<5)
#define UFFD_FEATURE_EVENT_UNMAP        (1<<6)
#define UFFD_FEATURE_SIGBUS             (1<<7)
#define UFFD_FEATURE_THREAD_ID          (1<<8)

#define UFFDIO_REGISTER_MODE_MISSING   (1UL<<0)
#define UFFDIO_REGISTER_MODE_WP        (1UL<<1)
#define UFFDIO_COPY_MODE_DONTWAKE      (1UL<<0)
#define UFFDIO_ZEROPAGE_MODE_DONTWAKE  (1UL<<0)

struct uffdio_api {
	uint64_t api;
	uint64_t features;
	uint64_t ioctls;

	uffdio_api(uint64_t features = 0, uint64_t api = UFFD_API, uint64_t ioctls = 0)
	 : api(api), features(features), ioctls(ioctls) {}
};

struct uffdio_range {
	uint64_t start;
	uint64_t len;

	uffdio_range(uint64_t start = 0, uint64_t len = 0)
	 : start(start), len(len) {}
};

struct uffdio_register {
	struct uffdio_range range;
	uint64_t mode;
	uint64_t ioctls;

	uffdio_register(uint64_t start = 0, uint64_t len = 0, uint64_t mode = 0, uint64_t ioctls = 0)
	 : range(start, len), mode(mode), ioctls(ioctls) {}
};

struct uffdio_copy {
	uint64_t dst;
	uint64_t src;
	uint64_t len;
	uint64_t mode;
	int64_t copy;

	uffdio_copy(uint64_t dst = 0, uint64_t src = 0, uint64_t len = 0, uint64_t mode = 0, int64_t copy = 0)
	 : dst(dst), src(src), len(len), mode(mode), copy(copy) {}
};

struct uffdio_zeropage {
	struct uffdio_range range;
	uint64_t mode;
	int64_t zeropage;

	uffdio_zeropage(uint64_t start = 0, uint64_t len = 0, uint64_t mode = 0, uint64_t zeropage = 0)
	 : range(start, len), mode(mode), zeropage(zeropage) {}
};
