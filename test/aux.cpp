// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include <dlh/stream/output.hpp>
#include <dlh/auxiliary.hpp>
#include <dlh/systypes.hpp>

int main(int argc, const char *argv[]) {
	(void) argc;
	(void) argv;

	cout << "Environment variables:" << endl;
	static int envc = -1;
	if (envc == -1) {
		for (envc = 0; environ[envc] != nullptr; envc++)
			cout << environ[envc] << endl;
	}

	cout << "Auciliary vectors:" << endl;
	Auxiliary * auxv = reinterpret_cast<Auxiliary *>(environ + envc + 1);
	for (int auxc = 0 ; auxv[auxc].a_type != Auxiliary::AT_NULL; auxc++)
		cout << "AUX[" << auxv[auxc].a_type << "] = "<< auxv[auxc].a_un.a_ptr << endl;

	cout << reinterpret_cast<char*>(0x400000) << endl;
	return 0;
}
