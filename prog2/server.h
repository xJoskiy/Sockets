#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <thread>

class server {
private:
    sockaddr_in server_address_;
    int server_socket_;

    void handle_client(int client_socket);
    void accept_connections();
    void setup_server(short port);

public:
    server() = default;
    server(server&&) = default;
    server(const server&) = default;

    void run(ushort port = 1111);
};