/*
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>

using namespace boost::asio;
using namespace boost::asio::ssl;

void handle_client(ssl::stream<ip::tcp::socket> ssl_socket) {
    try {
        ssl_socket.handshake(ssl::stream_base::server);
        std::cout << "Secure connection established with client.\n";

        char data[1024];
        size_t len = ssl_socket.read_some(buffer(data));
        std::cout << "Client says: " << std::string(data, len) << "\n";

        std::string response = "Hello, Secure Client!";
        write(ssl_socket, buffer(response));

        ssl_socket.shutdown();
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

int main() {
    try {
        io_context io;
        ssl::context ctx(ssl::context::tlsv12_server);

        ctx.use_certificate_chain_file("server.crt");
        ctx.use_private_key_file("server.key", ssl::context::pem);

        ip::tcp::acceptor acceptor(io, ip::tcp::endpoint(ip::tcp::v4(), 4433));
        std::cout << "Secure TCP Server listening on port 4433...\n";

        while (true) {
            ip::tcp::socket socket(io);
            acceptor.accept(socket);
            ssl::stream<ip::tcp::socket> ssl_socket(std::move(socket), ctx);
            std::thread(handle_client, std::move(ssl_socket)).detach();
        }
    }
    catch (std::exception& e) {
        std::cerr << "Server error: " << e.what() << "\n";
    }
    return 0;
}
*/


#include "networkServer.h"
#include "./../Common/util.h"
#include <thread>

int main()
{
    try {
        Network::NetworkContext context { ConnectionSystem::SERVER };


        Network::NetworkServer server {context, 1234};
        std::cout << "Server Starting!\n";

        server.Accept();
        while (true)
        {
            //std::cout << "Waiting\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        std::cout << "Server Ending!\n";

    }
    catch (std::exception& e) {
        std::cerr << "Client error: " << e.what() << "\n";
    }
    return 0;
}

