#include <iostream>
#include <fstream>
#include <winsock2.h>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
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

    cout << "Waiting for file...\n";
    int clientSize = sizeof(client);
    SOCKET clientSocket = accept(serverSocket, (sockaddr*)&client, &clientSize);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Accept failed.\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    ofstream file("received.txt", ios::binary);
    char buffer[1024];
    int bytesReceived;
    while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        file.write(buffer, bytesReceived);
    }

    file.close();
    cout << "File received successfully as received.txt.\n";
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
