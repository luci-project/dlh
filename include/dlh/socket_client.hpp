#pragma once

#include <dlh/log.hpp>
#include <dlh/socket.hpp>

namespace Socket {

class Client {
	int socket = -1;

public:
	Client(const char* host = nullptr) {
		if (host != nullptr)
			connect(host);
	}

	/* Determine connection from string
	   Supported types are
	      unix:SERVER_FILE              (for stream)
	      unix:SERVER_FILE:CLIENT_FILE  (for datagram)
	      tcp:IP:PORT
	      udp:IP:PORT
	*/
	bool connect(const char * host);

	/* Connect via tcp/udp */
	bool connect_inet(const char * ip, uint16_t port, bool tcp = true);

	/* Connect via unix domain socket */
	bool connect_unix(const char * server_path, const char * client_path = nullptr);

	bool is_connected() {
		return socket >= 0;
	}

	/* Disconnect from socket */
	bool disconnect();

	/* (Blocking) send data via socket */
	size_t send(const char * data, size_t len, bool retry = true);

	/* (Blocking) receive data via socket
	   if until_eol is set full buffer might be tampered (independent from return value) */
	size_t recv(char * buf, size_t len, bool until_eol = false);

	/* Check if data is waiting to be received */
	bool has_data();
};

}  // namespace Socket
