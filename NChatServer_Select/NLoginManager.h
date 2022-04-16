#pragma once

#include "singleton.h"
#include "Packet.h"

namespace NChat
{
	class NLoginManager : public Singleton<NLoginManager>
	{
	public:
		static void OnLoginReq(NChatClient& client, const SDSBuffer& buf);

	private:
		std::unordered_map<std::string, std::string> id2pw; // :(
		std::unordered_map<NChatClient::_ClientIDType, NChatClient&> online;
		std::recursive_mutex mutex;
	};
}
