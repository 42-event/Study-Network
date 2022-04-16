#pragma once

#include <SDSSelect.h>
#include <SDSException.h>

#include "NChatClient.h"
#include "Packet.h"

namespace NChat
{
	class NPacketProcessor
	{
	private:
		static std::unordered_map<NCommon::PACKET_ID, std::function<void(NChat::NChatClient&, const SDSBuffer&)>> map;

	public:
		static void Process(NChatClient& client, NCommon::PACKET_ID id, const SDSBuffer& buf);
	};
}
