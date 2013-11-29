#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "Ws2_32.lib")

#define BUFFERL 80
#define DEFAULT_PORT "25213"

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
    struct addrinfo *result = NULL, *ptr = NULL, hints; //Struct holding data used for connecting
    SOCKET sock = INVALID_SOCKET;
    SOCKET csock = INVALID_SOCKET;
    char recv_buffer[BUFFERL];
    int send_errco;

    // Initialize Winsock here (required step on both client and server)
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        errco = WSAGetLastError();
        printf("Initializing Winsock failed with error code: %d\n", errco);
        return errco;
    }

    //Fill out addrinfo hints for a TCP stream socket
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    //Get address and port that will be used by the server
    errco = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (errco)
    {
        printf("Failed to resolve address and port to be used by the server with error code: %d\n", errco);
        WSACleanup();
        return errco;
    }

    //Initialize socket and listen for connections from a client
    sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    //Ensure that socket was actually created
    if (sock == INVALID_SOCKET)
    {
        errco = WSAGetLastError();
        printf("We couldn't initialize the socket with error code: %d\n", errco);
        freeaddrinfo(result);
        WSACleanup();
        return errco;
    }

    // Bind the socket, if not successful, fail
    errco = bind(sock, result->ai_addr, (int) result->ai_addrlen);
    if (errco == SOCKET_ERROR)
    {
        freeaddrinfo(result);
        errco = cleanupAfterFailedSend(errco, &sock, "We couldn't bind the socket");
    }
    if (errco) return errco;

    printf("Successfully bound on localhost:%s\n", DEFAULT_PORT);
    freeaddrinfo(result);  //This is no longer needed, so we free it

    //Start listening on socket; if there was an error, die
    errco = listen(sock, SOMAXCONN);
    errco = cleanupAfterFailedSend(errco, &sock, "Listen failed");
    if (errco) return errco;

    //Accept a connection from client on client socket
    csock = accept(sock, NULL, NULL);
    if (csock == INVALID_SOCKET)
    {
        errco = WSAGetLastError();
        errco = cleanupAfterFailedSend(errco, &sock, "Accept from client failed");
        if (errco) return errco;
    }

    //Receive data from the client until the client stops sending
    do
    {
        errco = recv(csock, recv_buffer, BUFFERL, 0);
        if (errco > 0)
        {
            printf("The client says: %s\n", recv_buffer);
            send_errco = send(csock, "Hello! I am a server!", errco, 0);
            send_errco = cleanupAfterFailedSend(send_errco, &csock, "Couldn't send a reply to the client");
            if (send_errco) return send_errco;
        }
        else if (!errco)
        {
            printf("Closing connection . . . \n");
        }
        else
        {
            send_errco = cleanupAfterFailedSend(SOCKET_ERROR, &csock, "Failed to receive message from client");
            if (send_errco) return send_errco;
        }
    }
    while (errco > 0);

    //Cleanup and shutdown
    errco = shutdown(csock, SD_SEND);
    errco = cleanupAfterFailedSend(errco, &csock, "Failed to shutdown socket");
    if (errco) return errco;

    closesocket(csock);
    WSACleanup();

    return 0;
}
