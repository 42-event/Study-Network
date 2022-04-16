#pragma once

#include "singleton.h"
#include "NRoom.h"

namespace NChat
{
	class NRoomManager : public Singleton<NRoomManager>
	{
	public:
		static void OnRoomEnterReq(NChat::NChatClient& client, const SDSBuffer& buf);
		static void OnRoomLeaveReq(NChat::NChatClient& client, const SDSBuffer& buf);
		static void OnRoomChatReq(NChat::NChatClient& client, const SDSBuffer& buf);

	private:
		std::map<long, std::shared_ptr<NChat::NRoom>> rooms;
		std::recursive_mutex mutex;

	public:
		std::shared_ptr<NChat::NRoom> GetOrNew(long roomNumber)
		{
			auto it = rooms.find(roomNumber);
			if (it != rooms.end())
			{
				return it->second;
			}
			auto newRoom = std::make_shared<NChat::NRoom>(roomNumber);
			rooms.emplace(roomNumber, newRoom);
			return newRoom;
		}
	};
}
