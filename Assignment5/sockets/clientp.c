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
    DWORD dwWritten;  //Number of bytes written to the pipe
    char buf[MESG_BUFFER_SIZE];  //Buffer to store stream from pipe in

    if (!WaitNamedPipe(SERV_PIPE, NMPWAIT_WAIT_FOREVER))
    {
        printf("Error code %d happened while waiting for the pipe. :(\n", GetLastError());
        return 1;
    }
    h = CreateFile(
            SERV_PIPE,
            GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );
    if (h == INVALID_HANDLE_VALUE)
    {
        printf("Error code %d happened during CreateFile. :(\n", GetLastError());
        return 1;
    }

    sprintf(buf, "Hello! I am a client!");
    if (!WriteFile(h, buf, MESG_BUFFER_SIZE, &dwWritten, NULL))
    {
        printf("Failed to write to the pipe. :(\n");
        return 1;
    }

    //Close the handle and return
    CloseHandle(h);
    return 0;
}
