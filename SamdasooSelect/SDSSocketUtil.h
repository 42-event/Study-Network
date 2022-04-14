#pragma once

class SDSSocketUtil
{
public:
	SDSSocketUtil() = delete;
	~SDSSocketUtil() = delete;
	SDSSocketUtil(const SDSSocketUtil&) = delete;
	SDSSocketUtil(SDSSocketUtil&&) = delete;
	SDSSocketUtil& operator=(const SDSSocketUtil&) = delete;
	SDSSocketUtil& operator=(SDSSocketUtil&&) = delete;

	enum class OPTION
	{
		RE_USE_ADDRESS,
		TCP_NO_DELAY
	};

public:
	static void SetOption(SOCKET s, OPTION key, PCSTR val);
	static void SetNonBlocking(SOCKET s, bool enable);
};
