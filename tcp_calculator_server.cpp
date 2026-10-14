#include <iostream>
#include <winsock2.h>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

bool receiveAll(SOCKET s, char* data, int length) {
    int received = 0;
    while (received < length) {
        int n = recv(s, data + received, length - received, 0);
        if (n <= 0) return false;
        received += n;
    }
    return true;
}

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

    cout << "Waiting for client...\n";
    int clientSize = sizeof(client);
    SOCKET clientSocket = accept(serverSocket, (sockaddr*)&client, &clientSize);
    if (clientSocket == INVALID_SOCKET) {
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    double a, b, result = 0;
    char op;
    bool ok = receiveAll(clientSocket, (char*)&a, sizeof(a)) &&
              receiveAll(clientSocket, (char*)&b, sizeof(b)) &&
              receiveAll(clientSocket, &op, sizeof(op));

    if (ok) {
        switch (op) {
            case '+': result = a + b; break;
            case '-': result = a - b; break;
            case '*': result = a * b; break;
            case '/':
                if (b == 0) {
                    cout << "Division by zero is not allowed.\n";
                    result = 0;
                } else result = a / b;
                break;
            default:
                cout << "Invalid operator.\n";
                result = 0;
        }
        send(clientSocket, (char*)&result, sizeof(result), 0);
        cout << "Calculation completed.\n";
    }

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
