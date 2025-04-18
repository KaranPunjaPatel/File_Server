#pragma once
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>


#include <iostream>
#include <memory>

#include "./../Common/util.h"
#include "networkContext.h"

#include "asymmetricKey.h"
//#include "securityClass.h"

namespace Network {

	class NetworkContextImpl {
	public:
		std::unique_ptr<boost::asio::io_context>   io;
		std::unique_ptr<boost::asio::ssl::context> ctx;

		//std::thread threadContext;
		std::vector<std::thread> threadPool;
		boost::asio::executor_work_guard<boost::asio::io_context::executor_type> workGuard;  // Keep io_context running

		NetworkContextImpl(ConnectionSystem system)
			: io(std::make_unique<boost::asio::io_context>()),
			workGuard(boost::asio::make_work_guard(*io))
		{
			if (system == ConnectionSystem::CLIENT)
			{
				ctx = std::make_unique<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv13_client);

				ctx->load_verify_file("server.crt");
			}
			else if (system == ConnectionSystem::SERVER)
			{
				ctx = std::make_unique<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv13_server);

				ctx->use_certificate_chain_file("server.crt");
				ctx->use_private_key_file("server.key", boost::asio::ssl::context::pem);
			}
			else
			{
				std::cerr << "Error: System Connection not supported\n";
			}

			/* 
			Allow multiple threads to process I/O operations by using a thread pool.
			This allows multiple threads to handle incoming connections.
			*/
			for (size_t i = 0; i < std::thread::hardware_concurrency(); ++i) {
				threadPool.emplace_back([this]() { io->run(); });
			}

			//threadContext = std::thread([this]() { io->run(); });
			
		}

		~NetworkContextImpl() {
			io->stop();
			for (auto& thread : threadPool) {
				if (thread.joinable()) thread.join();
			}

			//if (threadContext.joinable()) threadContext.join();
		}
	};

	class NetworkSocket : public std::enable_shared_from_this<NetworkSocket> {
	public:
		boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket;
		//std::deque<Packet> sendQueue;
		//std::mutex sendMutex;

		NetworkSocket(NetworkContext& context)
			: socket(*(context.impl->io), *(context.impl->ctx)) {
		}

		NetworkSocket(boost::asio::ip::tcp::socket&& t_socket, NetworkContext& context)
			: socket(std::move(t_socket), *(context.impl->ctx)) {
		}

		//void Send(const Packet& packet) {
		//	bool sending = false;
		//	{
		//		std::lock_guard<std::mutex> lock(sendMutex);
		//		sending = !sendQueue.empty(); // If queue was empty, we will start sending
		//		sendQueue.push_back(packet);
		//	}

		//	if (!sending) {
		//		WriteNext();
		//	}
		//}
		~NetworkSocket() {}

	private:
		//void WriteNext() {
		//	std::lock_guard<std::mutex> lock(sendMutex);
		//	if (sendQueue.empty()) return; // Nothing to send

		//	auto self = shared_from_this();

		//	Packet& packet = sendQueue.front();
		//	std::vector<boost::asio::const_buffer> buffers;

		//	buffers.push_back(boost::asio::buffer(&packet.header, sizeof(PacketHeader)));

		//	if (!packet.payload.empty()) {
		//		buffers.push_back(boost::asio::buffer(packet.payload.data(), packet.payload.size()));
		//	}

		//	boost::asio::async_write(socket, buffers,
		//		[this, self](boost::system::error_code ec, std::size_t) {
		//			if (!ec) {
		//				std::lock_guard<std::mutex> lock(sendMutex);
		//				sendQueue.pop_front();
		//				if (!sendQueue.empty()) {
		//					WriteNext(); // Send next packet in queue
		//				}
		//			}
		//			else {
		//				std::cerr << "Send failed: " << ec.message() << "\n";
		//			}
		//		}
		//	);
		//}

	};

	class NetworkAcceptor {
	public:
		boost::asio::ip::tcp::acceptor acceptor;

		NetworkAcceptor(NetworkContext& context, const boost::asio::ip::port_type port)
			: acceptor(*(context.impl->io), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
		{ }

		~NetworkAcceptor() {}
	};

	class NetworkResolver {
	public: 
		boost::asio::ip::tcp::resolver resolver;

		NetworkResolver(NetworkContext& context)
			: resolver(*(context.impl->io))
		{ }

		~NetworkResolver() {}

	};

	class NetworkEndpoint {
	public:
		boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp> endpoint;

		NetworkEndpoint() {}

		NetworkEndpoint(boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp> t_endpoint)
			: endpoint(t_endpoint)
		{ }
		~NetworkEndpoint() {}

	};

}