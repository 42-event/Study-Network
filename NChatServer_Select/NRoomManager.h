#pragma once

#include "singleton.h"
#include "NRoom.h"

namespace NChat
{
	class NRoomManager : public Singleton<NRoomManager>
	{
	public:
		static void OnRoomEnterReq(NChatClient& client, const SDSBuffer& buf);
		static void OnRoomLeaveReq(NChatClient& client, const SDSBuffer& buf);
		static void OnRoomChatReq(NChatClient& client, const SDSBuffer& buf);

	private:
		std::map<NChatClient::_ClientIDType, NRoom::_RoomNumberType> clients;
		std::map<NRoom::_RoomNumberType, std::shared_ptr<NRoom>> rooms;
		std::recursive_mutex mutex;

	public:
		std::shared_ptr<NChat::NRoom> GetOrNew(NRoom::_RoomNumberType roomNumber);
	};
}
