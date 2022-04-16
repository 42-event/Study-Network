#include "SDSSelect.h"
#include "SDSClient.h"

#include "SDSException.h"
#include "SDSSocketUtil.h"

SDSClient::~SDSClient()
{
	if (this->childSocket != INVALID_SOCKET)
	{
		::closesocket(this->childSocket);
	}
}

bool SDSClient::IsReadable() const
{
	return true;
}

bool SDSClient::IsWritable() const
{
	return this->sendBuffer.GetCount() > 0;
}

bool SDSClient::IsFinished() const
{
	return this->clientDead;
}

void SDSClient::OnRead()
{
	constexpr int len = SDSSocketUtil::BufferSize;
	char buf[len];
	auto recvResult = ::recv(this->childSocket, buf, len, 0);
	if (recvResult == SOCKET_ERROR)
	{
		/**
		* WSANOTINITIALISED: A successful WSAStartup call must occur before using this function.
		* WSAENETDOWN: The network subsystem has failed.
		* WSAEFAULT: The buf parameter is not completely contained in a valid part of the user address space.
		* WSAENOTCONN: The socket is not connected.
		* WSAEINTR: The (blocking) call was canceled through WSACancelBlockingCall.
		* WSAEINPROGRESS: A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
		* WSAENETRESET: For a connection-oriented socket, this error indicates that the connection has been broken due to keep-alive activity that detected a failure while the operation was in progress. For a datagram socket, this error indicates that the time to live has expired.
		* WSAENOTSOCK: The descriptor is not a socket.
		* WSAEOPNOTSUPP: MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only send operations.
		* WSAESHUTDOWN: The socket has been shut down; it is not possible to receive on a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH.
		* WSAEWOULDBLOCK: The socket is marked as nonblocking and the receive operation would block.
		* WSAEMSGSIZE: The message was too large to fit into the specified buffer and was truncated.
		* WSAEINVAL: The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled or (for byte stream sockets only) len was zero or negative.
		* WSAECONNABORTED: The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable.
		* WSAETIMEDOUT: The connection has been dropped because of a network failure or because the peer system failed to respond.
		* WSAECONNRESET: The virtual circuit was reset by the remote side executing a hard or abortive close. The application should close the socket as it is no longer usable. On a UDP-datagram socket, this error would indicate that a previous send operation resulted in an ICMP "Port Unreachable" message. 
		*/
		auto errorNumber = WSAGetLastError();
		if (errorNumber == WSAEWOULDBLOCK)
		{
			return;
		}
		throw SDSWSException(errorNumber);
	}
	if (recvResult == 0)
	{
		throw SDSWSException(WSABASEERR);
	}
	else
	{
		this->recvBuffer.RawPut(buf, recvResult);
		this->ProcessPacket(this->recvBuffer);
	}
}

void SDSClient::OnWrite()
{
	synchronized(this->writeLock)
	{
		const char* buf = this->sendBuffer.RawGet();
		int len = this->sendBuffer.GetCount();
		auto sendResult = ::send(this->childSocket, buf, len, 0);
		if (sendResult == SOCKET_ERROR)
		{
			/**
			* WSANOTINITIALISED: A successful WSAStartup call must occur before using this function.
			* WSAENETDOWN: The network subsystem has failed.
			* WSAEACCES: The requested address is a broadcast address, but the appropriate flag was not set. Call setsockopt with the SO_BROADCAST socket option to enable use of the broadcast address.
			* WSAEINTR: A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall.
			* WSAEINPROGRESS: A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
			* WSAEFAULT: The buf parameter is not completely contained in a valid part of the user address space.
			* WSAENETRESET: The connection has been broken due to the keep-alive activity detecting a failure while the operation was in progress.
			* WSAENOBUFS: No buffer space is available.
			* WSAENOTCONN: The socket is not connected.
			* WSAENOTSOCK: The descriptor is not a socket.
			* WSAEOPNOTSUPP: MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only receive operations.
			* WSAESHUTDOWN: The socket has been shut down; it is not possible to send on a socket after shutdown has been invoked with how set to SD_SEND or SD_BOTH.
			* WSAEWOULDBLOCK: The socket is marked as nonblocking and the requested operation would block.
			* WSAEMSGSIZE: The socket is message oriented, and the message is larger than the maximum supported by the underlying transport.
			* WSAEHOSTUNREACH: The remote host cannot be reached from this host at this time.
			* WSAEINVAL: The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled.
			* WSAECONNABORTED: The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable.
			* WSAECONNRESET: The virtual circuit was reset by the remote side executing a hard or abortive close. For UDP sockets, the remote host was unable to deliver a previously sent UDP datagram and responded with a "Port Unreachable" ICMP packet. The application should close the socket as it is no longer usable.
			* WSAETIMEDOUT: The connection has been dropped, because of a network failure or because the system on the other end went down without notice.
			*/
			auto errorNumber = WSAGetLastError();
			if (errorNumber == WSAEWOULDBLOCK)
			{
				return;
			}
			throw SDSWSException(errorNumber);
		}
		this->sendBuffer.Delete(sendResult);
	}
}

void SDSClient::OnError(int errorNumber)
{
	this->clientDead = true;
	if (errorNumber == WSABASEERR)
	{
		//Gracefully Shutdown
	}
	else if (errorNumber == WSAECONNRESET)
	{
		//Connection Reset
	}
	else if (IsDebuggerPresent())
	{
		//WS: 예외 처리의 지옥에 오신 것을 환영합니다!!
		DebugBreak();
	}
}
