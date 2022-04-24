#include <SDSSelect.h>
#include <SDSBuffer.h>

#include "NChatClient.h"
#include "NLoginManager.h"

void NChat::NLoginManager::OnLoginReq(NChat::NChatClient& client, const SDSBuffer& buf)
{
    if (client.GetState() != NChatClient::STATE::LOGIN)
    {
        //Invalid State
        return;
    }
    auto& man = NLoginManager::GetInstance();
    auto packet = buf.Get<NCommon::PktLogInReq>();
    NCommon::PktLogInRes res;
    res.SetError(NCommon::ERROR_CODE::NONE);
    synchronized(man.mutex)
    {
        const std::string& name = packet.szID;
        auto info = man.id2pw.find(name);
        if (info == man.id2pw.end())
        {
            //Create New Member
            man.id2pw.emplace(name, packet.szPW);
        }
        else if (packet.szPW != info->second)
        {
            //Wrong Password
            res.SetError(NCommon::ERROR_CODE::USER_MGR_NOT_CONFIRM_USER);
        }
        else if (man.name2client.find(name) != man.name2client.end())
        {
            //Already Logged-in ID
            res.SetError(NCommon::ERROR_CODE::USER_MGR_ID_DUPLICATION);
        }
        else
        {
            //Correct Password
            ;
        }
        if (static_cast<NCommon::ERROR_CODE>(res.ErrorCode) == NCommon::ERROR_CODE::NONE)
        {
            client.SetName(name);
            client.SetState(NChatClient::STATE::LOBBY);
            man.online.emplace(client.GetClientID(), client);
            man.name2client.emplace(client.GetName(), client.GetClientID());
        }
    }
    client.SendPacket(NCommon::PACKET_ID::LOGIN_IN_RES, res);
}

void NChat::NLoginManager::OnClose(NChatClient& client)
{
    synchronized(this->mutex)
    {
        this->online.erase(client.GetClientID());
        this->name2client.erase(client.GetName());
    }
}
