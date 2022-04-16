#include "NChatClient.h"
#include "NRoom.h"

void NChat::NRoom::AddUser(NChat::NChatClient& client)
{
	users.emplace(client.GetClientID(), client);
}

inline void NChat::NRoom::RemoveUser(NChat::NChatClient client)
{
	users.erase(client.GetClientID());
}
