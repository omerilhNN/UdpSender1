#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>
#include <msxml6.h>

#pragma comment(lib, "ws2_32.lib")

#define SIZE 1024
#define PORT 23
int main() {
    WSADATA wsa_data;
    char ip[INET_ADDRSTRLEN], command[32], path[64], recvBuf[SIZE];

    int bytes_recv;
    
    int wsa = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (wsa != 0) {
        printf("[ERROR] WSAStartup failed: %d\n", wsa);
        WSACleanup();
        return 1;
    }

    SOCKET server_sockfd;
    struct sockaddr_in server_addr,from_addr;
    socklen_t fromlen = sizeof(from_addr);

    //command inputunu al
    printf("command IP path\n");
    scanf_s("%s %s %s", command,32, ip,INET_ADDRSTRLEN, path,64);
    if (strcmp(command,"send") == 0 && ip != NULL && path != NULL) {
        
        //udp soket tanımla
        server_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        
        //verdiğin pathteki dosyayı "read" modunda aç
        FILE* sendFile = fopen(path,"rb");
        if (sendFile == NULL) {
            printf("Failed to open file\n");
            WSACleanup();
            return -1;
        }
        //file boyutunu al
        fseek(sendFile, 0, SEEK_END);
        int fileSize = ftell(sendFile);
        rewind(sendFile);
        char* buffer = (char*)malloc(sizeof(char) * SIZE);
        //fread ile buffer'a fileSize kadar okuma yap - okunan byte sayısını readedBytes'a ver
        if (server_sockfd == INVALID_SOCKET) {
            printf("[ERROR] socket error: %d\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }
        
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        

        ////presentation to network - string verilen arrayi network(binary form) çevir
        if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0)
        {
            printf("Invalid IP\n");
            closesocket(server_sockfd);
            return 1;
        }
        //Soketi - server_addr'in IP ve portuna ba�la
        if (connect(server_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
            printf("Connect error\n");
            closesocket(server_sockfd);
            return 1;
        }

        int ctr =0;
        int bytes_sent;
        size_t readedBytes ;
        while ((readedBytes = fread(buffer, 1, SIZE, sendFile)) > 0) {
            ctr += readedBytes;
            bytes_sent = sendto(server_sockfd, buffer, (int)readedBytes, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
            if (bytes_sent < 0) {
                printf("[ERROR] sending data to the server: %d\n", WSAGetLastError());
                closesocket(server_sockfd);
                WSACleanup();
                return 1;
            }
        }
        printf("Data sent: %d bytes\n", fileSize);
        const char* end_msg = "END";
        sendto(server_sockfd, end_msg, sizeof(end_msg), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
        fclose(sendFile);
       
    }
    else {
           printf("Invalid command || IP || Path entered.\n");
           return -1;
    }

    closesocket(server_sockfd);
    WSACleanup();

    return 0;
}