// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include <dlh/socket.hpp>

namespace Socket {

// inspired by http://www.cs.cmu.edu/afs/cs/academic/class/15213-f00/unpv12e/libfree/inet_aton.c
bool inet_aton(const char *s, struct in_addr *dest) {
	int dots = 0;
	unsigned long addr = 0;
	unsigned long cur = 0;
	do {
		switch (*s) {
			case '0' ... '9':
				cur = cur * 10 + ((*s) - '0');
				break;

			case '.':
				if (++dots > 3)
					return false;
				[[fallthrough]];

			case '\0':
				if (cur > 255)
					return false;

				addr = (addr << 8) | cur;
				cur = 0;
				break;

			default:
				return false;
		}
	} while ((*s++) != 0);

	if (dots < 3)
		addr <<= 8 * (3 - dots);

	if (dest != 0)
		dest->s_addr = htonl(addr);

	return true;
}

}  // namespace Socket
