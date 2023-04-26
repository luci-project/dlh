// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include "socket.hpp"

int main() {
	sockaddr_un server_addr{"socket-dgram.server"};
	Syscall::unlink(server_addr.sun_path);

	if (auto pid = Syscall::fork()) {
		if (auto fd = Syscall::socket(PF_UNIX, SOCK_DGRAM, 0)) {
			if (pid == 0) {
				datagram_server(fd.value(), server_addr);
			} else {
				Syscall::sleep(1);
				sockaddr_un client_addr{"socket-dgram.client"};
				Syscall::unlink(client_addr.sun_path);
				if (auto bind = Syscall::bind(fd.value(), reinterpret_cast<struct sockaddr *>(&client_addr), sizeof(client_addr))) {
					client(fd.value(), server_addr);
				} else {
					cerr << "Client bind socket failed: " << bind.error_message() << endl;
				}
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
