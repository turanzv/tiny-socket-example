#include <iostream>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 9000

bool running = true;
std::vector<int> client_fds;

void cleanup_connections() {
    for (int client_fd : client_fds) {
        std::cout << "[Server] Closing client connection: " << client_fd << std::endl;
        shutdown(client_fd, SHUT_RDWR);
        close(client_fd);
    }
}

void handle_client(int client_fd) {
    char buffer[1024] = {0};

    while (true) {
        // Receive a message from the client
        ssize_t bytes = read(client_fd, buffer, sizeof(buffer));
        if (bytes > 0) {
            std::cout << "[Server] Received: " << buffer << std::endl;

            // Send a response back to the client
            std::string reply = "Got: " + std::string(buffer);
            send(client_fd, reply.c_str(), reply.size(), 0);

        } else if (bytes == 0) {
            std::cout << "[Server] Client disconnected" << std::endl;
            std::cout << "[Server] Closing client connection" << std::endl; 
            shutdown(client_fd, SHUT_RDWR);
            break;

        } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // No data, sleep for a short time and retry
            std::this_thread::sleep_for(std::chrono::milliseconds(10));

        } else {
            break; // Other errors
        }
    }

    close(client_fd);
}

void signal_handler(int signal) {
    std::cout << "[Server] Caught signal " << signal << std::endl;
    std::cout << "[Server] Closing client connections" << std::endl;
    cleanup_connections();
    std::cout << "[Server] Shutting down server..." << std::endl;
    running = false;
}

/**
 * 1. Create a TCP socket
 * 2. Bind the socket to a port (e.g., 9000) and listen for incoming connections
 * 3. Accept an incoming client connection
 * 4. Loop:
 *    a. Receive a message from the client
 *    b. Print the received message
 *    c. Send a response back to the client
 * 5. Close the client socket
 * 6. Close the server socket
 */
int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);

    // Set server_fd to non-blocking mode
    int flags = fcntl(server_fd, F_GETFL, 0);
    fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);

    signal(SIGINT, signal_handler);

    std::vector<std::thread> threads;
    std::cout << "[Server] Listening on port " << PORT << std::endl;


    while (running) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd >= 0) {
            std::cout << "[Server] Client connected" << std::endl;
            client_fds.push_back(client_fd);
            threads.emplace_back(handle_client, client_fd);

        } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // No incoming connections, sleep for a short time and retry
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;

        } else {
            std::cerr << "[Server] Accept error: " << strerror(errno) << std::endl;
            break; // Other errors
        }
    }

    for (auto& t : threads) {
        t.join();
    }

    close(server_fd);
    return 0;
}