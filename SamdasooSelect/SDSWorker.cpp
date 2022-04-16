#include "SDSSelect.h"
#include "SDSWorker.h"

#include "SDSException.h"

void SDSWorker::Run(SDSWorker* $)
{
	while ($->loop)
	{
		int nfds = 0;
		fd_set rfds{};
		fd_set wfds{};
		std::map<SOCKET, std::shared_ptr<ISDSWorkUnit>> snapshot;
		synchronized($->lock)
		{
			snapshot = $->units;
		}
		//ignore nfds?!
		rfds = $->fds;
		FD_ZERO(&wfds);
		for (auto& kvp : snapshot)
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
			else if (total == 0)
			{
				continue;
			}
			for (auto& kvp : snapshot)
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
					kvp.second->OnError(ex.GetError());
				}
				catch (const SDSException& ex)
				{
					if (IsDebuggerPresent())
					{
						//Local: 예외 처리의 지옥에 오신 것을 환영합니다!!
						DebugBreak();
					}
				}
				if (kvp.second->IsFinished())
				{
					$->RemoveUnit(kvp.second);
				}
			}
		}
		catch (const SDSException& ex)
		{
			if (IsDebuggerPresent())
			{
				//Global: 예외 처리의 지옥에 오신 것을 환영합니다!!
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
