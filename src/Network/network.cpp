
#include "network.h" 
#include "networkClass.h"
#include "networkConnection.h"

#include <chrono>


namespace Network {

	NetworkBase::NetworkBase(NetworkContext& context) 
		: networkContext(context)
	{
		
	}

    NetworkBase::~NetworkBase()
    {

    }


	void NetworkBase::Receive(std::shared_ptr<NetworkConnection> conn, bool first)
	{
        auto packet = std::make_shared<Packet>();
        auto sock = conn->socket;
        std::cout << "Receiving here\n";

        auto buffers = boost::asio::buffer(&packet->header, sizeof(PacketHeader));

		boost::system::error_code ec;
        sock->socket.async_read_some(buffers, 
            [this, &conn, &sock, &packet, first](boost::system::error_code ec, std::size_t bytes_transferred) {
                std::cout << "HERE ERR\n";

                if (!ec) {
                    std::cout << "Header received.\n";
                    std::cout << (*packet);

                    // Step 2: Read the payload if there is any
                    if (packet->header.payload_len > 0) {
                        packet->payload.resize(packet->header.payload_len);

                        boost::asio::async_read(sock->socket,
                            boost::asio::buffer(packet->payload.data(), packet->payload.size()),
                            [this, conn, sock, packet](boost::system::error_code ec, std::size_t bytes_transferred) {
                                if (!ec) {
                                    std::cout << "Payload received: " << bytes_transferred << " bytes.\n";
                                    // Now the packet is fully received and can be processed

                                    std::vector<uint8_t> rawData(sizeof(PacketHeader) + packet->payload.size());



                                    std::memcpy(rawData.data(), &packet->header, sizeof(PacketHeader));
                                    std::memcpy(rawData.data() + sizeof(PacketHeader), packet->payload.data(), packet->payload.size());

                                    for (uint8_t byte : rawData) {
                                        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
                                    }
                                    std::cout << std::dec << "\n";  // Reset formatting

                                    // Create InternalPacket
                                    InternalPacket internalPacket{
                                        conn->id,
                                        static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count()), // Timestamp
                                        std::move(rawData)
                                    };

                                    // Push to queue
                                    networkDeque.push_back(std::move(internalPacket));


                                }
                                else {
                                    std::cerr << "Payload receive failed: " << ec.message() << "\n";
                                }
                            }
                        );
                    }

                    // TODO: Create a process packet for client and server
                    if (first) {
                        ProcessFirstPacket(conn, packet);
                    }
                    else {
                        ProcessPacket(conn, packet);
                    }

                    Receive(conn);

                }
                else {
                    std::cerr << "Header receive failed: " << ec.message() << "\n";
                    return;
                }
            });

        /*
        // Step 1: Read the header
        boost::asio::async_read(sock->socket,
            boost::asio::buffer(&packet->header, sizeof(PacketHeader)),
            [this, &conn, &sock, &packet, first](boost::system::error_code ec, std::size_t bytes_transferred) {
                std::cout << "HERE ERR\n";

                if (!ec) {
                    std::cout << "Header received.\n";
                    std::cout << (*packet);

                    // Step 2: Read the payload if there is any
                    if (packet->header.payload_len > 0) {
                        packet->payload.resize(packet->header.payload_len);

                        boost::asio::async_read(sock->socket,
                            boost::asio::buffer(packet->payload.data(), packet->payload.size()),
                            [this, conn, sock, packet](boost::system::error_code ec, std::size_t bytes_transferred) {
                                if (!ec) {
                                    std::cout << "Payload received: " << bytes_transferred << " bytes.\n";
                                    // Now the packet is fully received and can be processed

                                    std::vector<uint8_t> rawData(sizeof(PacketHeader) + packet->payload.size());

                                    

                                    std::memcpy(rawData.data(), &packet->header, sizeof(PacketHeader));
                                    std::memcpy(rawData.data() + sizeof(PacketHeader), packet->payload.data(), packet->payload.size());

                                    for (uint8_t byte : rawData) {
                                        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
                                    }
                                    std::cout << std::dec << "\n";  // Reset formatting

                                    // Create InternalPacket
                                    InternalPacket internalPacket{
                                        conn->id,
                                        static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count()), // Timestamp
                                        std::move(rawData)
                                    };

                                    // Push to queue
                                    networkDeque.push_back(std::move(internalPacket));


                                }
                                else {
                                    std::cerr << "Payload receive failed: " << ec.message() << "\n";
                                }
                            }
                        );
                    }

                    // TODO: Create a process packet for client and server
                    if (first) {
                        ProcessFirstPacket(conn, packet);
                    }
                    else {
                        ProcessPacket(conn, packet);
                    }

                    Receive(conn);

                }
                else {
                    std::cerr << "Header receive failed: " << ec.message() << "\n";
                    return;
                }

            }
        );
                */
	}

	void NetworkBase::Send(std::shared_ptr<NetworkSocket> sock, const Packet& packet)
	{
        std::vector<boost::asio::const_buffer> buffers;

        // Send the header first
        buffers.push_back(boost::asio::buffer(&packet.header, sizeof(PacketHeader)));
        //std::cout << "Sending1\n";
        std::cout << packet;

        // Send the payload (only if it's not empty)
        if (!packet.payload.empty()) {
            buffers.push_back(boost::asio::buffer(packet.payload.data(), packet.payload.size()));
        }
        std::cout << "Sending2\n";
        boost::asio::async_write(sock->socket, buffers,
            [sock](boost::system::error_code ec, std::size_t bytes_transferred) {
                if (!ec) {
                    std::cout << "Packet sent successfully.\n";
                }
                else {
                    std::cerr << "Send failed: " << ec.message() << "\n";
                }
            }
        );
	}

    void NetworkBase::SyncSend(std::shared_ptr<NetworkSocket> sock, const Packet& packet)
    {
        std::vector<boost::asio::const_buffer> buffers;

        // Send the header first
        buffers.push_back(boost::asio::buffer(&packet.header, sizeof(PacketHeader)));
        //std::cout << "Sending1\n";
        std::cout << packet;

        // Send the payload (only if it's not empty)
        if (!packet.payload.empty()) {
            buffers.push_back(boost::asio::buffer(packet.payload.data(), packet.payload.size()));
        }
        boost::system::error_code ec;
        sock->socket.write_some(buffers, ec);

        if (!ec) {
            std::cout << "Packet sent successfully.\n";
        }
        else {
            std::cerr << "Send failed: " << ec.message() << "\n";
        }
    }

    bool NetworkBase::PacketHeaderCheck(PacketHeader header)
    {
        // Basic Header Checks
        if (header.magic_bytes != 123456 || header.version != 1)
            return false;
        for (int i = 0; i < 8; i++)
        {
            if (header.reserved[i] != 0) return false;
        }

        return true;
    }

}