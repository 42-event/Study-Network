#pragma once

#include <SDSSelect.h>
#include <SDSClient.h>

#include "Packet.h"

namespace NChat
{
    template <typename T>
    using packet_type_t = typename std::enable_if_t<std::is_convertible_v<T, NCommon::PktBase>, bool>;

    class NChatClient : public SDSClient
    {
    public:
        enum class RECV_MODE
        {
            HEAD,
            DATA
        };

        enum class STATE
        {
            LOGIN,
            LOBBY,
            ROOM
        };

    private:
        long clientID;
        RECV_MODE recvMode;
        NCommon::PktHeader recvHeader;
        std::atomic<STATE> state;

    public:
        inline NChatClient(SOCKET childSocket, const std::string& childHost, long clientID)
            : SDSClient(childSocket, childHost), clientID(clientID), recvMode(RECV_MODE::HEAD), recvHeader(), state(STATE::LOGIN)
        {
            ;
        }

        inline long GetClientID() const
        {
            return this->clientID;
        }

        inline STATE GetState() const
        {
            return this->state;
        }

        inline STATE SetState(STATE state)
        {
            return this->state = state;
        }

        void ProcessPacket(SDSBuffer& buf) override;

        template <typename T, typename packet_type_t<T> = true>
        void SendPacket(NCommon::PACKET_ID id, const T& packet)
        {
            synchronized(this->writeLock)
            {
                NCommon::PktHeader header{};
                header.Id = static_cast<decltype(header.Id)>(id);
                header.Reserve = 0;
                header.TotalSize = sizeof(header) + sizeof(T);
                SDSClient::SendPacket(header);
                SDSClient::SendPacket(reinterpret_cast<const char*>(&packet), sizeof(T));
            }
        }

        void OnError(int errorNumber) override;
    };
}
