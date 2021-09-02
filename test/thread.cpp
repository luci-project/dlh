#include <dlh/stream/output.hpp>
#include <dlh/utils/thread.hpp>
#include <dlh/unistd.hpp>
#include <dlh/errno.hpp>
#include <dlh/string.hpp>

void term(int signum) {
	write(2, "thread quit\n", 13);
	exit(0);
}

int foo(void * a) {
cout << "start " << __errno_location() << endl;
	cout << "int foo(" << a << ")" << endl;
	const char * name = "observer";
	errno = 0;

	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = term;
	auto sr = sigaction(SIGCHLD, &action, NULL);
	if (sr == -1)
		perror("sigaction");

cout << "start" << endl;

	auto r = prctl(PR_SET_PDEATHSIG, SIGCHLD);
	if (r == -1)
		perror("Set signal");
	cout << "Set signal " << r << endl;


	sleep(8);
	cout << "foo done" << endl;
	return 42;
}

int main(int argc, const char *argv[]) {
	cerr << "start " << __errno_location() << endl;
	Thread * f = Thread::create(foo, (void*)0x23, true);
	if (f == nullptr) {
		cerr << "failed" << endl;
		return 1;
	}
	sleep(4);
	cerr << "main exit" << endl;
	return 23;
}
