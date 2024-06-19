# chat-application-in-c
C program that simulates a simple chat application between multiple users using sockets for communication. This will involve creating a server that can handle multiple client connections, allowing them to send messages to each other through the server.


Explanation:
Server Side:

Global Variables and Structures: Client structure to store client information (socket, address, addrLen), clients array to hold connected clients, and numClients to keep track of the number of clients.
Main Function:
Creates a server socket and binds it to a specified port.
Listens for incoming connections using accept().
For each client connection, adds it to the clients array and creates a new thread (handleClient) to handle communication with the client.
Thread (handleClient):
Handles communication with an individual client.
Receives messages from the client using read() and broadcasts them to all other clients using broadcastMessage().
Removes the client from the clients array when disconnected.
Functions:
broadcastMessage(): Sends a message to all clients except the sender.
removeClient(): Removes a client from the clients array when disconnected.
Client Side:

Main Function:
Creates a client socket and connects to the server using connect().
Allows the user to input messages from stdin and sends them to the server using send().
Receives responses from the server using recv() and displays them to the user.
Closes the socket when the user exits the chat.
