#include "server.h"

void server::handle_client(int client_socket) {
    char buffer[8] = {0};
    while (recv(client_socket, buffer, sizeof(buffer), 0) > 0) {
        std::string msg(buffer);
        try {
            int val = std::stoi(msg);
            if (val % 32 == 0 and msg.size() > 2)
                std::cout << "Data error" << std::endl;
            else
                std::cout << "Data: " << buffer << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Data error" << std::endl;
        }
    }

    std::cout << "Client has disconnected" << std::endl;
}

void server::accept_connections() {
    std::cout << "Waiting for connections" << std::endl;
    while (true) {
        int client_socket = accept(server_socket_, nullptr, nullptr);
        if (client_socket != -1) {
            std::thread t(&server::handle_client, *this, client_socket);
            std::cout << "New connection, waiting for data" << std::endl;
            t.detach();
        };
    }
}

void server::run(ushort port /* = 1111 */) {
    setup_server(port);
    accept_connections();
    close(server_socket_);
}

void server::setup_server(short port) {
    while ((server_socket_ = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cout << "Error creating server socket, retrying..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    server_address_.sin_family = AF_INET;          // IPv4 protocol
    server_address_.sin_port = htons(port);        // Run on a given port
    server_address_.sin_addr.s_addr = INADDR_ANY;  // Accepting any IP

    while (bind(server_socket_, reinterpret_cast<sockaddr*>(&server_address_),
                sizeof(server_address_)) == -1) {
        std::cout << "Error binding server socket, retrying..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    while (listen(server_socket_, 1) == -1) {
        std::cout << "Error listening socket, retrying..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}