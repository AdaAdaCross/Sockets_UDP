﻿// UDP_client.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

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
    myaddress.sin_port = htons(0);
    myaddress.sin_addr.s_addr = inet_addr("192.168.1.8");

    struct sockaddr_in destination;
    destination.sin_family = AF_INET;
    destination.sin_port = htons(50011);
    destination.sin_addr.s_addr = inet_addr("192.168.1.8");
    int dest_size = sizeof(destination);

    if (bind(sock, (const struct sockaddr*)&myaddress, sizeof(myaddress)) < 0)
    {
        printf("Error of binding\n");
        closesocket(sock);
        UninitializeSockets();
        return -1;
    }

    while (true)
    {
        std::string buffer;
        std::cin >> buffer;
        char answer[11] = "";

        int code = sendto(sock, buffer.c_str(), buffer.size(), 0, (sockaddr*)&destination, sizeof(destination));
        if (code != buffer.size())
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

        code = recvfrom(sock, answer, 11, 0, (sockaddr*)&destination, &dest_size);
        if (code <= 0)
        {
            printf("Network error\n");
            return -1;
        }
        printf("Server answer = %s\n", answer);
    }
}
