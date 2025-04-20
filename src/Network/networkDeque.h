#pragma once
#ifndef PACKET_DEQUE_H
#define PACKET_DEQUE_H

#include <deque>
#include <mutex>
#include <iostream>
#include <condition_variable>

#include "packet.h"

namespace Network {

    class PacketDeque {
    public:
        PacketDeque() = default;
        PacketDeque(const PacketDeque&) = delete;

        ~PacketDeque() { clear(); }

        const InternalPacket& front();
        
        const InternalPacket& back();

        InternalPacket pop_front();

        InternalPacket pop_back();

        void push_back(const InternalPacket&& item);

        void push_back(const std::shared_ptr<InternalPacket> item);

        void push_front(const InternalPacket&& item);

        bool empty();

        size_t count();

        void clear();

        void wait();

    protected:
        std::mutex muxQueue;
        std::deque<InternalPacket> deqQueue;
        std::condition_variable cvBlocking;
    };

}


#endif // PACKET_DEQUE_H