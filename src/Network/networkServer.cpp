
#include "networkClass.h"
#include "networkServer.h"
#include "networkContext.h"
#include "networkConnection.h"

#include "securityClass.h"
#include "asymmetricKey.h"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <random>

constexpr uint64_t SESSION_TOKEN_LIFETIME = 3600;

namespace Network {


	NetworkServer::NetworkServer(NetworkContext& context, uint16_t port) 
		: NetworkBase(context), 
		acceptor(std::make_unique<NetworkAcceptor>(context, port))
	{

	}

	NetworkServer::~NetworkServer()
	{

	}

	void NetworkServer::TLS_Handshake(std::shared_ptr<NetworkSocket> sock)
	{
		// TODO: First need to check if connected or not theen perform handshake

		sock->socket.async_handshake(boost::asio::ssl::stream_base::server, 
			[this,sock](boost::system::error_code ec) {
				if (!ec) {
					std::cout << "SSL handshake successful (Server)\n";
					std::shared_ptr<NetworkConnection> conn = std::make_shared<NetworkConnection>(0, sock);
					std::cout << "Here\n";

					Receive(conn, true);
				}
				else {
					std::cerr << "Handshake failed: " << ec.message() << "\n";
				}
			}
		);
	}

	void NetworkServer::Accept()
	{
		if (acceptor == nullptr) {

			std::cout << "NO ACCEPTOR\n";
			return;
		}
		std::cout << "Listening\n";

		acceptor->acceptor.async_accept(
			[this](const boost::system::error_code& ec, boost::asio::ip::tcp::socket socket)
			{
				if (!ec)
				{
					std::cout << "[Server] New Connection: " << socket.remote_endpoint() << "\n";

					TLS_Handshake(std::make_shared<NetworkSocket>(std::move(socket), networkContext));
				}
				else
				{
					std::cout << "[Server] New Connection Error: " << ec.message() << "\n";
				}

				Accept();
			}
		);
	}

	uint64_t NetworkServer::GetUniqueId()
	{
		std::random_device rd;  // Seed generator
		std::mt19937_64 gen(rd()); // 64-bit Mersenne Twister PRNG
		std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);

		uint64_t randomNum{};
		do {
			randomNum = dist(gen);
		} while (clients.find(randomNum) != clients.end());

		return randomNum;
	}

	void NetworkServer::ProcessFirstPacket(std::shared_ptr<NetworkConnection>& conn, std::shared_ptr<Packet>& packet)
	{
		if (!PacketHeaderCheck(packet->header)) return;

		std::cout << "Server First Packet\n";

		uint16_t length = packet->header.payload_len;
		std::vector<uint8_t> payload = packet->payload;

		switch (packet->header.type)
		{
		case Type::NewClient:
		{
			

			std::vector<Segment> segments;
			CreateSegments(segments, payload);

			bool newClient{false};

			for (Segment& segment : segments)
			{
				switch (segment.action)
				{
				case Action::KEY:
				{
					newClient = true;

					// Got Client Key
					conn->remotePublicKey->DeserializeKey(segment.data);

					// Create a client session
					uint64_t id = GetUniqueId();

					ClientSession session{};
					session.clientId	= id;
					session.mainSocket	= conn;
					session.token		= Security::SessionToken::Generate(id, SESSION_TOKEN_LIFETIME);

					clients.insert(std::make_pair(id, session));
					tokenToClientId.insert(std::make_pair(session.token, id));

					// TODO: Send 
					Packet packet{};

					std::vector<uint8_t> encryptedKey = conn->remotePublicKey->Encrypt(conn->myPublicKey->SerializeKey());
					std::vector<uint8_t> encryptedSalt = conn->remotePublicKey->Encrypt(conn->salt);
					std::vector<uint8_t> encryptedToken = conn->remotePublicKey->Encrypt(session.token.Serialize());
					std::vector<uint8_t> encryptedId = conn->remotePublicKey->Encrypt(id);

					Segment keySegment = SegmentBuilder(Action::KEY)
											.withKey(encryptedKey)
											.build();

					Segment saltSegment = SegmentBuilder(Action::SALT)
											.withSalt(encryptedSalt)
											.build();
					
					Segment tokenSegment = SegmentBuilder(Action::SESSION_TOKEN)
											.withSession_Token(encryptedToken)
											.build();

					Segment idSegment = SegmentBuilder(Action::ID)
											.withId(encryptedId)
											.build();

					packet.header.type = Type::NewClient;
					packet << keySegment << saltSegment << tokenSegment;

					Send(conn->GetSocket(), packet);
					break;
				}

				}
			}
			
			break;
		}
		case Type::NewSocket:
		{

			break;
		}
		default:
			break;
		}


	}


	void NetworkServer::ProcessPacket(std::shared_ptr<NetworkConnection>& conn, std::shared_ptr<Packet>& packet)
	{

	}
	
}

// 0. Get Context
	// 1. Context use certificate and key
	// 2. Acceptor needs to listen on port
	// 3. Create a socket and pass to acceptor the new connection will become that socket and connection established
	// 4. Pass the tcp socket to create a ssl socket
	// 5. Perform tls handshake TLS_Handshake()

