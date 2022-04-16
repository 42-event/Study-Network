#pragma once

#include <SDSSelect.h>
#include <SDSClient.h>

namespace NChat
{
    class NChatClient : public SDSClient
    {
    public:
        NChatClient(SOCKET childSocket, const std::string& childHost)
            : SDSClient(childSocket, childHost)
        {
            ;
        }

        void ProcessPacket(SDSBuffer& buf) override;
        void OnError(int errorNumber) override;
    };
}
