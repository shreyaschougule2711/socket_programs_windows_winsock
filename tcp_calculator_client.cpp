#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
using namespace std;

bool receiveAll(SOCKET s, char *data, int length)
{
    int received = 0;
    while (received < length)
    {
        int n = recv(s, data + received, length - received, 0);
        if (n <= 0)
            return false;
        received += n;
    }
    return true;
}

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

    double a, b, result;
    char op;
    cout << "Enter first number: ";
    cin >> a;
    cout << "Enter operator (+, -, *, /): ";
    cin >> op;
    cout << "Enter second number: ";
    cin >> b;

    bool sent = send(clientSocket, (char *)&a, sizeof(a), 0) == sizeof(a) &&
                send(clientSocket, (char *)&b, sizeof(b), 0) == sizeof(b) &&
                send(clientSocket, &op, sizeof(op), 0) == sizeof(op);

    if (sent && receiveAll(clientSocket, (char *)&result, sizeof(result)))
    {
        if (op == '/' && b == 0)
            cout << "Cannot divide by zero.\n";
        else
            cout << "Result = " << result << endl;
    }
    else
    {
        cout << "Communication failed.\n";
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
