#include <iostream>
#include <cstring>
#include <winsock2.h>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "WSAStartup failed.\n";
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cout << "Socket creation failed.\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in server{}, client{};
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8080);

    if (bind(serverSocket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR ||
        listen(serverSocket, 1) == SOCKET_ERROR) {
        cout << "Bind/listen failed.\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Server waiting for connection...\n";
    int clientSize = sizeof(client);
    SOCKET clientSocket = accept(serverSocket, (sockaddr*)&client, &clientSize);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Accept failed.\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    char buffer[1024]{};
    int n = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (n > 0) {
        buffer[n] = '\0';
        cout << "Client: " << buffer << endl;
    }

    const char message[] = "Hello Client!";
    send(clientSocket, message, sizeof(message), 0);

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
