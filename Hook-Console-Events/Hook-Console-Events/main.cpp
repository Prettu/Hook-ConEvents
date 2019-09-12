/*

~ Hook Events ~

*/

#include <Windows.h>
#include <stdio.h>

#define MAX_BUFFER_SIZE 128

DWORD fdwOldMode;
HANDLE hStdIn;

VOID PrintError(LPCSTR lpszErrorFunc)
{
	fprintf_s(stderr, "%s failed with the error code %d", lpszErrorFunc, GetLastError());
	SetConsoleMode(hStdIn, fdwOldMode);
	ExitProcess(0);
}

VOID HookKeyProc(KEY_EVENT_RECORD ker)
{
	if (ker.bKeyDown)
		fprintf_s(stdout, "Keyboard tapped HOOK\n");
}

VOID HookResizeProc(WINDOW_BUFFER_SIZE_RECORD wbsr)
{
	//fprintf_s(stdout, "Resize HOOK\n");
	fprintf_s(stdout, "Rows = %d, Columns = %d\n", wbsr.dwSize.X, wbsr.dwSize.Y);
}

int main(VOID)
{
	hStdIn = GetStdHandle(STD_INPUT_HANDLE);

	if (hStdIn == INVALID_HANDLE_VALUE)
		PrintError("GetStdHandle");

	INPUT_RECORD irInputBuffer[MAX_BUFFER_SIZE];
	DWORD fdwMode, c = 0, nEvents = 0, i = 0;

	fdwOldMode = GetConsoleMode(hStdIn, &fdwOldMode);

	if (!fdwOldMode)
		PrintError("GetConsoleMode");

	fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;

	if (!SetConsoleMode(hStdIn, fdwMode))
		PrintError("SetConsoleMode");

	// Process only 10 events
	while (c++ <= 10)
	{
		if (!ReadConsoleInput(hStdIn, irInputBuffer, MAX_BUFFER_SIZE, &nEvents))
			PrintError("ReadConsoleInput");

		for (i = 0; i < nEvents; i++)
		{
			switch (irInputBuffer[i].EventType)
			{
			case KEY_EVENT:
				HookKeyProc(irInputBuffer[i].Event.KeyEvent);
				break;
			case WINDOW_BUFFER_SIZE_EVENT:
				HookResizeProc(irInputBuffer[i].Event.WindowBufferSizeEvent);
				break;
			}
		}
	}

	SetConsoleMode(hStdIn, fdwMode);
	return 0;
}