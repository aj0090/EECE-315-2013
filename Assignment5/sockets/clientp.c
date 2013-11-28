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
    LPTSTR send_buf = TEXT("Hello! I am a client!");  //Buffer to store message to write


    if (!WaitNamedPipe(SERV_PIPE, NMPWAIT_WAIT_FOREVER))
    {
        printf("Error code %d happened while waiting for the pipe. :(\n", GetLastError());
        return 1;
    }
    h = CreateFile(
            SERV_PIPE,
            GENERIC_WRITE | GENERIC_READ,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL
        );
    if (h == INVALID_HANDLE_VALUE)
    {
        printf("Error code %d happened during CreateFile. :(\n", GetLastError());
        return 1;
    }
    printf("Successfully connected to pipe. :)\n"); fflush(stdout);

    sprintf(send_buf, "Hello! I am a client!");
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
    FlushFileBuffers(h);
    printf("Successfully wrote message to the server.\n"); fflush(stdout);

    // While the server sends data, receive it
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
    printf("Message from server: %s\n", recv_buf); fflush(stdout);

    //Close the handle and return
    CloseHandle(h);
    return 0;
}
