// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include <dlh/environ.hpp>

extern char **environ;

namespace Environ {

char * variable(const char * name, bool consume) {
	for (char ** ep = environ; *ep != nullptr; ep++) {
		char * e = *ep;
		const char * n = name;
		while (*n == *e && *e != '\0') {
			n++;
			e++;
		}
		if (*e == '=' && *n == '\0') {
			if (consume)
				**ep = '\0';
			return e + 1;
		}
	}
	return nullptr;
}

}  // namespace Environ
