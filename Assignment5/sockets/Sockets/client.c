#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "Ws2_32.lib")

#define BUFFERL 80

/*
* argv
* 1) Server address
* 2) Server port
*/

int cleanupAfterFailedSend(int errco, SOCKET *sock, char *errmsg)
{
    if (errco == SOCKET_ERROR)
    {
        errco = WSAGetLastError();
        printf("%s; error code: %d\n", errmsg, errco);
        closesocket(*sock);
        WSACleanup();
        return errco;
    }
    else
        return 0;
}

int main(int argc, char **argv)
{
    int errco;  //Holds return code for winsock functions
    WSADATA wsaData;  //The WSADATA structure contains information about the Windows Sockets implementation.
    struct addrinfo *result = NULL, *ptr = NULL, hints;  //Struct holding data used for connecting
    SOCKET sock = INVALID_SOCKET;
    char *message = "Hello! I am a client!";  //Message to send to server
    char recv_buffer[BUFFERL];  //Buffer to receive reply from server

    // Validate the parameters
    if (argc != 3)
    {
        printf("USAGE: %s server-address server-port\n", argv[0]);
        printf("e.g., %s 127.0.0.1 25213\n", argv[0]);
        return 1;
    }

    // Initialize Winsock here (required step on both client and server)
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        errco = WSAGetLastError();
        printf("Initializing Winsock failed with error code: %d\n", errco);
        return errco;
    }

    //Fill out addrinfo hints for a TCP stream socket
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    //Resolve the server address and port; if could not, clean up and quit
    errco = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (errco != 0)
    {
        printf("Resolving the server address and port failed with error code: %d\n", errco);
        WSACleanup();
        return errco;
    }

    // Try all suggested address from getaddrinfo until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        // Create a SOCKET
        sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (sock == INVALID_SOCKET)
        {
            errco = WSAGetLastError();
            printf("Could not create a SOCKET; error code: %d\n", errco);
            freeaddrinfo(result); WSACleanup();
            return errco;
        }

        // Now we actually try and connect to the server
        errco = connect( sock, ptr->ai_addr, (int) ptr->ai_addrlen);
        if (errco == SOCKET_ERROR)
        {
            closesocket(sock);
            sock = INVALID_SOCKET;
            continue;
        }
        break;
    }
    // Clean up result as no longer needed
    freeaddrinfo(result);
    // If a connection wasn't successful, quit
    if (sock == INVALID_SOCKET)
    {
        printf("We weren't able to connect to the server at %s:%s :(\n", argv[1], argv[2]);
        WSACleanup();
        return 1;
    }

    //Send message to server; if we couldn't, clean up and die
    errco = send(sock, message, (int) strlen(message), 0);
    errco = cleanupAfterFailedSend(errco, &sock, "We couldn't send the message");
    if (errco) return errco;

    //Shutdown the connection for sending as it's no longer needed
    errco = shutdown(sock, SD_SEND);
    errco = cleanupAfterFailedSend(errco, &sock, "We couldn't shutdown the connection for sending");
    if (errco) return errco;

    //Receive data until the server chooses to step sending
    do
    {
        errco = recv(sock, recv_buffer, BUFFERL, 0);
        if (errco > 0) printf("The server says: %s\n", recv_buffer);
        else if (!errco) printf("Connection closed\n");
        else printf("Receiving message from the server failed; error code: %d\n", WSAGetLastError());
    }
    while (errco > 0);

    //cleanup and finish
    closesocket(sock);
    WSACleanup();

    return 0;
}
