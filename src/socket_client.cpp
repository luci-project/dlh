#include <dlh/socket_client.hpp>

#include <dlh/log.hpp>
#include <dlh/mem.hpp>
#include <dlh/string.hpp>
#include <dlh/socket.hpp>
#include <dlh/parser/string.hpp>

namespace Socket {

bool Client::connect(const char * host) {
	bool r = false;
	if (host != nullptr && String::len(host) > 0) {
		auto parts = String::split(host, ':', 2);
		auto psize = parts.size();
		uint16_t port = 0;
		if (psize >= 2 && String::compare(parts[0], "unix") == 0)
			r = connect_unix(parts[1], psize == 3 ? parts[2] : nullptr);
		else if (psize == 3 && String::compare(parts[0], "tcp") == 0 && Parser::string(port, parts[2]))
			r = connect_inet(parts[1], port, true);
		else if (psize == 3 && String::compare(parts[0], "udp") == 0 && Parser::string(port, parts[2]))
			r = connect_inet(parts[1], port, false);
		else
			LOG_ERROR << "Unknown protocol " << parts[0] << endl;
		if (psize > 0)
			for (auto & p: parts)
				Memory::free(p);
	}
	return r;
}

bool Client::connect_unix(const char * server_path, const char * client_path) {
	if (is_connected())
		return false;

	if (auto fd = Syscall::socket(PF_UNIX, client_path != nullptr ? SOCK_DGRAM : SOCK_STREAM, 0)) {
		if (client_path != nullptr) {
			sockaddr_un client_addr{client_path};
			if (auto bind = Syscall::bind(fd.value(), reinterpret_cast<struct sockaddr *>(&client_addr), sizeof(client_addr))) {
				LOG_TRACE << "Client binded to socket " << client_path << endl;
			} else {
				LOG_ERROR << "Client bind to socket " << client_path << " failed: " << bind.error_message() << endl;
				Syscall::close(fd.value());
				return false;
			}
		}

		sockaddr_un server_addr{server_path};
		if (auto connect = Syscall::connect(fd.value(), reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr))) {
			LOG_TRACE << "Client connected to server socket " << server_path << endl;
			socket = fd.value();
			return true;
		} else {
			LOG_ERROR << "Client connect to server socket " << server_path << " failed: " << connect.error_message() << endl;
			Syscall::close(fd.value());
			return false;
		}
	} else {
		LOG_ERROR << "Creating unix socket failed: " << fd.error_message() << endl;
		return false;
	}
}

bool Client::connect_inet(const char * ip, uint16_t port, bool tcp) {
	if (is_connected())
		return false;

	if (auto fd = Syscall::socket(PF_INET, tcp ? SOCK_STREAM : SOCK_DGRAM, 0)) {
		sockaddr_in server_addr{ip, port};
		if (auto connect = Syscall::connect(fd.value(), reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr))) {
			LOG_TRACE << "Client connected to server socket " << ip << ':' << port << endl;
			socket = fd.value();
			return true;
		} else {
			LOG_ERROR << "Client connect to server socket " << ip << ':' << port << " failed: " << connect.error_message() << endl;
			Syscall::close(fd.value());
			return false;
		}
	} else {
		LOG_ERROR << "Creating unix socket failed: " << fd.error_message() << endl;
		return false;
	}
}

bool Client::disconnect() {
	if (is_connected()) {
		int tmp = socket;
		socket = -1;
		if (auto close = Syscall::close(tmp)) {
			LOG_TRACE << "Client closed socket" << endl;
			socket = -1;
			return true;
		} else {
			LOG_ERROR << "Closeing socket failed: " << close.error_message() << endl;
		}
	}
	return false;
}

size_t Client::send(const char * data, size_t len, bool retry) {
	size_t sent = 0;
	if (is_connected())
		while (true) {
			if (auto send = Syscall::send(socket, data, len, 0)) {
				assert(send.value() >= 0);
				sent += send.value();
				if (static_cast<size_t>(send.value()) < len && retry) {
					data += send.value();
					len -= send.value();
				} else {
					break;
				}
			} else {
				LOG_ERROR << "Sent failed: " << send.error_message() << " (disconnecting)" << endl;
				disconnect();
				break;
			}
		}
	return sent;
}

size_t Client::recv(char * buf, size_t len, bool until_eol) {
	size_t received = 0;
	if (is_connected()) {
		if (until_eol) {
			while (true) {
				if (auto peek = Syscall::recv(socket, buf, len, MSG_PEEK)) {
					size_t plen = peek.value();
					size_t slurp = 0;
					for (size_t i = 0; i < plen ; i++)
						if (buf[i] == '\0' || buf[i] == '\r' || buf[i] == '\n') {
							plen = i + 1;
							slurp++;
						} else if (slurp > 0) {
							break;
						}
					if (plen == 0) {
						buf[0] = '\0';
						break;
					}
					if (auto recv = Syscall::recv(socket, buf, plen, 0)) {
						size_t rlen = recv.value();
						received += rlen;
						if (rlen >= plen - slurp) {
							for (size_t i = plen - slurp; i < rlen ; i++)
								buf[i] = '\0';
							break;
						} else {
							buf += rlen;
							len -= rlen;
						}
					} else {
						LOG_ERROR << "Receive of " << len << " bytes failed: " << recv.error_message() << " (disconnecting)" << endl;
						disconnect();
						break;
					}
				} else {
					LOG_ERROR << "Receive (peek) failed: " << peek.error_message() << " (disconnecting)" << endl;
					disconnect();
					break;
				}
			}
		} else {
			if (auto recv = Syscall::recv(socket, buf, len, 0)) {
				received = recv.value();
			} else {
				LOG_ERROR << "Receive failed: " << recv.error_message() << " (disconnecting)" << endl;
				disconnect();
			}
		}
	}
	return received;
}

bool Client::has_data() {
	if (is_connected()) {
		char buf;
		if (auto recv = Syscall::recv(socket, &buf, sizeof(char), MSG_PEEK | MSG_DONTWAIT)) {
			return true;
		} else if (recv.error() != EWOULDBLOCK) {
			LOG_ERROR << "Receive failed: " << recv.error_message() << " (disconnecting)" << endl;
			disconnect();
		}
	}
	return false;
}

}  // namespace Socket
