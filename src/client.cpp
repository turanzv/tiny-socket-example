#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9000

/**
 * 1. Create a TCP socket
 * 2. Connect the socket to the server at `127.0.0.1` on the chosen port
 * 3. Loop:
 *    a. Get user input (a message)
 *    b. Send the message to the server
 *    c. Receive the server's response
 *    d. Print the response
 * 4. Close the client socket
 */
int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if(connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connetion failed" << std::endl;
        return 1;
    }

    std::cout << "connected ot server!" << std::endl;
    char buffer[1024] = {0};

    while (true) {
        std::string message;
        std::cout << "[Client] Enter message: ";
        std::getline(std::cin, message);

        send(sock, message.c_str(), message.size(), 0);

        ssize_t bytes = read(sock, buffer, sizeof(buffer));
        if (bytes <= 0) break;

        std::cout << "[Client] Server said: " << buffer << std::endl;
    }

    close(sock);
    return 0;
}