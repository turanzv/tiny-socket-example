#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9000

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
    listen(server_fd, 1);

    std::cout << "Server listening on port " << PORT << std::endl;

    int client_fd = accept(server_fd, nullptr, nullptr);
    std::cout << "Client connected!" << std::endl;

    char buffer[1024] = {0};

    while (true) {
        // Receive a message from the client
        ssize_t bytes = read(client_fd, buffer, sizeof(buffer));
        if (bytes <= 0) break;
        std::cout << "[Server] Received: " << buffer << std::endl;

        // Send a response back to the client
        std::string reply = "Got: " + std::string(buffer);
        send(client_fd, reply.c_str(), reply.size(), 0);
    }
}