#pragma once

#include "Packet.h"

namespace NChat
{
	class NRoom
	{
	private:
		const long roomNumber;
		std::map<long, NChat::NChatClient&> users;
		std::recursive_mutex mutex;

	public:
		NRoom(long roomNumber)
			: roomNumber(roomNumber)
		{
			;
		}

		inline long GetRoomNumber() const
		{
			return this->roomNumber;
		}

		void AddUser(NChat::NChatClient& client);

		inline void RemoveUser(NChat::NChatClient client);

		template <typename T, typename NChat::packet_type_t<T> = true>
		void Broadcast(const T& t)
		{

		}
	};
}
