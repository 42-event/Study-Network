#include <SDSSelect.h>
#include <SDSBuffer.h>
#include "NChatClient.h"
#include "NLoginManager.h"

void NChat::NLoginManager::OnLoginReq(NChat::NChatClient& client, const SDSBuffer& buf)
{
    if (client.GetState() != NChat::NChatClient::STATE::LOGIN)
    {
        //Invalid State
        return;
    }
    auto& man = NChat::NLoginManager::GetInstance();
    auto packet = buf.Get<NCommon::PktLogInReq>();
    NCommon::PktLogInRes res;
    res.SetError(NCommon::ERROR_CODE::NONE);
    synchronized(man.mutex)
    {
        auto info = man.id2pw.find(packet.szID);
        if (info == man.id2pw.end())
        {
            //Create New Member
            man.id2pw.emplace(packet.szID, packet.szPW);
        }
        else if (packet.szPW == info->second)
        {
            //Correct Password
            ;
        }
        else
        {
            //Wrong Password
            res.SetError(NCommon::ERROR_CODE::UNASSIGNED_ERROR);
        }
    }
    if (static_cast<NCommon::ERROR_CODE>(res.ErrorCode) == NCommon::ERROR_CODE::NONE)
    {
        man.online.emplace(client.GetClientID(), client);
        client.SetState(NChat::NChatClient::STATE::LOBBY);
    }
    client.SendPacket(NCommon::PACKET_ID::LOGIN_IN_RES, res);
}
