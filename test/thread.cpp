#include <dlh/stream/output.hpp>
#include <dlh/syscall.hpp>
#include <dlh/string.hpp>
#include <dlh/thread.hpp>
#include <dlh/error.hpp>

void term(int signum) {
	(void) signum;
	Syscall::write(2, "thread quit\n", 13);
	Syscall::exit(0);
}

void* foo(void * a) {
	cout << "start" << endl;
	cout << "int foo(" << a << ")" << endl;

	struct sigaction action;
	Memory::set(&action, 0, sizeof(struct sigaction));
	action.sa_handler = term;
	Syscall::sigaction(SIGCHLD, &action, NULL).die_on_error("sigaction");

	cout << "start" << endl;
	auto r = Syscall::prctl(PR_SET_PDEATHSIG, SIGCHLD).value_or_die("set signal");
	cout << "Set signal " << r << endl;


	Syscall::sleep(8);
	cout << "foo done" << endl;
	return (void*)42;
}

int main(int argc, const char *argv[]) {
	(void) argc;
	(void) argv;

	cerr << "start" << endl;
	Thread * f = Thread::create(foo, (void*)0x23, true);
	if (f == nullptr) {
		cerr << "failed" << endl;
		return 1;
	}
	Syscall::sleep(4);
	cerr << "main exit" << endl;
	return 23;
}
