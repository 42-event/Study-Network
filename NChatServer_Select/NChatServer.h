#pragma once

#include <SDSSelect.h>
#include <SDSServer.h>

#include "NChatClient.h"

namespace NChat
{
    class NChatServer : public SDSServer
    {
    public:
        NChatServer(const std::string& host, USHORT port, int tcpBacklog)
            : SDSServer(host, port, tcpBacklog)
        {
            ;
        }

        std::shared_ptr<SDSClient> OnAccept(SOCKET childSocket, const std::string& childHost) override;
    };
}
