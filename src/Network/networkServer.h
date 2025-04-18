#pragma once

#include "network.h"
#include "sessionToken.h"

#include <unordered_map>


namespace Network {

	class NetworkAcceptor;

	class Connection;

	class NetworkConnection;

	struct ClientSession {
		uint64_t clientId;
		std::shared_ptr<NetworkConnection> mainSocket;  // Auth socket
		std::vector<std::shared_ptr<NetworkConnection>> extraSockets;  // Additional connections
		Security::SessionToken token;
	};

	class NetworkServer : public NetworkBase
	{
	public:
		NetworkServer(NetworkContext& context, uint16_t port);

		~NetworkServer();

		virtual void TLS_Handshake(std::shared_ptr<NetworkSocket> sock);

		void Accept();

		virtual uint64_t GetUniqueId();

		virtual void ProcessFirstPacket(std::shared_ptr<NetworkConnection>& conn, std::shared_ptr<Packet>& packet);

		virtual void ProcessPacket(std::shared_ptr<NetworkConnection>& conn, std::shared_ptr<Packet>& packet);

		//void AddClientConnection(uint64_t clientId, std::shared_ptr<NetworkConnection> conn, bool isMain = false);
		//void RemoveClientConnection(uint64_t clientId, std::shared_ptr<NetworkConnection> conn);

	private:
		std::unique_ptr<NetworkAcceptor> acceptor;

		std::unordered_map<uint64_t, ClientSession> clients;

		// Additional mapping for quick lookup of sessions via token.
		std::unordered_map<Security::SessionToken, uint64_t> tokenToClientId;

		//std::vector<std::shared_ptr<NetworkConnection>> clients;

		//std::unordered_map<uint64_t, std::shared_ptr<NetworkConnection>> activeConnections;
	};

}


