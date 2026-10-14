#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

int main()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
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

    ifstream file("sample.txt", ios::binary);
    if (!file)
    {
        cout << "File sample.txt not found in the current directory.\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    char buffer[1024];
    while (file)
    {
        file.read(buffer, sizeof(buffer));
        int bytesRead = static_cast<int>(file.gcount());
        if (bytesRead > 0)
        {
            int sent = 0;
            while (sent < bytesRead)
            {
                int n = send(clientSocket, buffer + sent, bytesRead - sent, 0);
                if (n <= 0)
                {
                    cout << "Send failed.\n";
                    file.close();
                    closesocket(clientSocket);
                    WSACleanup();
                    return 1;
                }
                sent += n;
            }
        }
    }

    shutdown(clientSocket, SD_SEND);
    file.close();
    cout << "File sent successfully.\n";
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
