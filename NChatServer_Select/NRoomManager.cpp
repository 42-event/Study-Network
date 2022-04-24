#include <SDSSelect.h>
#include <SDSBuffer.h>

#include "NChatServer.h"
#include "NChatClient.h"
#include "NRoomManager.h"

void NChat::NRoomManager::OnRoomEnterReq(NChatClient& client, const SDSBuffer& buf)
{
    if (client.GetState() != NChatClient::STATE::LOBBY)
    {
        //Invalid State
        return;
    }

    auto& man = NRoomManager::GetInstance();
    auto packet = buf.Get<NCommon::PktRoomEnterReq>();
    synchronized(man.mutex)
    {
        auto room = man.GetOrNew(packet.RoomIndex);
        NCommon::PktRoomEnterRes res;

        res.SetError(NCommon::ERROR_CODE::NONE);
        res.RoomUserUniqueId = static_cast<int64_t>(client.GetClientID());
        room->AddUser(client);
        room->SendUserList(client);
        client.SetState(NChatClient::STATE::ROOM);
        man.clients.emplace(client.GetClientID(), room->GetRoomNumber());
        client.SendPacket(NCommon::PACKET_ID::ROOM_ENTER_RES, res);
    }
}

void NChat::NRoomManager::OnRoomLeaveReq(NChatClient& client, const SDSBuffer& buf)
{
    if (client.GetState() != NChatClient::STATE::ROOM)
    {
        //Invalid State
        return;
    }

    auto& man = NRoomManager::GetInstance();
    //auto packet = buf.Get<NCommon::PktRoomLeaveReq>();
    synchronized(man.mutex)
    {
        auto& room = man.rooms[man.clients[client.GetClientID()]];
        NCommon::PktRoomLeaveRes res;

        res.SetError(NCommon::ERROR_CODE::NONE);
        room->RemoveUser(client);
        client.SetState(NChatClient::STATE::LOBBY);
        man.clients.erase(client.GetClientID());
        client.SendPacket(NCommon::PACKET_ID::ROOM_LEAVE_RES, res);
    }
}

void NChat::NRoomManager::OnRoomChatReq(NChatClient& client, const SDSBuffer& buf)
{
    if (client.GetState() != NChatClient::STATE::ROOM)
    {
        //Invalid State
        return;
    }

    auto& man = NRoomManager::GetInstance();
    auto msgLen = buf.Get<NChatServer::_MessageLengthType>();
    if (msgLen < 0 || msgLen > buf.GetCount() - sizeof(msgLen))
    {
        //Packet Hack
        return;
    }
    std::string msg(&buf.RawGet()[sizeof(msgLen)], static_cast<std::string::size_type>(msgLen));
    synchronized(man.mutex)
    {
        auto& room = man.rooms[man.clients[client.GetClientID()]];

        NCommon::PktRoomChatRes res;
        res.SetError(NCommon::ERROR_CODE::NONE);
        room->SendChat(client.GetClientID(), msg);
        client.SendPacket(NCommon::PACKET_ID::ROOM_CHAT_RES, res);
    }
}

//Without Lock
std::shared_ptr<NChat::NRoom> NChat::NRoomManager::GetOrNew(NRoom::_RoomNumberType roomNumber)
{
    auto it = this->rooms.find(roomNumber);
    if (it != this->rooms.end())
    {
        return it->second;
    }
    auto newRoom = std::make_shared<NRoom>(roomNumber);
    this->rooms.emplace(roomNumber, newRoom);
    return newRoom;
}

void NChat::NRoomManager::OnClose(NChatClient& client)
{
    synchronized(this->mutex)
    {
        auto it = this->clients.find(client.GetClientID());
        if (it != this->clients.end())
        {
            auto& room = this->rooms[it->second];
            room->RemoveUser(client);
        }
    }
}
