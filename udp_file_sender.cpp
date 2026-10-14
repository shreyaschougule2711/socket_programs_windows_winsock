#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

const int PORT = 9000;
const int CHUNK = 1024;
const int TIMEOUT_MS = 1000;
const int MAX_RETRIES = 10;

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

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    // Replace this with the receiver computer's IPv4 address.
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    DWORD timeout = TIMEOUT_MS;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

    string filename;
    cout << "Enter file name (e.g. sample.txt): ";
    getline(cin, filename);

    ifstream file(filename, ios::binary);
    if (!file) {
        cout << "Cannot open file.\n";
        closesocket(s);
        WSACleanup();
        return 1;
    }

    size_t slash = filename.find_last_of("/\\");
    string base = (slash == string::npos) ? filename : filename.substr(slash + 1);
    sendto(s, base.c_str(), (int)base.size(), 0, (sockaddr*)&server, sizeof(server));

    int seq = 0;
    bool done = false;
    while (!done) {
        Packet p{};
        p.seq = seq;
        file.read(p.data, CHUNK);
        p.size = (int)file.gcount();

        int packetBytes = (int)(sizeof(int) * 2 + p.size);
        bool acknowledged = false;

        for (int attempt = 0; attempt < MAX_RETRIES; attempt++) {
            sendto(s, (char*)&p, packetBytes, 0, (sockaddr*)&server, sizeof(server));

            int ack = -1;
            sockaddr_in from{};
            int fromLen = sizeof(from);
            int n = recvfrom(s, (char*)&ack, sizeof(ack), 0, (sockaddr*)&from, &fromLen);
            if (n == sizeof(ack) && ack == seq + 1) {
                acknowledged = true;
                break;
            }
        }

        if (!acknowledged) {
            cout << "Transfer failed: no acknowledgment.\n";
            break;
        }

        if (p.size == 0) done = true;
        else seq++;
    }

    file.close();
    if (done) cout << "File sent successfully.\n";

    closesocket(s);
    WSACleanup();
    return 0;
}
