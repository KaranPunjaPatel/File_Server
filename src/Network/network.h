#pragma once

#include <memory>
#include <vector>

#include "networkDeque.h"
#include "./../Common/util.h"

#include "networkContext.h"

namespace Network {

	class NetworkContextImpl;
	

	class NetworkSocket;

	class NetworkConnection;

	class NetworkBase {
	public:

		NetworkBase(NetworkContext& context);
		~NetworkBase();

		virtual void TLS_Handshake(std::shared_ptr<NetworkSocket> sock) = 0;

		void Receive(std::shared_ptr<NetworkConnection> conn, bool first = false);

		void Send(std::shared_ptr<NetworkSocket> sock, const Packet& packet);

		virtual uint64_t GetUniqueId() = 0;

		virtual void ProcessFirstPacket(std::shared_ptr<NetworkConnection>& conn, std::shared_ptr<Packet>& packet) = 0;

		virtual void ProcessPacket(std::shared_ptr<NetworkConnection>& conn, std::shared_ptr<Packet>& packet) = 0;

		bool PacketHeaderCheck(PacketHeader header);

		void SyncSend(std::shared_ptr<NetworkSocket> sock, const Packet& packet);

	protected:
		NetworkContext& networkContext;

		PacketDeque networkDeque; // From "networkDeque.h"

	private:
		
	};
}