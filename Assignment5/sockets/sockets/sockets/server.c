#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

int main() {
	int errco;
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
		errco = WSAGetLastError();
		printf("Initializing Winsock failed with error code: %d", errco);
		return errco;
	}

	return 0;
}
