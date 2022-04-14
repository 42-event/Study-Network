#pragma once

#include "ISDSWorkUnit.h"

class SDSServer : public ISDSWorkUnit
{
public:
	static void InitGlobal();
	static void FinalGlobal();

private:
	SOCKET bossSocket = INVALID_SOCKET;
	const std::string host;
	const USHORT port;
	int bossTCPBacklog;
	bool bossReuseAddr = true;
	bool childTCPNodelay = true;
	bool commonNonBlocking = true;
	std::atomic_bool socketCreated = false;
	std::atomic_bool tcpListened = false;

public:
	SDSServer(const SDSServer&) = delete;
	SDSServer(SDSServer&&) = delete;
	SDSServer& operator=(const SDSServer&) = delete;
	SDSServer& operator=(SDSServer&&) = delete;
	SDSServer(const std::string& host, USHORT port, int tcpBacklog);
	~SDSServer();
	bool CreateSocket();
	bool Listen();

	inline void DisableReUseAddress()
	{
		this->bossReuseAddr = FALSE;
	}

	inline void DisableTcpNoDelay()
	{
		this->childTCPNodelay = FALSE;
	}

	inline SOCKET GetSocket() const override
	{
		return this->bossSocket;
	}

	inline const std::string& GetHost() const
	{
		return host;
	}

	inline USHORT GetPort() const
	{
		return port;
	}

	inline bool IsReadable() const override
	{
		return true;
	}

	inline bool IsWritable() const override
	{
		return false;
	}

	void OnRead() override;

	constexpr inline void OnWrite() override
	{

	}
};
