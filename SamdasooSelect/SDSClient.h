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

public:
	inline SDSClient(SOCKET childSocket, const std::string& host)
		: childSocket(childSocket), host(host), recvBuffer(), sendBuffer()
	{

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
	void OnRead() override;
	void OnWrite() override;

	void OnClose();
};
