#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include "constantsp.h"


int main()
{
    HANDLE h;  //Handle for the pipe
    DWORD dwBytesRead = 0; //Number of bytes read from the pipe
    DWORD dwWritten = 0; //Number of bytes written to the pipe
    TCHAR recv_buf[MESG_BUFFER_SIZE];  //Buffer to store stream from pipe in
    LPTSTR send_buf = TEXT(SERVER_MSG);  //Buffer to store message to write
    BOOL success = FALSE;  //bool for storing operation success

    // Create a named pipe
    h = CreateNamedPipe(
            SERV_PIPE,  //Name of the pipe as defined above
            PIPE_ACCESS_DUPLEX,  //OpenMode: bidirectional
            //RW messages, blocking
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
        printf("Pipe creation failed; Error code: %d\n", GetLastError());
        return 1;
    }

    // If we got a valid handle, connect to the pipe
    ConnectNamedPipe(h, NULL);
    if (DEBUG)printf("Pipe successfully created and connected. :)\n");

    // While the client sends data, receive it
    do
    {
        success = ReadFile(
                      h,  //Handle to pipe
                      recv_buf,  //Buffer to store read message
                      MESG_BUFFER_SIZE * sizeof(TCHAR),  //Size of message to read
                      &dwBytesRead,  //Address of variable that stores num. bytes read
                      NULL  //Not using overlapped
                  );
    }
    while (!success);
    //Print received message
    printf("Message from client: %s\n", recv_buf);

    //Write a reply
    if (!WriteFile(
                h,  //Handle to pipe
                send_buf,  //Message to send
                (lstrlen(send_buf) + 1)*sizeof(TCHAR),  //Size of message to send
                &dwWritten,  //Number of bytes written
                NULL  //Overlapped not used
            )
       )
    {
        printf("Failed to write to the pipe; Error code: %d\n", GetLastError());
        return 1;
    }

    //Flush pipe, disconnect pipe, close handle
    FlushFileBuffers(h);
    DisconnectNamedPipe(h);
    CloseHandle(h);
    return 0;
}
