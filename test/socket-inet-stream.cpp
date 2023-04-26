// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include "socket.hpp"

int main() {
	sockaddr_in server_addr{"127.0.0.1", 9042};

	if (auto pid = Syscall::fork()) {
		if (auto fd = Syscall::socket(PF_INET, SOCK_STREAM, 0)) {
			const int enable = 1;
			Syscall::setsockopt(fd.value(), SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
			if (pid == 0) {
				stream_server(fd.value(), server_addr);
			} else {
				Syscall::sleep(1);
				client(fd.value(), server_addr);
			}
			Syscall::close(fd);
		} else {
			cerr << "Creating socket failed: " << fd.error_message() << endl;
		}
	} else {
		cerr << "Fork failed: " << pid.error_message() << endl;
	}

	return 0;
}
