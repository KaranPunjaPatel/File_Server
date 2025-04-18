#pragma once

#include "network.h"
#include "sessionToken.h"

#include <string>
#include <unordered_map>
#include <thread>
#include <atomic>

namespace Network {

	class NetworkResolver;

	class NetworkEndpoint;

	class NetworkConnection;

	class NetworkClient : public NetworkBase
	{
	public:
		NetworkClient(NetworkContext& context);
		~NetworkClient();

		virtual void TLS_Handshake(std::shared_ptr<NetworkSocket> sock);

		void Connect(std::shared_ptr<NetworkSocket> sock);

		void Resolve(const std::string& host, const std::string& port);
		
		void TCP_Connect(std::shared_ptr<NetworkSocket> sock);
		
		std::shared_ptr<NetworkSocket> CreateSocket();
		
		void SetMainSocket(std::shared_ptr<NetworkSocket> sock);

		std::shared_ptr<NetworkSocket> CreateAdditionalSocket();
		std::shared_ptr<NetworkSocket> GetMainSocket() const;

		virtual uint64_t GetUniqueId();

		virtual void ProcessFirstPacket(std::shared_ptr<NetworkConnection>& conn, std::shared_ptr<Packet>& packet);

		virtual void ProcessPacket(std::shared_ptr<NetworkConnection>& conn, std::shared_ptr<Packet>& packet);

	private:
		std::unique_ptr<NetworkResolver> resolver;
		std::unique_ptr<NetworkEndpoint> endpoint;

		std::shared_ptr<NetworkConnection> mainSocket;

		std::unordered_map<uint64_t, std::shared_ptr<NetworkConnection>> additionalSockets;


		bool typeOfConnection; // 0 - Public, 1 - Private
		std::atomic_flag mainInitialized;

		Security::SessionToken token;

		//uint64_t server_id;
	};

}
