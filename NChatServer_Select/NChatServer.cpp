#include "NChatServer.h"

std::shared_ptr<SDSClient> NChat::NChatServer::OnAccept(SOCKET childSocket, const std::string& childHost)
{
    /// !!! TEST !!!
    std::cout << "OnAccept: " << childHost.c_str() << std::endl;

    return std::make_shared<NChatClient>(childSocket, childHost);
}
