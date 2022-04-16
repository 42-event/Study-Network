#include "NChatServer.h"
#include "NChatClient.h"
#include "NRoom.h"

void NChat::NRoom::AddUser(NChatClient& client)
{
    synchronized(this->mutex)
    {
        this->users.emplace(client.GetClientID(), client);
    }

    SDSBuffer packet;
    auto uid = static_cast<int64_t>(client.GetClientID());
    const std::string& name = client.GetName();
    auto nameLen = static_cast<int8_t>(name.length());
    packet.Put<int64_t>(uid);
    packet.Put<int8_t>(nameLen);
    packet.RawPut(name.c_str(), nameLen);
    this->Broadcast(NCommon::PACKET_ID::ROOM_ENTER_NEW_USER_NTF, packet, client.GetClientID());
}

void NChat::NRoom::RemoveUser(NChatClient& client)
{
    synchronized(this->mutex)
    {
        this->users.erase(client.GetClientID());
    }

    SDSBuffer packet;
    int64_t uid = static_cast<int64_t>(client.GetClientID());
    packet.Put<int64_t>(uid);
    this->Broadcast(NCommon::PACKET_ID::ROOM_LEAVE_USER_NTF, packet, client.GetClientID());
}

void NChat::NRoom::SendUserList(NChatClient& client)
{
    SDSBuffer packet;
    synchronized(this->mutex)
    {
        int8_t userCount = static_cast<int8_t>(this->users.size());
        packet.Put<int8_t>(userCount);
        for (auto& kvp : this->users)
        {
            auto uid = static_cast<int64_t>(kvp.first);
            const std::string& name = kvp.second.GetName();
            auto nameLen = static_cast<int8_t>(name.length());
            packet.Put<int64_t>(uid);
            packet.Put<int8_t>(nameLen);
            packet.RawPut(name.c_str(), nameLen);
            if (--userCount == 0)
            {
                // :(
                //users.size() > int8_t_max
                break;
            }
        }
    }
    client.SendPacket(NCommon::PACKET_ID::ROOM_USER_LIST_NTF, packet);
}

void NChat::NRoom::SendChat(NChatClient::_ClientIDType clientID, const std::string& msg)
{
    SDSBuffer packet;
    auto uid = static_cast<int64_t>(clientID);
    auto msgLen = static_cast<NChatServer::_MessageLengthType>(msg.length());
    packet.Put<int64_t>(uid);
    packet.Put<NChatServer::_MessageLengthType>(msgLen);
    packet.RawPut(msg.c_str(), msgLen);
    this->Broadcast(NCommon::PACKET_ID::ROOM_CHAT_NTF, packet);
}
