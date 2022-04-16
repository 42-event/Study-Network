#include "SDSSelect.h"
#include "SDSSocketUtil.h"

#include "SDSException.h"

void SDSSocketUtil::SetOption(SOCKET s, OPTION key, PCSTR val)
{
	int level;
	int optname;
	switch (key)
	{
	case OPTION::RE_USE_ADDRESS:
		level = SOL_SOCKET;
		optname = SO_REUSEADDR;
		break;
	case OPTION::TCP_NO_DELAY:
		level = IPPROTO_TCP;
		optname = TCP_NODELAY;
		break;
	default:
		throw SDSException("Not supported option");
	}
	auto optResult = ::setsockopt(s, level, optname, val, static_cast<int>(std::strlen(val)));
	if (optResult == SOCKET_ERROR)
	{
		/**
		* WSANOTINITIALISED: A successful WSAStartup call must occur before using this function.
		* WSAENETDOWN: The network subsystem has failed.
		* WSAEFAULT: The buffer pointed to by the optval parameter is not in a valid part of the process address space or the optlen parameter is too small.
		* WSAEINPROGRESS: A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
		* WSAEINVAL: The level parameter is not valid, or the information in the buffer pointed to by the optval parameter is not valid.
		* WSAENETRESET: The connection has timed out when SO_KEEPALIVE is set.
		* WSAENOPROTOOPT: The option is unknown or unsupported for the specified provider or socket (see SO_GROUP_PRIORITY limitations).
		* WSAENOTCONN: The connection has been reset when SO_KEEPALIVE is set.
		* WSAENOTSOCK: The descriptor is not a socket.
		*/
		throw SDSWSException(WSAGetLastError());
	}
}

void SDSSocketUtil::SetNonBlocking(SOCKET s, bool enable)
{
	u_long arg = enable ? TRUE : FALSE;
	auto ioctlResult = ::ioctlsocket(s, FIONBIO, &arg);
	if (ioctlResult == SOCKET_ERROR)
	{
		/**
		* WSANOTINITIALISED: A successful WSAStartup call must occur before using this function.
		* WSAENETDOWN: The network subsystem has failed.
		* WSAEINPROGRESS: A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
		* WSAENOTSOCK: The descriptor s is not a socket.
		* WSAEFAULT: The argp parameter is not a valid part of the user address space.
		*/
		throw SDSWSException(WSAGetLastError());
	}
}
