// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include <dlh/stream/output.hpp>
#include <dlh/syscall.hpp>

int main(int argc, const char ** argv) {
	if (argc == 1) {
		const char * args[4] = { argv[0], "Hello", "world!", NULL };
		auto exec = Syscall::execv(argv[0], args);
		cout << "Exec failed: " << exec.error_message() << endl;
	} else {
		cout << (argc - 1) << " arguments:";
		for (int i = 1; i < argc; i++)
			cout << ' ' << argv[i];
		cout << endl;
	}
	return 0;
}
