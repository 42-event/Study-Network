#pragma once

#include "ISDSWorkUnit.h"
#include "SDSBuffer.h"

class SDSClient : public ISDSWorkUnit
{
private:
	SOCKET childSocket;
	const std::string host;
	SDSBuffer recvBuffer;
	SDSBuffer sendBuffer;

protected:
	std::atomic_bool clientDead = false;
	std::recursive_mutex writeLock;

public:
	inline SDSClient(SOCKET childSocket, const std::string& host)
		: childSocket(childSocket), host(host), recvBuffer(), sendBuffer()
	{
		;
	}

	SDSClient(const SDSClient&) = delete;
	SDSClient(SDSClient&&) = delete;
	SDSClient& operator=(const SDSClient&) = delete;
	SDSClient& operator=(SDSClient&&) = delete;
	~SDSClient();

	inline SOCKET GetSocket() const override
	{
		return this->childSocket;
	}

	bool IsReadable() const override;
	bool IsWritable() const override;
	bool IsFinished() const override;
	void OnRead() override;
	void OnWrite() override;
	void OnError(int errorNumber) override;

	virtual void ProcessPacket(SDSBuffer& buf) = 0;

	inline void SendPacket(const char* buf, int len)
	{
		synchronized(this->writeLock)
		{
			this->sendBuffer.RawPut(buf, len);
			OnWrite();
		}
	}

	template<typename T>
	void SendPacket(const T& t)
	{
		synchronized(this->writeLock)
		{
			this->sendBuffer.Put(t);
			OnWrite();
		}
	}
};
