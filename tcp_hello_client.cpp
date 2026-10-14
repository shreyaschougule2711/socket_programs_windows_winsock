#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

int main()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        cout << "WSAStartup failed.\n";
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        cout << "Socket creation failed.\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket, (sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        cout << "Connection failed. Start the server first.\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    const char message[] = "Hello Server!";
    send(clientSocket, message, sizeof(message), 0);

    char buffer[1024]{};
    int n = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (n > 0)
    {
        buffer[n] = '\0';
        cout << "Server: " << buffer << endl;
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
