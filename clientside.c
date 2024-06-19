#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define MAX_MSG_LENGTH 256

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char message[MAX_MSG_LENGTH];

    // Create client socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_port = htons(PORT);

    // Connect to server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to chat server.\n");

    // Send and receive messages
    while (1) {
        printf("Enter message: ");
        fgets(message, MAX_MSG_LENGTH, stdin);

        // Remove newline character
        message[strcspn(message, "\n")] = '\0';

        // Send message to server
        send(clientSocket, message, strlen(message), 0);

        // Receive response from server
        memset(message, 0, sizeof(message));
        if (recv(clientSocket, message, MAX_MSG_LENGTH, 0) == 0) {
            printf("Server disconnected.\n");
            break;
        }

        printf("Server response: %s\n", message);
    }

    // Close socket
    close(clientSocket);

    return 0;
}
