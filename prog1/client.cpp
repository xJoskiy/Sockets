#include "client.h"

void client::setup_client(ushort port) {
    client_socket_ = socket(AF_INET, SOCK_STREAM, 0);

    server_address_.sin_family = AF_INET;          // IPv4 protocol
    server_address_.sin_port = htons(port);        // Run on a given port
    server_address_.sin_addr.s_addr = INADDR_ANY;  // Accepting any IP
}

void client::reconnect_to_server() {
    close(client_socket_);
    client_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    connect_to_server();
}

void client::connect_to_server() {
    if (is_connecting_) return;
    is_connecting_ = true;
    while (connect(client_socket_, reinterpret_cast<sockaddr*>(&server_address_),
                   sizeof(server_address_)) == -1) {
        std::cout << "Error connecting to the server, retrying..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
    std::cout << "Connection established" << std::endl;
    is_connecting_ = false;
}

std::string client::modify(std::string str) {
    std::string res;
    std::sort(str.begin(), str.end(), [](char l, char r) { return l > r; });
    for (char c : str) {
        if (c % 2) {
            res += c;
        } else {
            res += "KB";
        }
    }

    return res;
}

bool client::check_str(std::string& str) {
    if (str.size() > 64) return 0;
    for (auto c : str) {
        if (!isdigit(c)) return 0;
    }

    return 1;
}

void client::first_thread(std::mutex& mtx, std::condition_variable& cond_var, std::string& buf) {
    std::string tmp, res;
    std::cout << "Enter a string" << std::endl;
    while (true) {
        std::cin >> tmp;
        if (check_str(tmp)) {
            std::string res = modify(tmp);  // NRVO
            std::lock_guard<std::mutex> lk(mtx);
            buf = std::move(res);
            cond_var.notify_one();
        } else
            std::cout << "Input error, try again" << std::endl;
    }
}

void client::second_thread(std::mutex& mtx, std::condition_variable& cond_var, std::string& buf) {
    while (true) {
        std::unique_lock<std::mutex> lk(mtx);
        cond_var.wait(lk, [&buf]() { return !buf.empty(); });
        std::cout << "Modified: " << buf << '\n';

        size_t sum = 0;
        std::string tmp = std::move(buf);

        for (char x : tmp) {
            if (std::isdigit(x)) sum += static_cast<int>(x) - 48;
        }

        std::cout << "Sum: " << sum << '\n';
        std::string msg = std::to_string(sum);
        if (send(client_socket_, msg.c_str(), msg.size() + 1, MSG_NOSIGNAL) == -1 and
            not is_connecting_) {
            std::cout << "Error sending a message, reconnecting" << std::endl;
            std::thread t(&client::reconnect_to_server, std::ref(*this));
            t.detach();
        }
    }
}

void client::run(ushort port /* = 1111 */) {
    setup_client(port);
    std::thread con_thr(&client::connect_to_server, std::ref(*this));

    std::mutex mtx;
    std::condition_variable cond_var;
    std::thread first_thr(&client::first_thread, std::ref(*this), std::ref(mtx), std::ref(cond_var),
                          std::ref(buf_));
    std::thread second_thr(&client::second_thread, std::ref(*this), std::ref(mtx),
                           std::ref(cond_var), std::ref(buf_));
    con_thr.join();
    first_thr.join();
    second_thr.join();

    close(client_socket_);
}