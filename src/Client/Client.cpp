
/*
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>


using namespace boost::asio;
using namespace boost::asio::ssl;

int main() {
    try {
        io_context io;
        ssl::context ctx(ssl::context::tlsv12_client);

        ctx.load_verify_file("server.crt");

        ip::tcp::resolver resolver(io);
        auto endpoints = resolver.resolve("127.0.0.1", "4433");

        ssl::stream<ip::tcp::socket> ssl_socket(io, ctx);

        connect(ssl_socket.next_layer(), endpoints);
        ssl_socket.handshake(ssl::stream_base::client);

        std::cout << "Secure connection established with server.\n";

        std::string message = "Hello, Secure Server!";
        write(ssl_socket, buffer(message));

        char data[1024];
        size_t len = ssl_socket.read_some(buffer(data));
        std::cout << "Server says: " << std::string(data, len) << "\n";

        ssl_socket.shutdown();
    }
    catch (std::exception& e) {
        std::cerr << "Client error: " << e.what() << "\n";
    }
    return 0;
}
*/

#include "networkClient.h"
#include "./../Common/util.h"

#include <chrono>
#include <thread>

int main() 
{
    try {
        Network::NetworkContext context{ ConnectionSystem::CLIENT };

        Network::NetworkClient client { context };

        //client.Resolve("127.0.0.1", "1234");
        //client.SetMainSocket(client.CreateSocket());

        //std::this_thread::sleep_for(std::chrono::seconds(2));
        //std::cout << "Client Starting!\n";

        //client.TCP_Connect(nullptr);
        

        std::this_thread::sleep_for(std::chrono::seconds(120)); 
        std::cout << "Done!\n";

    }
    catch (std::exception& e) {
        std::cerr << "Client error: " << e.what() << "\n";
    }
    return 0;
}
