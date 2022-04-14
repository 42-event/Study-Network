#include "pch.h"
#include "SDSWorker.h"

#include "SDSException.h"

void SDSWorker::Run(SDSWorker* $)
{
	while ($->loop.load())
	{
		int nfds = 0;
		fd_set rfds{};
		fd_set wfds{};
		synchronized($->lock)
		{
			//ignore nfds?!
			rfds = $->fds;
			FD_ZERO(&wfds);
			for (auto& kvp : $->units)
			{
				if (!kvp.second->IsReadable())
				{
					FD_CLR(kvp.first, &rfds);
				}
				if (kvp.second->IsWritable())
				{
					FD_SET(kvp.first, &wfds);
				}
			}
		}
		try
		{
			int total = ::select(nfds, &rfds, &wfds, nullptr, nullptr);
			if (total == SOCKET_ERROR)
			{
				/**
				* WSANOTINITIALISED: A successful WSAStartup call must occur before using this function.
				* WSAEFAULT: The Windows Sockets implementation was unable to allocate needed resources for its internal operations, or the readfds, writefds, exceptfds, or timeval parameters are not part of the user address space.
				* WSAENETDOWN: The network subsystem has failed.
				* WSAEINVAL: The time-out value is not valid, or all three descriptor parameters were null.
				* WSAEINTR: A blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall.
				* WSAEINPROGRESS: A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.
				* WSAENOTSOCK: One of the descriptor sets contains an entry that is not a socket. 
				*/
				throw SDSWSException(WSAGetLastError());
			}
			synchronized($->lock)
			{
				for (auto& kvp : $->units)
				{
					try
					{
						if (FD_ISSET(kvp.first, &rfds))
						{
							kvp.second->OnRead();
						}
						if (FD_ISSET(kvp.first, &wfds))
						{
							kvp.second->OnWrite();
						}
					}
					catch (const SDSWSException& ex)
					{
						if (ex.GetError() == WSAECONNRESET)
						{
							std::cout << "Connection Reset" << std::endl;
						}
						else if (IsDebuggerPresent())
						{
							//Local_WS: ���� ó���� ������ ���� ���� ȯ���մϴ�!!
							DebugBreak();
						}
					}
					catch (const SDSException& ex)
					{
						if (IsDebuggerPresent())
						{
							//Local: ���� ó���� ������ ���� ���� ȯ���մϴ�!!
							DebugBreak();
						}
					}
				}
			}
		}
		catch (const SDSException& ex)
		{
			if (IsDebuggerPresent())
			{
				//Global: ���� ó���� ������ ���� ���� ȯ���մϴ�!!
				DebugBreak();
			}
		}
	}
}

void SDSWorker::Init()
{
	synchronized(this->lock)
	{
		this->units.clear();
		FD_ZERO(&this->fds);
	}
}

void SDSWorker::BeginThread()
{
	if (this->loop.exchange(true))
	{
		return;
	}
	this->workerThread = std::thread(SDSWorker::Run, this);
}

void SDSWorker::JoinThread()
{
	if (this->loop.load())
	{
		this->workerThread.join();
	}
}

void SDSWorker::EndThread()
{
	if (this->loop.exchange(false))
	{
		this->workerThread.join();
	}
}

std::shared_ptr<ISDSWorkUnit> SDSWorker::GetUnit(SOCKET key)
{
	synchronized(this->lock)
	{
		auto it = this->units.find(key);
		if (it != this->units.end())
		{
			return it->second;
		}
	}
	return nullptr;
}

bool SDSWorker::InsertUnit(std::shared_ptr<ISDSWorkUnit> unit)
{
	synchronized(this->lock)
	{
		if (this->units.size() < FD_SETSIZE)
		{
			auto socket = unit->GetSocket();
			FD_SET(socket, &this->fds);
			this->units.emplace(socket, unit);
			return true;
		}
	}
	return false;
}

void SDSWorker::RemoveUnit(std::shared_ptr<ISDSWorkUnit> unit)
{
	synchronized(this->lock)
	{
		auto socket = unit->GetSocket();
		FD_CLR(socket, &this->fds);
		this->units.erase(socket);
	}
}
