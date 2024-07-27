#ifndef H_CLIENT
#define H_CLIENT_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <condition_variable>
#include <cstring>
#include <iostream>
#include <sstream>
#include <thread>

class client {
private:
    std::string buf_;
    int client_socket_;
    bool is_connecting_ = false;
    sockaddr_in server_address_;

    bool check_str(std::string& str);
    std::string modify(std::string str);
    void first_thread(std::mutex& mtx, std::condition_variable& cond_var, std::string& buf);
    void second_thread(std::mutex& mtx, std::condition_variable& cond_var, std::string& buf);

    void connect_to_server();
    void reconnect_to_server();

public:
    client() = default;
    client(const client&) = default;
    client(client&&) = default;

    void run(ushort port = 1111);
    void setup_client(ushort port);
};

#endif  // H_CLIENT
