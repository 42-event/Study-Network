#include "pch.h"
#include "SDSServer.h"

#include "SDSApp.h"
#include "SDSClient.h"
#include "SDSSocketUtil.h"
#include "SDSException.h"

void SDSServer::InitGlobal()
{
	//Request Provide Windows Sockets v2.2
	WORD wsaVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	int startupError = WSAStartup(wsaVersion, &wsaData);
	if (startupError != 0)
	{
		/**
		* WSASYSNOTREADY: The underlying network subsystem is not ready for network communication.
		* WSAVERNOTSUPPORTED: The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation.
		* WSAEINPROGRESS: A blocking Windows Sockets 1.1 operation is in progress.
		* WSAEPROCLIM: A limit on the number of tasks supported by the Windows Sockets implementation has been reached.
		* WSAEFAULT: The lpWSAData parameter is not a valid pointer.
		*/
		throw SDSWSException(startupError);
	}
}

void SDSServer::FinalGlobal()
{
	int cleanupResult = WSACleanup();
	if (cleanupResult == SOCKET_ERROR)
	{
		/**
		* WSANOTINITIALISED: A successful WSAStartup call must occur before using this function.
		* WSAENETDOWN: The network subsystem has failed.
		* WSAEINPROGRESS: A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
		*/
		throw SDSWSException(WSAGetLastError());
	}
}

SDSServer::SDSServer(const std::string& host, USHORT port, int tcpBacklog)
	: host(host), port(port)
{
	this->bossTCPBacklog = tcpBacklog;
}

SDSServer::~SDSServer()
{
	if (this->socketCreated && this->bossSocket != INVALID_SOCKET)
	{
		::closesocket(this->bossSocket);
	}
}

bool SDSServer::CreateSocket()
{
	//Prevent Double Socket
	if (this->socketCreated.exchange(true))
	{
		return false;
	}

	//Create New Boss Socket as TCP
	SOCKET bossSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (bossSocket == INVALID_SOCKET)
	{
		/**
		* WSANOTINITIALISED: A successful WSAStartup call must occur before using this function.
		* WSAENETDOWN: The network subsystem or the associated service provider has failed.
		* WSAEAFNOSUPPORT: The specified address family is not supported. For example, an application tried to create a socket for the AF_IRDA address family but an infrared adapter and device driver is not installed on the local computer.
		* WSAEINPROGRESS: A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
		* WSAEMFILE: No more socket descriptors are available.
		* WSAEINVAL: An invalid argument was supplied. This error is returned if the af parameter is set to AF_UNSPEC and the type and protocol parameter are unspecified.
		* WSAEINVALIDPROVIDER: The service provider returned a version other than 2.2.
		* WSAEINVALIDPROCTABLE: The service provider returned an invalid or incomplete procedure table to the WSPStartup.
		* WSAENOBUFS: No buffer space is available. The socket cannot be created.
		* WSAEPROTONOSUPPORT: The specified protocol is not supported.
		* WSAEPROTOTYPE: The specified protocol is the wrong type for this socket.
		* WSAEPROVIDERFAILEDINIT: The service provider failed to initialize. This error is returned if a layered service provider (LSP) or namespace provider was improperly installed or the provider fails to operate correctly.
		* WSAESOCKTNOSUPPORT: The specified socket type is not supported in this address family.
		*/
		throw SDSWSException(WSAGetLastError());
	}
	this->bossSocket = bossSocket;

	//Set Boss Socket Option (Re-use Address = True)
	SDSSocketUtil::SetOption(this->bossSocket, SDSSocketUtil::OPTION::RE_USE_ADDRESS, this->bossReuseAddr ? "\1" : "\0");

	//Set Non Blocking Socket
	SDSSocketUtil::SetNonBlocking(this->bossSocket, this->commonNonBlocking);

	return true;
}

bool SDSServer::Listen()
{
	//Prevent Double Listen
	if (this->tcpListened.exchange(true))
	{
		return false;
	}

	SOCKADDR_IN addr{ .sin_family = AF_INET };
	if (this->host.empty())
	{
		//정의에 따르면 이미 network-order가 보장되었으므로 htonl은 필요 없다.
		addr.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		auto addrResult = ::inet_pton(addr.sin_family, this->host.c_str(), &addr.sin_addr);
		if (addrResult == 0)
		{
			throw SDSException("not a valid IPv4 dotted-decimal string");
		}
		else if (addrResult < 0)
		{
			/**
			* WSAEAFNOSUPPORT: The address family specified in the Family parameter is not supported. This error is returned if the Family parameter specified was not AF_INET or AF_INET6.
			* WSAEFAULT: The pszAddrString or pAddrBuf parameters are NULL or are not part of the user address space.
			*/
			throw SDSWSException(WSAGetLastError());
		}
	}
	addr.sin_port = ::htons(this->port);

	auto bindResult = ::bind(this->bossSocket, reinterpret_cast<LPSOCKADDR>(&addr), sizeof(addr));
	if (bindResult == SOCKET_ERROR)
	{
		/**
		* WSANOTINITIALISED: A successful WSAStartup call must occur before using this function.
		* WSAENETDOWN: The network subsystem has failed.
		* WSAEACCES: An attempt was made to access a socket in a way forbidden by its access permissions.
		*		This error is returned if nn attempt to bind a datagram socket to the broadcast address failed because the setsockopt option SO_BROADCAST is not enabled.
		* WSAEADDRINUSE: Only one usage of each socket address (protocol/network address/port) is normally permitted.
		*		This error is returned if a process on the computer is already bound to the same fully qualified address and the socket has not been marked to allow address reuse with SO_REUSEADDR. For example, the IP address and port specified in the name parameter are already bound to another socket being used by another application. For more information, see the SO_REUSEADDR socket option in the SOL_SOCKET Socket Options reference, Using SO_REUSEADDR and SO_EXCLUSIVEADDRUSE, and SO_EXCLUSIVEADDRUSE.
		* WSAEADDRNOTAVAIL: The requested address is not valid in its context.
		*		This error is returned if the specified address pointed to by the name parameter is not a valid local IP address on this computer.
		* WSAEFAULT: The system detected an invalid pointer address in attempting to use a pointer argument in a call.
		*		This error is returned if the name parameter is NULL, the name or namelen parameter is not a valid part of the user address space, the namelen parameter is too small, the name parameter contains an incorrect address format for the associated address family, or the first two bytes of the memory block specified by name do not match the address family associated with the socket descriptor s.
		* WSAEINPROGRESS: A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
		* WSAEINVAL: An invalid argument was supplied.
		*		This error is returned of the socket s is already bound to an address.
		* WSAENOBUFS: Typically, WSAENOBUFS is an indication that there aren't enough ephemeral ports to allocate for the bind.
		* WSAENOTSOCK: An operation was attempted on something that is not a socket.
		*		This error is returned if the descriptor in the s parameter is not a socket.
		*/
		throw SDSWSException(WSAGetLastError());
	}

	auto listenResult = ::listen(this->bossSocket, this->bossTCPBacklog);
	if (listenResult == SOCKET_ERROR)
	{
		/**
		* WSANOTINITIALISED: A successful WSAStartup call must occur before using this function.
		* WSAENETDOWN: The network subsystem has failed.
		* WSAEADDRINUSE: The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR. This error usually occurs during execution of the bind function, but could be delayed until this function if the bind was to a partially wildcard address (involving ADDR_ANY) and if a specific address needs to be committed at the time of this function.
		* WSAEINPROGRESS: A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
		* WSAEINVAL: The socket has not been bound with bind.
		* WSAEISCONN: The socket is already connected.
		* WSAEMFILE: No more socket descriptors are available.
		* WSAENOBUFS: No buffer space is available.
		* WSAENOTSOCK: The descriptor is not a socket.
		* WSAEOPNOTSUPP: The referenced socket is not of a type that supports the listen operation.
		*/
		throw SDSWSException(WSAGetLastError());
	}

	return true;
}

void SDSServer::OnRead()
{
	SOCKADDR_IN addr{};
	auto addrlen = static_cast<socklen_t>(sizeof(addr));
	SOCKET childSocket = ::accept(this->bossSocket, reinterpret_cast<LPSOCKADDR>(&addr), &addrlen);
	if (childSocket == INVALID_SOCKET)
	{
		/**
		* WSANOTINITIALISED: A successful WSAStartup call must occur before using this function.
		* WSAECONNRESET: An incoming connection was indicated, but was subsequently terminated by the remote peer prior to accepting the call.
		* WSAEFAULT: The addrlen parameter is too small or addr is not a valid part of the user address space.
		* WSAEINTR: A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall.
		* WSAEINVAL: The listen function was not invoked prior to accept.
		* WSAEINPROGRESS: A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
		* WSAEMFILE: The queue is nonempty upon entry to accept and there are no descriptors available.
		* WSAENETDOWN: The network subsystem has failed.
		* WSAENOBUFS: No buffer space is available.
		* WSAENOTSOCK: The descriptor is not a socket.
		* WSAEOPNOTSUPP: The referenced socket is not a type that supports connection-oriented service.
		* WSAEWOULDBLOCK: The socket is marked as nonblocking and no connections are present to be accepted. 
		*/
		throw SDSWSException(WSAGetLastError());
	}

	//Set Child Socket Option (TCP No-delay = True)
	SDSSocketUtil::SetOption(childSocket, SDSSocketUtil::OPTION::TCP_NO_DELAY, this->childTCPNodelay ? "\1" : "\0");

	//Set Non Blocking Socket
	SDSSocketUtil::SetNonBlocking(childSocket, this->commonNonBlocking);

	///Retrieve Client Host
	std::array<std::string::value_type, 128> arr{};
	std::string childHost = ::inet_ntop(addr.sin_family, &addr.sin_addr, arr.data(), arr.size());

	//Add ChildSocket To Worker
	auto client = std::make_shared<SDSClient>(childSocket, childHost);
	SDSApp::worker->InsertUnit(client);

	/// !!! TEST !!!
	std::cout << "OnAccept: " << childHost.c_str() << std::endl;
}
