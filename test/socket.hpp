#include <dlh/stream/output.hpp>
#include <dlh/syscall.hpp>

static void rot13(char * buf, ssize_t len) {
	for (ssize_t i = 0; i < len ; i++) {
		if ((buf[i] >= 'a' && buf[i] <= 'm') || (buf[i] >= 'A' && buf[i] <= 'M')) {
			buf[i] += 13;
		} else if ((buf[i] >= 'n' && buf[i] <= 'z') || (buf[i] >= 'N' && buf[i] <= 'Z')) {
			buf[i] -= 13;
		}
	}
}

template<typename SOCKADDR>
static void datagram_server(int socket, SOCKADDR server_addr) {
	if (auto bind = Syscall::bind(socket, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr))) {
		while (true) {
			char buf[513];
			SOCKADDR from;
			socklen_t fromlen = sizeof(from);
			if (auto len = Syscall::recv(socket, buf, 512, 0, reinterpret_cast<struct sockaddr *>(&from), &fromlen)) {
				if (len.value() == 0) {
					cout << "Server recv EOF" << endl;
					break;
				} else {
					buf[len.value()] = 0;
					cout << "Server recv: " << buf << endl;
					rot13(buf, len.value());
					if (auto slen = Syscall::send(socket, buf, len.value(), 0, reinterpret_cast<struct sockaddr *>(&from), fromlen)) {
						if (slen.value() != len.value()) {
							cerr << "Server send incomplete: " << slen.value() << " vs " << len.value() << endl;
						}
					} else {
						cerr << "Server send failed: " << slen.error_message() << endl;
					}
				}
			} else {
				cerr << "Server recv failed: " << len.error_message() << endl;
			}
		}
	} else {
		cerr << "Server bind socket failed: " << bind.error_message() << endl;
	}
}

template<typename SOCKADDR>
static void stream_server(int socket, SOCKADDR server_addr) {
	if (auto bind = Syscall::bind(socket, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr))) {
		if (auto listen = Syscall::listen(socket, 23)) {
			SOCKADDR client_addr;
			socklen_t client_addrlen = sizeof(client_addr);
			if (auto accept = Syscall::accept(socket, reinterpret_cast<struct sockaddr *>(&client_addr), &client_addrlen)) {
				while (true) {
					char buf[513];
					if (auto len = Syscall::recv(accept.value(), buf, 512, 0)) {
						if (len.value() == 0) {
							cout << "Server recv EOF" << endl;
							break;
						} else {
							buf[len.value()] = 0;
							cout << "Server recv: " << buf << endl;
							rot13(buf, len.value());
							if (auto slen = Syscall::send(accept.value(), buf, len.value(), 0)) {
								if (slen.value() != len.value()) {
									cerr << "Server send incomplete: " << slen.value() << " vs " << len.value() << endl;
								}
							} else {
								cerr << "Server send failed: " << slen.error_message() << endl;
							}
						}
					} else {
						cerr << "Server recv failed: " << len.error_message() << endl;
						break;
					}
				}
			} else {
				cerr << "Server accept failed: " << accept.error_message() << endl;
			}
		} else {
			cerr << "Server listen failed: " << listen.error_message() << endl;
		}
	} else {
		cerr << "Server bind socket failed: " << bind.error_message() << endl;
	}
}

template<typename SOCKADDR>
static void client(int socket, SOCKADDR server_addr) {
	if (auto connect = Syscall::connect(socket, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr))) {
		char buf[] = "Hello world!";
		if (auto len = Syscall::send(socket, buf, sizeof(buf), 0)) {
			if (len.value() != static_cast<int>(sizeof(buf))) {
				cerr << "Client send incomplete: " << len.value() << " vs " << sizeof(buf) << endl;
			} else {
				if (auto rlen = Syscall::recv(socket, buf, 512, 0)) {
					buf[sizeof(buf) - 1] = '\0';
					cout << "Client recv: " << buf << endl;
				} else {
					cerr << "Client recv failed: " << rlen.error_message() << endl;
				}
			}
		} else {
			cerr << "Client send failed: " << len.error_message() << endl;
		}
	} else {
		cerr << "Client connect to socket failed: " << connect.error_message() << endl;
	}
}
