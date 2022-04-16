#include "NChatServer.h"

std::shared_ptr<SDSClient> NChat::NChatServer::OnAccept(SOCKET childSocket, const std::string& childHost)
{
    long id = ++this->clientIDCounter;
    auto client = std::make_shared<NChatClient>(childSocket, childHost, id);

    /// !!! TEST !!!
    std::cout << "OnAccept: Host=" << childHost.c_str() << "; ID=" << client->GetClientID() << std::endl;

    return client;
}
