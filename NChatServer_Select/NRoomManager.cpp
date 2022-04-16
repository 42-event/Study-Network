#include <SDSSelect.h>
#include <SDSBuffer.h>
#include "NChatClient.h"
#include "NRoomManager.h"

void NChat::NRoomManager::OnRoomEnterReq(NChat::NChatClient& client, const SDSBuffer& buf)
{
    if (client.GetState() != NChat::NChatClient::STATE::LOBBY)
    {
        //Invalid State
        return;
    }
    auto& man = NChat::NRoomManager::GetInstance();
    auto packet = buf.Get<NCommon::PktRoomEnterReq>();
    NCommon::PktRoomEnterRes res;
    res.SetError(NCommon::ERROR_CODE::NONE);
    auto room = man.GetOrNew(packet.RoomIndex);
    room->AddUser(client);
    client.SendPacket(NCommon::PACKET_ID::ROOM_ENTER_RES, res);
    //USER_LIST_NTF to self
    //Broadcast
}

void NChat::NRoomManager::OnRoomLeaveReq(NChat::NChatClient& client, const SDSBuffer& buf)
{
    if (client.GetState() != NChat::NChatClient::STATE::ROOM)
    {
        //Invalid State
        return;
    }
    auto& man = NChat::NRoomManager::GetInstance();
    auto packet = buf.Get<NCommon::PktRoomLeaveReq>();
    NCommon::PktRoomLeaveRes res;
    res.SetError(NCommon::ERROR_CODE::NONE);
    //room->RemoveUser
    client.SendPacket(NCommon::PACKET_ID::ROOM_LEAVE_RES, res);
    //Broadcast
}

void NChat::NRoomManager::OnRoomChatReq(NChat::NChatClient& client, const SDSBuffer& buf)
{
    if (client.GetState() != NChat::NChatClient::STATE::ROOM)
    {
        //Invalid State
        return;
    }
    auto& man = NChat::NRoomManager::GetInstance();
    auto packet = buf.Get<NCommon::PktRoomChatReq>();
    NCommon::PktRoomChatRes res;
    res.SetError(NCommon::ERROR_CODE::NONE);
    //room->SendChat
    client.SendPacket(NCommon::PACKET_ID::ROOM_CHAT_RES, res);
    //Broadcast
}
