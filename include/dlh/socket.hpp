#pragma once

// TODO: Auto generate from system headers

#include <dlh/types.hpp>
#include <dlh/systypes.hpp>

// Ressource Limits
typedef enum : int {
	RLIMIT_CPU        =  0,	/* CPU time in sec */
	RLIMIT_FSIZE      =  1,	/* Maximum filesize */
	RLIMIT_DATA       =  2,	/* max data size */
	RLIMIT_STACK      =  3,	/* max stack size */
	RLIMIT_CORE       =  4,	/* max core file size */
	RLIMIT_RSS        =  5,	/* max resident set size */
	RLIMIT_NPROC      =  6,	/* max number of processes */
	RLIMIT_NOFILE     =  7,	/* max number of open files */
	RLIMIT_MEMLOCK    =  8,	/* max locked-in-memory address space */
	RLIMIT_AS         =  9,	/* address space limit */
	RLIMIT_LOCKS      = 10,	/* maximum file locks held */
	RLIMIT_SIGPENDING = 11,	/* max number of pending signals */
	RLIMIT_MSGQUEUE   = 12,	/* maximum bytes in POSIX mqueues */
	RLIMIT_NICE       = 13,	/* max nice prio allowed to raise to 0-39 for nice level 19 .. -20 */
	RLIMIT_RTPRIO     = 14,	/* maximum realtime priority */
	RLIMIT_RTTIME     = 15	/* timeout for RT tasks in us */
} rlimit_t;

// Socket types
typedef enum : int {
	SOCK_STREAM    =  1,  /* Sequenced, reliable, connection-based byte streams */
	SOCK_DGRAM     =  2,  /* Connectionless, unreliable datagrams of fixed maximum length */
	SOCK_RAW       =  3,  /* Raw protocol interface */
	SOCK_RDM       =  4,  /* Reliably-delivered messages */
	SOCK_SEQPACKET =  5,  /* Sequenced, reliable, connection-based, datagrams of fixed maximum length */
	SOCK_DCCP      =  6,  /* Datagram Congestion Control Protocol */
	SOCK_PACKET    = 10,  /* Linux specific way of getting packets at the dev level.  For writing rarp and other similar things on the user level */
} socket_type_t;

// Socket constants
typedef enum : int {
	SOL_SOCKET    =  1,
	SO_REUSEADDR  =  2,
	SO_TYPE       =  3,
	SO_ERROR      =  4,
	SO_DONTROUTE  =  5,
	SO_BROADCAST  =  6,
	SO_SNDBUF     =  7,
	SO_RCVBUF     =  8,
	SO_KEEPALIVE  =  9,
	SO_OOBINLINE  = 10,
	SO_LINGER     = 13,
	SO_RCVLOWAT   = 18,
	SO_SNDLOWAT   = 19,
	SO_RCVTIMEO   = 20,
	SO_SNDTIMEO   = 21,
	SO_ACCEPTCONN = 30
} socket_constant_t;

/* Protocol families */
typedef enum : int {
	PF_UNSPEC     =  0,  /* Unspecified */
	PF_LOCAL      =  1,  /* Local to host (pipes and file-domain) */
	PF_UNIX       =  1,  /* POSIX name for PF_LOCAL */
	PF_FILE       =  1,  /* Another non-standard name for PF_LOCAL */
	PF_INET       =  2,  /* IP protocol family */
	PF_AX25       =  3,  /* Amateur Radio AX.25 */
	PF_IPX        =  4,  /* Novell Internet Protocol */
	PF_APPLETALK  =  5,  /* Appletalk DDP */
	PF_NETROM     =  6,  /* Amateur radio NetROM */
	PF_BRIDGE     =  7,  /* Multiprotocol bridge */
	PF_ATMPVC     =  8,  /* ATM PVCs */
	PF_X25        =  9,  /* Reserved for X.25 project */
	PF_INET6      = 10,  /* IP version 6 */
	PF_ROSE       = 11,  /* Amateur Radio X.25 PLP */
	PF_DECNET     = 12,  /* Reserved for DECnet project */
	PF_NETBEUI    = 13,  /* Reserved for 802.2LLC project */
	PF_SECURITY   = 14,  /* Security callback pseudo AF */
	PF_KEY        = 15,  /* PF_KEY key management API */
	PF_NETLINK    = 16,
	PF_ROUTE      = 16,  /* Alias to emulate 4.4BSD */
	PF_PACKET     = 17,  /* Packet family */
	PF_ASH        = 18,  /* Ash */
	PF_ECONET     = 19,  /* Acorn Econet */
	PF_ATMSVC     = 20,  /* ATM SVCs */
	PF_RDS        = 21,  /* RDS sockets */
	PF_SNA        = 22,  /* Linux SNA Project */
	PF_IRDA       = 23,  /* IRDA sockets */
	PF_PPPOX      = 24,  /* PPPoX sockets */
	PF_WANPIPE    = 25,  /* Wanpipe API sockets */
	PF_LLC        = 26,  /* Linux LLC */
	PF_IB         = 27,  /* Native InfiniBand address */
	PF_MPLS       = 28,  /* MPLS */
	PF_CAN        = 29,  /* Controller Area Network */
	PF_TIPC       = 30,  /* TIPC sockets */
	PF_BLUETOOTH  = 31,  /* Bluetooth sockets */
	PF_IUCV       = 32,  /* IUCV sockets */
	PF_RXRPC      = 33,  /* RxRPC sockets */
	PF_ISDN       = 34,  /* mISDN sockets */
	PF_PHONET     = 35,  /* Phonet sockets */
	PF_IEEE802154 = 36,  /* IEEE 802.15.4 sockets */
	PF_CAIF       = 37,  /* CAIF sockets */
	PF_ALG        = 38,  /* Algorithm sockets */
	PF_NFC        = 39,  /* NFC sockets */
	PF_VSOCK      = 40,  /* vSockets */
	PF_KCM        = 41,  /* Kernel Connection Multiplexor */
	PF_QIPCRTR    = 42,  /* Qualcomm IPC Router */
	PF_SMC        = 43,  /* SMC sockets */
	PF_XDP        = 44,  /* XDP sockets */
	PF_MAX        = 45   /* For now. */
} protocol_family_t;

/* Address families */
#define AF_UNSPEC     PF_UNSPEC
#define AF_LOCAL      PF_LOCAL
#define AF_UNIX       PF_UNIX
#define AF_FILE       PF_FILE
#define AF_INET       PF_INET
#define AF_AX25       PF_AX25
#define AF_IPX        PF_IPX
#define AF_APPLETALK  PF_APPLETALK
#define AF_NETROM     PF_NETROM
#define AF_BRIDGE     PF_BRIDGE
#define AF_ATMPVC     PF_ATMPVC
#define AF_X25        PF_X25
#define AF_INET6      PF_INET6
#define AF_ROSE       PF_ROSE
#define AF_DECNET     PF_DECNET
#define AF_NETBEUI    PF_NETBEUI
#define AF_SECURITY   PF_SECURITY
#define AF_KEY        PF_KEY
#define AF_NETLINK    PF_NETLINK
#define AF_ROUTE      PF_ROUTE
#define AF_PACKET     PF_PACKET
#define AF_ASH        PF_ASH
#define AF_ECONET     PF_ECONET
#define AF_ATMSVC     PF_ATMSVC
#define AF_RDS        PF_RDS
#define AF_SNA        PF_SNA
#define AF_IRDA       PF_IRDA
#define AF_PPPOX      PF_PPPOX
#define AF_WANPIPE    PF_WANPIPE
#define AF_LLC        PF_LLC
#define AF_IB         PF_IB
#define AF_MPLS       PF_MPLS
#define AF_CAN        PF_CAN
#define AF_TIPC       PF_TIPC
#define AF_BLUETOOTH  PF_BLUETOOTH
#define AF_IUCV       PF_IUCV
#define AF_RXRPC      PF_RXRPC
#define AF_ISDN       PF_ISDN
#define AF_PHONET     PF_PHONET
#define AF_IEEE802154 PF_IEEE802154
#define AF_CAIF       PF_CAIF
#define AF_ALG        PF_ALG
#define AF_NFC        PF_NFC
#define AF_VSOCK      PF_VSOCK
#define AF_KCM        PF_KCM
#define AF_QIPCRTR    PF_QIPCRTR
#define AF_SMC        PF_SMC
#define AF_XDP        PF_XDP
#define AF_MAX        PF_MAX

//  Socket level values
typedef enum : int {
	SOL_RAW       = 255,
	SOL_DECNET    = 261,
	SOL_X25       = 262,
	SOL_PACKET    = 263,
	SOL_ATM       = 264,  /* ATM layer (cell level) */
	SOL_AAL       = 265,  /* ATM Adaption Layer (packet level) */
	SOL_IRDA      = 266,
	SOL_NETBEUI	  = 267,
	SOL_LLC       = 268,
	SOL_DCCP      = 269,
	SOL_NETLINK   = 270,
	SOL_TIPC      = 271,
	SOL_RXRPC     = 272,
	SOL_PPPOL2TP  = 273,
	SOL_BLUETOOTH = 274,
	SOL_PNPIPE    = 275,
	SOL_RDS       = 276,
	SOL_IUCV      = 277,
	SOL_CAIF      = 278,
	SOL_ALG       = 279,
	SOL_NFC       = 280,
	SOL_KCM       = 281,
	SOL_TLS       = 282,
	SOL_XDP       = 283
} socket_level_t;

typedef enum : int {
	MSG_OOB          = 0x01,        /* Process out-of-band data */
	MSG_PEEK         = 0x02,        /* Peek at incoming messages */
	MSG_DONTROUTE    = 0x04,        /* Don't use local routing */
	MSG_TRYHARD      = 0x04,        /* DECnet uses a different name */
	MSG_CTRUNC       = 0x08,        /* Control data lost before delivery */
	MSG_PROXY        = 0x10,        /* Supply or ask second address */
	MSG_TRUNC        = 0x20,
	MSG_DONTWAIT     = 0x40,        /* Nonblocking IO */
	MSG_EOR          = 0x80,        /* End of record */
	MSG_WAITALL      = 0x100,       /* Wait for a full request */
	MSG_FIN          = 0x200,
	MSG_SYN          = 0x400,
	MSG_CONFIRM      = 0x800,       /* Confirm path validity */
	MSG_RST          = 0x1000,
	MSG_ERRQUEUE     = 0x2000,      /* Fetch message from error queue */
	MSG_NOSIGNAL     = 0x4000,      /* Do not generate SIGPIPE */
	MSG_MORE         = 0x8000,      /* Sender will send more */
	MSG_WAITFORONE   = 0x10000,     /* Wait for at least one packet to return */
	MSG_BATCH        = 0x40000,     /* sendmmsg: more messages coming */
	MSG_ZEROCOPY     = 0x4000000,   /* Use user data in kernel path */
	MSG_FASTOPEN     = 0x20000000,  /* Send data in TCP SYN */
	MSG_CMSG_CLOEXEC = 0x40000000   /* Set close_on_exit for file descriptor received through SCM_RIGHTS */
} msg_flags_t;


typedef unsigned short int sa_family_t;
typedef unsigned int socklen_t;
typedef uint32_t in_addr_t;

struct in_addr {
	in_addr_t s_addr;

	in_addr(in_addr_t s_addr = -1) : s_addr(s_addr) {}
};

typedef enum : in_addr_t {
	INADDR_ANY       = 0x00000000,  /* Address to accept any incoming messages  */
	INADDR_BROADCAST = 0xffffffff,  /* Address to send to all hosts */
	INADDR_NONE      = 0xffffffff,  /* Address indicating an error return */
	INADDR_LOOPBACK  = 0x7f000001   /* Address to loopback in software to local host (127.0.0.1) */
} inaddr_default_t;

namespace Socket {
bool inet_aton(const char *s, struct in_addr *dest);

static inline in_addr_t inet_addr(const char *p) {
	struct in_addr a;
	inet_aton(p, &a);
	return a.s_addr;
}

static inline uint16_t ntohs(uint16_t n) {
	union { int i; char c; } u = { 1 };
	return u.c ? __builtin_bswap16(n) : n;
}

static inline uint32_t ntohl(uint32_t n) {
	union { int i; char c; } u = { 1 };
	return u.c ? __builtin_bswap32(n) : n;
}

static inline uint16_t htons(uint16_t n) {
	union { int i; char c; } u = { 1 };
	return u.c ? __builtin_bswap16(n) : n;
}

static inline uint32_t htonl(uint32_t n) {
	union { int i; char c; } u = { 1 };
	return u.c ? __builtin_bswap32(n) : n;
}

}  // namespace Socket

struct sockaddr {
	sa_family_t sa_family;  /* Common data: address family and length */
	char sa_data[14];       /* Address data */
};

struct sockaddr_un {
	sa_family_t sun_family;  /* Protocol family */
	char sun_path[108];      /* Path name. */

	sockaddr_un(sa_family_t family = PF_UNSPEC, const char * path = nullptr) : sun_family(family) {
		for (size_t i = 0; i < sizeof(sun_path); i++)
			sun_path[i] = path != nullptr && *path != '\0' ? *(path++) : '\0';
	}

	explicit sockaddr_un(const char * path) : sockaddr_un(AF_UNIX, path) {}
};

struct sockaddr_in {
	sa_family_t sin_family;   /* Protocol family */
	uint16_t sin_port;        /* Port number */
	struct in_addr sin_addr;  /* Internet address */
	char sin_zero[8];

	sockaddr_in(sa_family_t family = PF_UNSPEC, uint16_t sin_port = 0, in_addr_t sin_addr = -1)
	: sin_family(family), sin_port(sin_port), sin_addr(sin_addr) {
		for (size_t i = 0; i < sizeof(sin_zero); i++)
			sin_zero[i] = '\0';
	}

	explicit sockaddr_in(in_addr_t addr, uint16_t port) : sockaddr_in(AF_INET, Socket::htons(port), addr) {}

	explicit sockaddr_in(const char * ip, uint16_t port) : sockaddr_in(Socket::inet_addr(ip), port) {}
};
