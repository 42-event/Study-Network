#pragma once

#include "Packet.h"

namespace NChat
{
	class NRoom
	{
	public:
		typedef long _RoomNumberType;

	private:
		const _RoomNumberType roomNumber;
		std::map<NChatClient::_ClientIDType, NChatClient&> users;

	public:
		inline NRoom(_RoomNumberType roomNumber)
			: roomNumber(roomNumber)
		{
			;
		}

		inline _RoomNumberType GetRoomNumber() const
		{
			return this->roomNumber;
		}

		void AddUser(NChatClient& client);
		void RemoveUser(NChatClient& client);
		void SendUserList(NChatClient& client);
		void SendChat(NChatClient::_ClientIDType uid, const std::string& msg);

		template <typename T>
		void Broadcast(NCommon::PACKET_ID id, const T& t, NChatClient::_ClientIDType except = 0)
		{
			for (auto& user : this->users)
			{
				if (user.first != except)
				{
					user.second.SendPacket(id, t);
				}
			}
		}
	};
}
