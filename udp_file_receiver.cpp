#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <winsock2.h>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

const int PORT = 9000;
const int CHUNK = 1024;
const int TIMEOUT_MS = 3000;

struct Packet {
    int seq;
    int size;
    char data[CHUNK];
};

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

    SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == INVALID_SOCKET) {
        WSACleanup();
        return 1;
    }

    sockaddr_in server{}, client{};
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(s, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        cout << "Bind failed. Is port 9000 already in use?\n";
        closesocket(s);
        WSACleanup();
        return 1;
    }

    cout << "UDP Receiver waiting on port " << PORT << "...\n";
    int clientLen = sizeof(client);
    int expected = 0;
    ofstream file;
    bool receiving = false;
    string filename;

    DWORD timeout = TIMEOUT_MS;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

    while (true) {
        char raw[sizeof(Packet)]{};
        int n = recvfrom(s, raw, sizeof(raw), 0, (sockaddr*)&client, &clientLen);
        if (n == SOCKET_ERROR) continue;

        if (!receiving) {
            filename.assign(raw, n);
            size_t pos = filename.find_last_of("/\\");
            if (pos != string::npos) filename = filename.substr(pos + 1);
            if (filename.empty() || filename == "." || filename == "..") {
                cout << "Invalid filename.\n";
                continue;
            }

            file.open("received_" + filename, ios::binary);
            if (!file) {
                cout << "Cannot create output file.\n";
                continue;
            }
            receiving = true;
            expected = 0;
            cout << "Receiving: " << filename << endl;
            continue;
        }

        if (n < (int)(sizeof(int) * 2) || n > (int)sizeof(Packet)) continue;
        Packet p{};
        memcpy(&p, raw, n);
        if (p.size < 0 || p.size > CHUNK ||
            n != (int)(sizeof(int) * 2 + p.size)) continue;

        if (p.seq == expected) {
            if (p.size > 0) file.write(p.data, p.size);
            expected++;
        }

        int ack = expected;
        sendto(s, (char*)&ack, sizeof(ack), 0, (sockaddr*)&client, clientLen);

        if (p.size == 0 && p.seq == expected - 1) {
            file.close();
            receiving = false;
            cout << "File received successfully.\n";
        }
    }

    closesocket(s);
    WSACleanup();
    return 0;
}
