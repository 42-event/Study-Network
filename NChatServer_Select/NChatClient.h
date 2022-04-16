#pragma once

#include <SDSSelect.h>
#include <SDSClient.h>
#include <type_traits>

#include "Packet.h"

namespace NChat
{
    template <typename T>
    using packet_type_t = typename std::enable_if_t<std::is_convertible_v<T, NCommon::PktBase>, bool>;

    class NChatClient : public SDSClient
    {
    public:
        typedef int64_t _ClientIDType;

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
        _ClientIDType clientID;
        RECV_MODE recvMode;
        NCommon::PktHeader recvHeader;
        std::atomic<STATE> state;
        std::string name;

    public:
        inline NChatClient(SOCKET childSocket, const std::string& childHost, _ClientIDType clientID)
            : SDSClient(childSocket, childHost), clientID(clientID), recvMode(RECV_MODE::HEAD), recvHeader(), state(STATE::LOGIN), name("(null)")
        {
            ;
        }

        inline _ClientIDType GetClientID() const
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

        inline const std::string& GetName() const
        {
            return this->name;
        }

        inline void SetName(const std::string& name)
        {
            this->name = name;
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
                SDSClient::SendPacket(std::bit_cast<const char*>(&packet), sizeof(T));
            }
        }

        inline void SendPacket(NCommon::PACKET_ID id, const SDSBuffer& buffer)
        {
            synchronized(this->writeLock)
            {
                NCommon::PktHeader header{};
                header.Id = static_cast<decltype(header.Id)>(id);
                header.Reserve = 0;
                auto count = static_cast<decltype(header.TotalSize)>(buffer.GetCount());
                header.TotalSize = sizeof(header) + count;
                SDSClient::SendPacket(header);
                SDSClient::SendPacket(buffer.RawGet(), count);
            }
        }

        void OnError(int errorNumber) override;
    };
}
