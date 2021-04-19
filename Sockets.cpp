#ifdef WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#pragma comment (lib, "Ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <iostream>
#include <string>

void InitializeSockets()
{
#ifdef WIN32
    WSADATA wsaData;
    int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (res != 0)
    {
        printf("WSAStartup failed with error: %d\n", res);
        return;
    }
#endif // WIN32
}

void UninitializeSockets()
{
#ifdef WIN32
    WSACleanup();
#endif
}

int main()
{
    InitializeSockets();

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1)
    {
        printf("Error of sockets created\n");
        return -1;
    }

    struct sockaddr_in myaddress;
    myaddress.sin_family = AF_INET;
    myaddress.sin_port = htons(50011);
    myaddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (const struct sockaddr*)&myaddress, sizeof(myaddress)) < 0)
    {
        printf("Error of binding\n");
        closesocket(sock);
        UninitializeSockets();
        return -1;
    }

    while (true)
    {
        char buffer[11] = "";
        struct sockaddr_in client;
        client.sin_family = AF_INET;
        client.sin_port = htons(0);
        client.sin_addr.s_addr = INADDR_ANY;
        int client_addr_size = sizeof(client);

        int code = recvfrom(sock, buffer, 11, 0, (sockaddr*)&client, &client_addr_size);
        if (code <= 0)
        {
            printf("Network error\n");
            continue;
        }

        printf("Received message: %s\n", buffer);
        int number = std::atoi(buffer);

        std::string answer;

        if (number == 0)
        {
            answer = "Wrong msg";
        }
        else
        {
            number *= number;
            answer = std::to_string(number);
        }

        code = sendto(sock, answer.c_str(), answer.size(), 0, (sockaddr*)&client, sizeof(client));
        if (code != answer.size())
        {
            if (code <= 0)
            {
                printf("Network error\n");
            }
            else
            {
                printf("Network buffer error\n");
            }
            return -1;
        }
    }
}
