#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

#define SERV_PIPE "\\\\.\\pipe\\serv_pipe"
#define PIPE_BUFFER_SIZE 1024
#define MESG_BUFFER_SIZE 80
#define TIMEOUT 2000

int main()
{
    HANDLE h;  //Handle for the pipe
    DWORD dwBytesRead;  //Number of bytes read from the pipe
    DWORD dwWritten;  //Number of bytes written to the pipe
    TCHAR recv_buf[MESG_BUFFER_SIZE];  //Buffer to store stream from pipe in
    LPTSTR send_buf = TEXT("Hello! I am a server!");  //Buffer to store message to write

    // Create a named pipe
    h = CreateNamedPipe(
            SERV_PIPE,  //Name of the pipe as defined above
            PIPE_ACCESS_DUPLEX,  //OpenMode: bidirectional
            //RW stream of bytes, blocking
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,  //Instances of the pipe that can be created
            PIPE_BUFFER_SIZE,  //Input buffer size
            PIPE_BUFFER_SIZE,  //Output buffer size
            TIMEOUT,  //Time to wait before timing out,
            NULL  //Optional security attributes
        );
    // In case of an invalid handle, die
    if (h == INVALID_HANDLE_VALUE)
    {
        printf("Pipe creation failed, ERR %d\n", GetLastError());
        return 1;
    }
    // If we got a valid handle, connect to the pipe
    ConnectNamedPipe(h, NULL);
    printf("Pipe successfully created and connected. :)\n"); fflush(stdout);
    // While the client sends data, receive it
    while (1)
    {
        if (!ReadFile(
                    h,
                    recv_buf,
                    MESG_BUFFER_SIZE * sizeof(TCHAR),
                    &dwBytesRead,
                    NULL))
            break;
    }
    //Print received message
    printf("Message from client: %s\n", recv_buf); fflush(stdout);

    //sprintf(send_buf, "Hello! I am a server!");
    if (!WriteFile(
                h,
                send_buf,
                (lstrlen(send_buf) + 1)*sizeof(TCHAR),
                &dwWritten,
                NULL))
    {
        printf("Failed to write to the pipe. :( ERR %d\n", GetLastError()); fflush(stdout);
        return 1;
    }

    //Disconnect pipe, close handle
    FlushFileBuffers(h);
    DisconnectNamedPipe(h);
    CloseHandle(h);
    return 0;
}
