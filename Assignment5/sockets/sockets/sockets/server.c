#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "Ws2_32.lib")

/*
 * argv
 * 1) Server address
 * 2) Server port
 */

int main(int argc, char **argv) {
	int errco;  //Holds return code for winsock functions
	WSADATA wsaData;  //The WSADATA structure contains information about the Windows Sockets implementation.
	struct addrinfo *result = NULL, *ptr = NULL, hints;  //Struct holding data used for connecting
	SOCKET sock = INVALID_SOCKET;

	// Initialize Winsock here (required step on both client and server)
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
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
	if (errco != 0) {
		printf("Resolving the server address and port failed with error code: %d\n", errco);
		WSACleanup();
		return errco;
	}

	// Try all suggested address from getaddrinfo until one succeeds
	for(ptr=result; ptr != NULL; ptr=ptr->ai_next) {
		// Create a SOCKET
		sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (sock == INVALID_SOCKET) {
			errco = WSAGetLastError();
			printf("Could not create a SOCKET; error code: %d\n", errco);
			freeaddrinfo(result); WSACleanup();
			return errco;
		}

		// Now we actually try and connect to the server
		errco = connect( sock, ptr->ai_addr, (int) ptr->ai_addrlen);
		if (errco == SOCKET_ERROR) {
			closesocket(sock);
			sock = INVALID_SOCKET;
			continue;
		}
		break;
	}
	// Clean up result as no longer needed
	freeaddrinfo(result);
	// If a connection wasn't successful, quit
    if (sock == INVALID_SOCKET) {
        printf("We weren't able to connect to the server at %s:%S :(\n", argv[1], argv[2]);
        WSACleanup();
        return 1;
    }



	return 0;
}
