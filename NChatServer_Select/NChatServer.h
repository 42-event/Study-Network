#pragma once

#include <SDSSelect.h>
#include <SDSServer.h>

#include "NChatClient.h"

namespace NChat
{
    class NChatServer : public SDSServer
    {
    private:
        std::atomic_long clientIDCounter;

    public:
        inline NChatServer(const std::string& host, USHORT port, int tcpBacklog)
            : SDSServer(host, port, tcpBacklog), clientIDCounter(0)
        {
            ;
        }

        std::shared_ptr<SDSClient> OnAccept(SOCKET childSocket, const std::string& childHost) override;
    };
}
