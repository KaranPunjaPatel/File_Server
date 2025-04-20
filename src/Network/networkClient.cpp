
#include "networkClass.h"
#include "networkClient.h"
#include "networkContext.h"
#include "networkConnection.h"

#include "securityClass.h"

#include <chrono>
#include <random>

// #include <boost/asio/ssl/impl/src.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace Network {

	NetworkClient::NetworkClient(NetworkContext& context)
		: NetworkBase(context),
		resolver(std::make_unique<NetworkResolver>(context)),
		endpoint(nullptr),
		mainSocket(nullptr),
		typeOfConnection(true)
	{ 
		mainInitialized.clear();
		Resolve("127.0.0.1", "1234");

		TCP_Connect(nullptr);
	}

	NetworkClient::~NetworkClient(){}

	void NetworkClient::TLS_Handshake(std::shared_ptr<NetworkSocket> sock)
	{
		sock->socket.async_handshake(boost::asio::ssl::stream_base::client,
			[this, sock](boost::system::error_code ec) {
				if (!ec) {
					std::cout << "TLS handshake successful\n";

					// TODO: The conn needs to now call async_receive
					if (!mainInitialized.test_and_set()) {
						std::cout << "MainSocket\n";

						mainSocket = std::make_shared<NetworkConnection>(0, sock);

						// Generate Public/Private Key pair for authentication
						mainSocket->GenerateRSAKeyPair();

						//mainSocket->PrintKeys();

						// Sending Client Public key
						Packet packet{};
						Segment keySegment = SegmentBuilder(Action::KEY)
												.withKey(mainSocket->myPublicKey->SerializeKey())
												.build();


						packet.header.type = Type::NewClient;
						packet << keySegment;

						//Send(mainSocket->GetSocket(), packet);
						SyncSend(mainSocket->GetSocket(), packet);

						Receive(mainSocket, true);
					}
					else 
					{
						std::cout << "Different Socket\n";

						uint64_t id = GetUniqueId();
						auto conn = std::make_shared<NetworkConnection>(id, sock);

						additionalSockets.insert(std::make_pair(id, conn));

						Receive(conn);
					}

				}
				else {
					std::cerr << "TLS handshake failed: " << ec.message() << "\n";
				}
			}
		);
	}

	void NetworkClient::Connect(std::shared_ptr<NetworkSocket> sock) // DONT NEED IT, I THINK
	{
		//connect(ssl_socket.next_layer(), endpoints);
	}

	void NetworkClient::Resolve(const std::string& host, const std::string& port)
	{
		if (resolver == nullptr) {
			std::cerr << "Error: Resolver not initialized\n";
			return;
		}

		boost::system::error_code ec;
		boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp> res = resolver->resolver.resolve(host, port, ec);

		if (!ec) {
			endpoint = std::make_unique<NetworkEndpoint>(res);
			std::cout << "Resolved " << host << " to endpoint.\n";
		}
		else {
			std::cerr << "Resolve failed: " << ec.message() << "\n";
		}

		/*resolver->resolver.async_resolve(host, port,
			[this, host](boost::system::error_code ec, boost::asio::ip::tcp::resolver::results_type results) {
				if (!ec) {
					endpoint = std::make_unique<NetworkEndpoint>(results);
					std::cout << "Resolved " << host << " to endpoint.\n";
				}
				else {
					std::cerr << "Resolve failed: " << ec.message() << "\n";
				}
			});*/
		
	}

	void NetworkClient::TCP_Connect(std::shared_ptr<NetworkSocket> sock = nullptr)
	{
		if (sock == nullptr)
			sock = CreateSocket();
		if (sock == nullptr) return;
		while (endpoint == nullptr) // Change this if resolve is not async
		{
			std::cout << "Waiting\n";
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		std::cout << "TCP connect\n";

		boost::asio::async_connect(sock->socket.next_layer(), endpoint->endpoint,
			[this, sock](boost::system::error_code ec, const boost::asio::ip::tcp::endpoint&) {
				if (!ec) {
					std::cout << "TCP Connection established.\n";

					TLS_Handshake(sock);
				}
				else {
					std::cerr << "Connect failed: " << ec.message() << "\n";
				}
			});
	}

	std::shared_ptr<NetworkSocket> NetworkClient::CreateSocket()
	{
		return std::make_shared<NetworkSocket>(networkContext);
	}

	void NetworkClient::SetMainSocket(std::shared_ptr<NetworkSocket> sock)
	{
		//if (sock != nullptr) mainSocket->socket->socket.shutdown();
		mainSocket = std::make_shared<NetworkConnection>(0,sock);
	}

	std::shared_ptr<NetworkSocket> NetworkClient::CreateAdditionalSocket()
	{
		return std::make_shared<NetworkSocket>(networkContext);
	}

	std::shared_ptr<NetworkSocket> NetworkClient::GetMainSocket() const
	{
		return mainSocket->socket;
	}

	uint64_t NetworkClient::GetUniqueId()
	{
		std::random_device rd;  // Seed generator
		std::mt19937_64 gen(rd()); // 64-bit Mersenne Twister PRNG
		std::uniform_int_distribution<uint64_t> dist(1, UINT64_MAX);

		uint64_t randomNum {};
		do {
			randomNum = dist(gen);
		} while (additionalSockets.find(randomNum) != additionalSockets.end());

		return randomNum;
	}


	void NetworkClient::ProcessFirstPacket(std::shared_ptr<NetworkConnection>& conn, std::shared_ptr<Packet>& packet)
	{
		if (!PacketHeaderCheck(packet->header)) return;

		std::cout << "Client First Packet\n";



		uint16_t length = packet->header.payload_len;
		std::vector<uint8_t> payload = packet->payload;

		switch (packet->header.type)
		{
		case Type::NewClient:
		{

			std::vector<Segment> segments;
			CreateSegments(segments, payload);

			for (Segment& segment : segments)
			{
				switch (segment.action)
				{
				case Action::KEY:
				{
					// Got Server Key
					conn->remotePublicKey->DeserializeKey(segment.data);
					break;
				}
				case Action::SALT:
				{
					std::array<uint8_t, 32> salt;
					std::copy_n(segment.data.begin(), 32, salt.begin());
					conn->SetSalt(salt);
					break;
				}
				case Action::SESSION_TOKEN:
				{
					token.Deserialize(segment.data);
					break;
				}
				case Action::ID:
				{

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



	void NetworkClient::ProcessPacket(std::shared_ptr<NetworkConnection>& conn, std::shared_ptr<Packet>& packet)
	{ 
		if (!PacketHeaderCheck(packet->header)) return;

		uint16_t length = packet->header.payload_len;
		std::vector<uint8_t> payload = packet->payload;

		switch (packet->header.type)
		{
		case Type::NewClient:
		{

			std::vector<Segment> segments;
			CreateSegments(segments, payload);

			for (Segment& segment : segments)
			{
				switch (segment.action)
				{
				case Action::KEY:
				{
					// Got Server Key
					conn->remotePublicKey->DeserializeKey(
						conn->myPrivateKey->DecryptBuffer(segment.data)
					);
					break;
				}
				case Action::SALT:
				{

					std::array<uint8_t, 32> salt{};
					std::vector<uint8_t> saltVec = conn->myPrivateKey->DecryptBuffer(segment.data);

					std::copy_n(saltVec.begin(), 32, salt.begin());
					conn->SetSalt(salt);
					break;
				}
				case Action::SESSION_TOKEN:
				{
					token.Deserialize(
						conn->myPrivateKey->DecryptBuffer(segment.data)
					);
					break;
				}
				case Action::ID:
				{

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


	// 0. Get Context
	// 1. Resolve Server Address and get endpoint
	// 2. Create a ssl socket with the context
	// 3. Perform tcp connect 
	// 4. Perform tls handshake TLS_Handshake()
	// 
	// WHERE. Verify the Server Certificate by getting it and then authenticate 
	//
	// TCP_Connect
	// TLS_Handshake
	// 
}
