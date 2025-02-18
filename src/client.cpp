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

    std::cout << "connected to server!" << std::endl;
    char buffer[1024] = {0};

    fd_set read_fs;

    while (true) {
        FD_ZERO(&read_fs);
        FD_SET(sock, &read_fs); // Monitor the socket for incoming data
        FD_SET(STDIN_FILENO, &read_fs); // Monitor stdin for user input

        int max_fd = sock > STDIN_FILENO ? sock : STDIN_FILENO;
        int activity = select(max_fd + 1, &read_fs, nullptr, nullptr, nullptr);

        if (activity < 0) {
            std::cerr << "[Client] Error in select()" << std::endl;
            break;
        }

        if (FD_ISSET(sock, &read_fs)) {
            ssize_t bytes = read(sock, buffer, sizeof(buffer));
            if (bytes == 0) {
                std::cout << "[Client] Server has shut down the communication." << std::endl;
                break;
            } else if (bytes < 0) {
                std::cerr << "[Client] Error receiving message" << std::endl;
                break;
            }

            std::cout << "[Client] Server said: " << buffer << std::endl;
        }

        if (FD_ISSET(STDIN_FILENO, &read_fs)) {
            std::string message;
            std::getline(std::cin, message);

            if (message == "exit") {
                std::cout << "[Client] Exiting..." << std::endl;
                shutdown(sock, SHUT_WR);
                continue;
            }

            send(sock, message.c_str(), message.size(), 0);
        }
    }

    close(sock);
    return 0;
}