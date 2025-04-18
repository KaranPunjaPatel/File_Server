#include "networkDeque.h"

namespace Network {

	const InternalPacket& PacketDeque::front()
	{
		std::lock_guard<std::mutex> lg(muxQueue);
		return deqQueue.front();
	}

	const InternalPacket& PacketDeque::back() {
		std::lock_guard<std::mutex> lg(muxQueue);
		return deqQueue.back();
	}

    InternalPacket PacketDeque::pop_front() {
        std::lock_guard<std::mutex> lg(muxQueue);
        InternalPacket item = std::move(deqQueue.front());
        deqQueue.pop_front();
        return item;
    }

    InternalPacket PacketDeque::pop_back() {
        std::lock_guard<std::mutex> lg(muxQueue);
        InternalPacket item = std::move(deqQueue.back());
        deqQueue.pop_back();
        return item;
    }

    void PacketDeque::push_back(const InternalPacket&& item) {
        {
            //std::cout << "Insert: " << item << "\n";
            std::lock_guard<std::mutex> lg(muxQueue);
            deqQueue.push_back(item);
        }
        cvBlocking.notify_one();
    }

    void PacketDeque::push_back(const std::shared_ptr<InternalPacket> item)
    {
        if (item == nullptr) return;
        {
            //std::cout << "Insert: " << item << "\n";
            std::lock_guard<std::mutex> lg(muxQueue);
            deqQueue.push_back(*item);
        }
        cvBlocking.notify_one();
    }


    void PacketDeque::push_front(const InternalPacket&& item) {
        {
            std::lock_guard<std::mutex> lg(muxQueue);
            deqQueue.push_front(item);
        }
        cvBlocking.notify_one();
    }

    bool PacketDeque::empty() {
        std::lock_guard<std::mutex> lg(muxQueue);
        return deqQueue.empty();
    }

    size_t PacketDeque::count() {
        std::lock_guard<std::mutex> lg(muxQueue);
        return deqQueue.size();
    }

    void PacketDeque::clear() {
        std::lock_guard<std::mutex> lg(muxQueue);
        deqQueue.clear();
    }

    void PacketDeque::wait() {
        std::unique_lock<std::mutex> ul(muxQueue);
        cvBlocking.wait(ul, [this] { return !deqQueue.empty(); });
    }

}

