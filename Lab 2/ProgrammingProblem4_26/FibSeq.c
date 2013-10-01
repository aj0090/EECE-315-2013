#include <windows.h>
#include <stdio.h>

#define MAX_COUNT 20

DWORD Fib[MAX_COUNT]; /* data is shared by the thread(s) */

/* the thread runs in this separate function */
DWORD WINAPI GetFibSeq(LPVOID Param)
{
	DWORD Upper = *(DWORD*)Param;
	DWORD i;

	Fib[0] = 0;
	Fib[1] = 1;

	for (i = 2; (i <= MAX_COUNT) && (i <= Upper); i++)
	{
		Fib[i] = Fib[i-1] + Fib[i-2];
	}

	return 0;
}

int main(int argc, char *argv[])
{
	DWORD ThreadId;
	HANDLE ThreadHandle;
	int Param;
	int i;

	if (argc != 2)
	{
		fprintf(stderr,"An integer parameter is required\n");
		return -1;
	}
	Param = atoi(argv[1]);
	if (Param < 0 || Param > MAX_COUNT)
	{
		fprintf(stderr,"An integer >= 0 and < %d is required\n", MAX_COUNT);
		return -1;
	}

	/* create the thread */
	ThreadHandle = CreateThread(
		NULL, /* default security attributes */
		0, /* default stack size */
		GetFibSeq, /* thread function */
		&Param, /* parameter to thread function */
		0, /* default creation flags */
		&ThreadId); /* returns the thread identifier */

	if (ThreadHandle != NULL)
	{
		/* now wait for the thread to finish */
		WaitForSingleObject(ThreadHandle,INFINITE);

		/* close the thread handle */
		CloseHandle(ThreadHandle);

		printf("The %dth Fibonacci sequence is:", Param);

		for (i = 0; i <= Param; i++)
		{
			/* Only print a comma after the first term in sequence */
			printf("%c ", i ? ',' : 0 );
			printf("%d", Fib[i]);
		}
		printf("\n");
	}
}
