#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9000

int main() {
    int udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_fd < 0) {
        std::cerr << "[Client] Failed to create socket" << std::endl;
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    char buffer[1024];
    socklen_t addr_len = sizeof(server_addr);

    while (true) {
        std::string message;
        std::cout << "[Client] Enter message: ";
        std::getline(std::cin, message);

        // Send message to server
        sendto(udp_fd, message.c_str(), message.size(), 0, (struct sockaddr*) &server_addr, sizeof(server_addr));

        // Receive response from server
        size_t bytes = recvfrom(udp_fd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*) &server_addr, &addr_len);

        if (bytes < 0) {
            std::cerr << "[Client] Error receiving message" << std::endl;
            continue;
        }

        buffer[bytes] = '\0'; // Null-terminate the message
        std::cout << "[Client] Server said: " << buffer << std::endl;
    }

    close(udp_fd);
    return 0;
}