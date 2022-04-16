#include "NPacketProcessor.h"
#include "NLoginManager.h"
#include "NRoomManager.h"

std::unordered_map<NCommon::PACKET_ID, std::function<void(NChat::NChatClient&, const SDSBuffer&)>> NChat::NPacketProcessor::map
{
	{ NCommon::PACKET_ID::LOGIN_IN_REQ, &NLoginManager::OnLoginReq },
	{ NCommon::PACKET_ID::ROOM_ENTER_REQ, &NRoomManager::OnRoomEnterReq },
	{ NCommon::PACKET_ID::ROOM_LEAVE_REQ, &NRoomManager::OnRoomLeaveReq },
	{ NCommon::PACKET_ID::ROOM_CHAT_REQ, &NRoomManager::OnRoomChatReq },
};

void NChat::NPacketProcessor::Process(NChatClient& client, NCommon::PACKET_ID id, const SDSBuffer& buf)
{
	auto e = map.find(id);
	if (e != map.end())
	{
		e->second(client, buf);
	}
}
