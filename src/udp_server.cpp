#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9000

int main() {
    int udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_fd < 0) {
        std::cerr << "[Server] Failed to create socket" << std::endl;
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(udp_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "[Server] bind failed" << std::endl;
        close(udp_fd);
    }

    std::cout << "[Server] Listening on port " << PORT << std::endl;

    char buffer[1024];
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while (true) {
        memset(buffer, 0, sizeof(buffer));

        // Receive a message from the client
        size_t bytes = recvfrom(udp_fd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*) &client_addr, &client_len);

        if (bytes < 0) {
            std::cerr << "[Server] Error receiving message" << std::endl;
            continue;
        }

        buffer[bytes] = '\0'; // Null-terminate the message
        std::cout << "[Server] Received: " << buffer <<  " from "
            << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;

        // Respond to the same client
        std::string response = "Got: " + std::string(buffer);
        sendto(udp_fd, response.c_str(), response.size(), 0, (struct sockaddr*) &client_addr, client_len);
    }
    
    close(udp_fd);
    return 0;
}