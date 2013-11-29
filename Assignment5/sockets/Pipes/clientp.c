#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include "constantsp.h"


int main()
{
    HANDLE h;  //Handle for the pipe
    DWORD dwBytesRead;  //Number of bytes read from the pipe
    DWORD dwWritten;  //Number of bytes written to the pipe
    TCHAR recv_buf[MESG_BUFFER_SIZE];  //Buffer to store stream from pipe in
    LPTSTR send_buf = TEXT(CLIENT_MSG);  //Buffer to store message to write
    BOOL success = FALSE;  //bool for storing operation success

    //Wait for an instance of SERV_PIPE to become available for connection
    if (!WaitNamedPipe(SERV_PIPE, NMPWAIT_WAIT_FOREVER))
    {
        printf("Error code %d happened while waiting for the pipe. :(\n", GetLastError());
        return 1;
    }

    //Create a handle to SERV_PIPE
    h = CreateFile(
            SERV_PIPE,  //Pipe name
            GENERIC_WRITE | GENERIC_READ,  //Setup for both read and write
            0,  //File only accessible by this process
            NULL,  //No additional security attributes
            OPEN_EXISTING,  //Only open pipe if exists
            0,  //Default flags and attributes
            NULL  //No optional template file
        );
    if (h == INVALID_HANDLE_VALUE)
    {
        printf("Error code %d happened during CreateFile. :(\n", GetLastError());
        return 1;
    }
    if (DEBUG)printf("Successfully connected to pipe. :)\n");

    //Write message to server
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
    FlushFileBuffers(h);
    if (DEBUG)printf("Successfully wrote message to the server.\n");

    // While the server sends data, receive it
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
    printf("Message from server: %s\n", recv_buf);

    //Close the handle and return
    CloseHandle(h);
    return 0;
}
