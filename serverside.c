#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define MAX_MSG_LENGTH 256

// Structure to hold client information
typedef struct {
    int socket;
    struct sockaddr_in address;
    int addrLen;
} Client;

// Global variables
Client clients[MAX_CLIENTS];
int numClients = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Function prototypes
void *handleClient(void *arg);
void broadcastMessage(char *message, int senderSocket);
void removeClient(int index);

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    pthread_t threadID;

    // Create server socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind socket to address
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Chat server started on port %d...\n", PORT);

    // Accept incoming connections
    while (1) {
        int clientAddrLen = sizeof(clientAddr);
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, (socklen_t *)&clientAddrLen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        printf("New client connected: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        // Add client to clients array
        pthread_mutex_lock(&mutex);
        clients[numClients].socket = clientSocket;
        clients[numClients].address = clientAddr;
        clients[numClients].addrLen = clientAddrLen;
        numClients++;
        pthread_mutex_unlock(&mutex);

        // Create thread to handle client
        if (pthread_create(&threadID, NULL, handleClient, (void *)&clientSocket) < 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

// Thread function to handle individual client
void *handleClient(void *arg) {
    int clientSocket = *((int *)arg);
    char message[MAX_MSG_LENGTH];

    while (1) {
        int readSize = read(clientSocket, message, MAX_MSG_LENGTH);
        if (readSize == 0) {
            // Client disconnected
            pthread_mutex_lock(&mutex);
            close(clientSocket);
            removeClient(clientSocket);
            pthread_mutex_unlock(&mutex);
            break;
        } else if (readSize > 0) {
            // Message received
            message[readSize] = '\0';
            printf("Received message from client %d: %s\n", clientSocket, message);
            broadcastMessage(message, clientSocket);
        } else {
            perror("Read failed");
            break;
        }
    }

    pthread_exit(NULL);
}

// Broadcast message to all clients except sender
void broadcastMessage(char *message, int senderSocket) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < numClients; i++) {
        if (clients[i].socket != senderSocket) {
            write(clients[i].socket, message, strlen(message));
        }
    }
    pthread_mutex_unlock(&mutex);
}

// Remove client from clients array
void removeClient(int index) {
    for (int i = 0; i < numClients; i++) {
        if (clients[i].socket == index) {
            for (int j = i; j < numClients - 1; j++) {
                clients[j] = clients[j + 1];
            }
            numClients--;
            break;
        }
    }
}
