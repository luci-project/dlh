#include <dlh/stream/output.hpp>
#include <dlh/syscall.hpp>
#include <dlh/mem.hpp>

struct sockaddr_un server_addr = { AF_UNIX, "test_socket.server" };
struct sockaddr_un client_addr = { AF_UNIX, "test_socket.client" };

static void server() {
	if (auto fd = Syscall::socket(PF_UNIX, SOCK_DGRAM, 0)) {
		if (auto bind = Syscall::bind(fd.value(), (struct sockaddr *)&server_addr, sizeof(server_addr))) {
			while (true) {
				char buf[513];
				struct sockaddr_un from;
				socklen_t fromlen = sizeof(from);
				if (auto len = Syscall::recv(fd.value(), buf, 512, 0, (struct sockaddr *)&from, &fromlen)) {
					if (len.value() == 0) {
						cout << "Server recv EOF" << endl;
						break;
					} else {
						buf[len.value()] = 0;
						cout << "Server recv: " << buf << endl;
						for (ssize_t i = 0; i < len.value() ; i++) {
							if ((buf[i] >= 'a' && buf[i] <= 'm') || (buf[i] >= 'A' && buf[i] <= 'M')) {
								buf[i] += 13;
							} else if ((buf[i] >= 'n' && buf[i] <= 'z') || (buf[i] >= 'N' && buf[i] <= 'Z')) {
								buf[i] -= 13;
							}
						}
						if (auto slen = Syscall::send(fd.value(), buf, len.value(), 0, (struct sockaddr *)&from, fromlen)) {
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
		Syscall::close(fd);
	} else {
		cerr << "Server creating socket failed: " << fd.error_message() << endl;
	}
}

static void client() {
	if (auto fd = Syscall::socket(PF_UNIX, SOCK_DGRAM, 0)) {
		if (auto bind = Syscall::bind(fd.value(), (struct sockaddr *)&client_addr, sizeof(client_addr))) {
			if (auto connect = Syscall::connect(fd.value(), (struct sockaddr *)&server_addr, sizeof(server_addr))) {
				char buf[] = "Hello world!";
				if (auto len = Syscall::send(fd.value(), buf, sizeof(buf), 0)) {
					if (len.value() != static_cast<int>(sizeof(buf))) {
						cerr << "Client send incomplete: " << len.value() << " vs " << sizeof(buf) << endl;
					} else {
						if (auto rlen = Syscall::recv(fd.value(), buf, 512, 0)) {
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
		} else {
			cerr << "Client bind socket failed: " << bind.error_message() << endl;
		}
		Syscall::close(fd);
	} else {
		cerr << "Client creating socket failed: " << fd.error_message() << endl;
	}
}

int main() {
	/*Mem::set(&server_addr, 0, sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, server_socket);*/
	Syscall::unlink(server_addr.sun_path);
	Syscall::unlink(client_addr.sun_path);

	if (auto pid = Syscall::fork()) {
		if (pid == 0) {
			server();
		} else {
			Syscall::sleep(1);
			client();
		}
	} else {
		cerr << "Fork failed: " << pid.error_message() << endl;
	}

	return 0;
}
