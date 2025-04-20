#pragma once

#include <cstdint>
#include <memory>
#include <array>


namespace Security {
	class SecurityRSAKey;
}

namespace Network {

	class NetworkSocket;

	class NetworkConnection
	{
	public:
		uint64_t id;
		std::shared_ptr<NetworkSocket> socket;

		std::shared_ptr<Security::SecurityRSAKey> myPrivateKey;
		std::shared_ptr<Security::SecurityRSAKey> myPublicKey;

		std::shared_ptr<Security::SecurityRSAKey> remotePublicKey;

		std::array<uint8_t, 32> salt{}; // 32-byte salt for Argon2

		NetworkConnection(uint64_t id, std::shared_ptr<NetworkSocket> sock);

		void SetRemotePublicKey(std::shared_ptr<Security::SecurityRSAKey> key);

		void GenerateRSAKeyPair();

		void SetSalt(std::array<uint8_t, 32>& tempSalt);

		void GenerateSalt();

		void PrintKeys();

		std::shared_ptr<NetworkSocket> GetSocket() const;

		~NetworkConnection();
	};

}

