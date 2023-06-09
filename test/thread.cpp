// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

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
	cout << "foo start" << endl;
	cout << "int foo(" << a << ")" << endl;

	struct sigaction action;
	Memory::set(&action, 0, sizeof(struct sigaction));
	action.sa_handler = term;
	Syscall::sigaction(SIGCHLD, &action, nullptr).die_on_error("sigaction");

	auto r = Syscall::prctl(PR_SET_PDEATHSIG, SIGCHLD).value_or_die("set signal");
	cout << "set signal " << r << endl;

	Syscall::sleep(8);
	cout << "foo done" << endl;
	return reinterpret_cast<void*>(42);
}

int main(int argc, const char *argv[]) {
	(void) argc;
	(void) argv;

	cerr << "main start" << endl;
	Thread * f = Thread::create(foo, reinterpret_cast<void*>(0x23), true, true);
	if (f == nullptr) {
		cerr << "failed" << endl;
		return 1;
	}
	Syscall::sleep(4);
	cerr << "main exit" << endl;
	return 0;
}
