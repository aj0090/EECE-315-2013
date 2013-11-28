#include <windows.h>
#include <stdio.h>

#define SERV_PIPE "\\\\.\\pipe\\serv_pipe"
#define PIPE_BUFFER_SIZE 1024
#define MESG_BUFFER_SIZE 80
#define TIMEOUT 4000

int main()
{
    HANDLE h;  //Handle for the pipe
    DWORD dwBytesRead;  //Number of bytes read from the pipe
    char buf[MESG_BUFFER_SIZE];  //Buffer to store stream from pipe in

    // Create a named pipe
    h = CreateNamedPipe(
                   SERV_PIPE,  //Name of the pipe as defined above
                   PIPE_ACCESS_DUPLEX | WRITE_DAC,  //OpenMode: bidirectional
                   //RW stream of bytes, blocking
                   PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                   1,  //Instances of the pipe that can be created
                   PIPE_BUFFER_SIZE,  //Input buffer size
                   PIPE_BUFFER_SIZE,  //Output buffer size
                   TIMEOUT,  //Time to wait before timing out,
                   NULL  //Optional security attributes
               );
    // In case of an invalid handle, die
    if (h == INVALID_HANDLE_VALUE)
    {
        printf("Pipe creation failed\n");
        return 1;
    }
    // If we got a valid handle, connect to the pipe
    ConnectNamedPipe(h, NULL);
    // While the client sends data, receive it
    while (!0)
        if (!ReadFile(h, buf, MESG_BUFFER_SIZE, &dwBytesRead, NULL)) break;
    //Print received message
    printf("Message from client: %s\n", buf);


    //Disconnect pipe, close handle
    DisconnectNamedPipe(h);
    CloseHandle(h);
    return 0;
}
