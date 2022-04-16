#include "NChatClient.h"

void NChat::NChatClient::ProcessPacket(SDSBuffer& buf)
{
    /// !!! TEST !!!
    std::cout << "Dump: " << buf.Dump() << std::endl;

    /// !!! TEST !!! echo
    while (buf.GetCount() > 0)
    {
        SendPacket(buf.Get<std::byte>());
        buf.Delete(sizeof(std::byte));
    }
}

void NChat::NChatClient::OnError(int errorNumber)
{
    if (errorNumber == WSABASEERR)
    {
        /// !!! TEST !!!
        std::cout << "Gracefully Shutdown" << std::endl;
    }
    else if (errorNumber == WSAECONNRESET)
    {
        /// !!! TEST !!!
        std::cout << "Connection Reset" << std::endl;
    }
    SDSClient::OnError(errorNumber);
}
