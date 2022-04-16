#include <type_traits>

#include "NChatClient.h"
#include "NPacketProcessor.h"
#include "Packet.h"

void NChat::NChatClient::ProcessPacket(SDSBuffer& buf)
{
    /// !!! TEST !!!
    std::cout << "Dump: " << buf.Dump() << std::endl;

    while (buf.GetCount() > 0)
    {
        if (this->recvMode == RECV_MODE::HEAD)
        {
            if (buf.GetCount() < sizeof(this->recvHeader))
            {
                break;
            }
            this->recvHeader = buf.Get<decltype(this->recvHeader)>();
            buf.Delete(sizeof(this->recvHeader));
            this->recvMode = RECV_MODE::DATA;
        }
        else
        {
            if (buf.GetCount() < this->recvHeader.TotalSize - sizeof(this->recvHeader))
            {
                break;
            }
            auto id = static_cast<NCommon::PACKET_ID>(this->recvHeader.Id);
            NPacketProcessor::Process(*this, id, buf);
            buf.Delete(this->recvHeader.TotalSize - sizeof(this->recvHeader));
            this->recvMode = RECV_MODE::HEAD;
        }
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
