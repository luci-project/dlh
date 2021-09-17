#pragma once

#include <dlh/utility.hpp>
#include <dlh/systypes.hpp>

struct Thread {
	 /* Pointer to the Thread Control Block */
	Thread * tcb = nullptr;
	/* Dynamic Thread Vector for Thread Local Storage */
	union DynamicThreadVector {
		size_t counter;
		struct dtv_pointer {
			void *val;
			void *to_free;
		} pointer;

		constexpr DynamicThreadVector(void * val = nullptr) : pointer{val, 0} {}

		inline bool allocated() const {
			return pointer.val != nullptr;
		}
	} * dtv = nullptr;
	/* Pointer to the thread descriptor.  */
	Thread * selfptr  = nullptr;
	int multiple_threads = 0;
	int gscope_flag = 0;
	uintptr_t sysinfo = 0;
	uintptr_t stack_guard = 0;
	uintptr_t pointer_guard = 0;
	unsigned long int vgetcpu_cache[2] = { 0, 0};
	unsigned int feature_1 = 0;
	int __glibc_unused1 = 0;
	/* Reservation of some values for the TM ABI.  */
	void *__private_tm[4] = { nullptr };
	/* GCC split stack support.  */
	void *__private_ss  = { nullptr };
	/* The lowest address of shadow stack,  */
	unsigned long long int ssp_base = 0;
	/* Must be kept even if it is no longer used by glibc since programs,
	  like AddressSanitizer, depend on the size of tcbhead_t.  */
	void *__glibc_unused2[64] __attribute__ ((aligned (32))) = { nullptr };

	union {
		struct {
			// Custom fields (in reserved space)
			int tls_errno;
			uintptr_t map_base;
			size_t map_size;
		};
		void * __padding[8] = { nullptr };
	};

	/* This descriptor's link on the `stack_used' or `__stack_user' list.  */
	struct ListHead {
		ListHead *next = nullptr;
		ListHead *prev = nullptr;
	} list;
	/* Thread ID - which is also a 'is this thread descriptor (and therefore stack) used' flag.  */
	pid_t tid = -1;
	/* Ununsed.  */
	pid_t pid_ununsed = 0;

	/* List of robust mutexes the thread is holding.  */
	void *robust_prev = nullptr;
	struct {
		void *list = nullptr;
		long int futex_offset = 0;
		void *list_op_pending = nullptr;
	} robust_head;
	/* List of cleanup buffers.  */
	void *cleanup = nullptr;
	/* Unwind information.  */
	void *cleanup_jmp_buf = nullptr;
	/* Flags determining processing of cancellation.  */
	int cancelhandling = 0;
	/* Flags.  Including those copied from the thread attribute.  */
	int flags = 0;
	/* We allocate one block of references here.  This should be enough
	 to avoid allocating any memory dynamically for most applications.  */
	struct {
		/* Sequence number.  We use uintptr_t to not require padding on
		   32- and 64-bit machines.  On 64-bit machines it helps to avoid
		   wrapping, too.  */
		uintptr_t seq = 0;
		/* Data pointer.  */
		void *data = nullptr;
	} specific_1stblock[32];
	/* Two-level array for the thread-specific data.  */
	void * specific[32] = { nullptr };
	/* Flag which is set when specific data is set.  */
	bool specific_used = false;
	/* True if events must be reported.  */
	bool report_events = false;
	/* True if the user provided the stack.  */
	bool user_stack = false;
	/* True if thread must stop at startup time.  */
	bool stopped_start = false;
	/* The parent's cancel handling at the time of the pthread_create
	 call.  This might be needed to undo the effects of a cancellation.  */
	int parent_cancelhandling = 0;
	/* Lock to synchronize access to the descriptor.  */
	int lock = 0;
	/* Lock for synchronizing setxid calls.  */
	unsigned int setxid_futex = 0;
	unsigned long long cpuclock_offset_ununsed = 0;
	/* If the thread waits to join another one the ID of the latter is
	   stored here.
	In case a thread is detached this field contains a pointer of the
	TCB if the thread itself.  This is something which cannot happen
	in normal operation.  */
	Thread * joindid = nullptr;
	/* The result of the thread function.  */
	void *result = nullptr;
	/* Scheduling parameters for the new thread.  */
	struct {
		int sched_priority  = 0;
	} schedparam;
	int schedpolicy = 0;
	/* Start position of the code to be executed and the argument passed to the function.  */
	void *(*start_routine) (void *) = nullptr;
	void *arg = nullptr;

	/* Debug state.  */
	struct {
		uint64_t eventmask = 0;          /* Mask of enabled events.  */
		int eventnum = 0;                /* Number of last event.  */
		void *eventdata = nullptr;       /* Data associated with event.  */
	} eventbuf;
	/* Next descriptor with a pending event.  */
	Thread *nextevent = nullptr;
	/* Machine-specific unwind info.  */
	struct {
		unsigned long exception_class = 0;
		void *exception_cleanup = nullptr;
		unsigned long  private_1 = 0;
		unsigned long  private_2 = 0;
	} exc;
	/* If nonzero, pointer to the area allocated for the stack and guard. */
	void *stackblock = nullptr;
	/* Size of the stackblock area including the guard.  */
	size_t stackblock_size = 0;
	/* Size of the included guard area.  */
	size_t guardsize = 0;
	/* This is what the user specified and what we will report.  */
	size_t reported_guardsize = 0;
	/* Thread Priority Protection data.  */
	void *tpp = nullptr;
	/* Resolver state.  */
	void * res[71] = { nullptr };
	/* Indicates whether is a C11 thread created by thrd_creat.  */
	bool c11 = false;
	/* This member must be last.  */
	char end_padding[0] = { };

	constexpr Thread(DynamicThreadVector * dtv = nullptr, uintptr_t base = 0, size_t size = 0, bool detach = false) : tcb(this), dtv(dtv), selfptr(this), map_base(base), map_size(size), joindid(detach ? this : 0) {}

	static Thread * create(void* (*func)(void*), void * arg = nullptr, bool detach = false, size_t stack_size = 1048576, size_t tls_size = 0xd40, DynamicThreadVector * dtv = nullptr, bool hidden = false);

	pid_t id() const {
		return tid;
	}

	bool detached() const {
		return joindid == this;
	}

	void detach();

	void exit(void * result = 0);

	int kill(signal_t sig);

	bool join(void** result = nullptr);

	void setup_guards(void* random);

	inline bool active() const {
		return tid > 0;
	}

	static inline Thread * self() {
		struct Thread *s;
		asm volatile("mov %%fs:%c1,%0"
		             : "=r" (s)
		             : "i" (__builtin_offsetof(Thread, selfptr))
		);
		return s;
	}

	static inline int & local_errno() {
		return self()->tls_errno;
	}
} __attribute__((aligned(64)));

static_assert(sizeof(Thread) == 0x900, "Wrong [P]Thread size");
