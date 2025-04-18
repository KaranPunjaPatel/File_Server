#include "networkConnection.h"
#include "asymmetricKey.h"
#include "securityClass.h"

namespace Network {

	NetworkConnection::NetworkConnection(uint64_t id, std::shared_ptr<NetworkSocket> sock)
		: id(id), socket(std::move(sock)), salt{}
	{

	}

	void NetworkConnection::SetRemotePublicKey(std::shared_ptr<Security::SecurityRSAKey> key)
	{
		remotePublicKey = key;
	}

	void NetworkConnection::GenerateRSAKeyPair() {
		Security::GenerateRSAKeyPair(myPublicKey, myPrivateKey);

	}

	void NetworkConnection::SetSalt(std::array<uint8_t, 32>& tempSalt)
	{
		salt = tempSalt;
	}

	void NetworkConnection::GenerateSalt()
	{
		Security::GenerateSalt(salt);
	}

	void NetworkConnection::PrintKeys()
	{
		myPublicKey->Print();
		myPrivateKey->Print();
		remotePublicKey->Print();
	}

	std::shared_ptr<NetworkSocket> NetworkConnection::GetSocket() const
	{ 
		if (!socket) { 
			std::cerr << "Socket Not Initialised\n"; 
			return nullptr;
		}
		return socket;
	}


	NetworkConnection::~NetworkConnection() {}

}

