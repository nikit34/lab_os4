#include "stdio.h"
#include "windows.h"
#include <conio.h>
#include <tchar.h>


HANDLE hEventChar;
HANDLE hEventTermination;
HANDLE hEvents[2];

CHAR lpEventName[] = "nameSync";
CHAR lpEventTerminationName[] = "nameEnd";
CHAR lpFileShareName[] = "nameFile";

HANDLE hFileMapping;

LPVOID lpFileMap;

int main() {
    CHAR chr;
    int count_request = 0;
    int num = 0;
    int ch_end = 0;
    int x, result;

    DWORD dwRetCode;

    hEventChar = CreateEvent(NULL, FALSE, FALSE, lpEventName);

    if (hEventChar == NULL) {
        fprintf(stdout, "CreateEvent: Error %ld\n", GetLastError());
        _getch();
        return 0;
    }

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        printf("\nApplication EVENT already started\n"
            "Press any key to exit...");
        _getch();
        return 0;
    }

    hEventTermination = CreateEvent(NULL, FALSE, FALSE, lpEventTerminationName);

    if (hEventTermination == NULL) {
        fprintf(stdout, "CreateEvent (Termination): Error %ld\n", GetLastError());
        _getch();
        return 0;
    }

    hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 256, lpFileShareName);

    if (hFileMapping == NULL) {
        fprintf(stdout, "CreateFileMapping: Error %ld\n", GetLastError());
        _getch();
        return 0;
    }

    lpFileMap = MapViewOfFile(hFileMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

    if (lpFileMap == 0) {
        fprintf(stdout, "MapViewOfFile: Error %ld\n", GetLastError());
        _getch();
        return 0;
    }

    hEvents[0] = hEventTermination;
    hEvents[1] = hEventChar;

    while (TRUE) {
        chr = _getche();

        if (chr == 0x20 || chr == 0X0A) {
            x = num;

            *((LPSTR)lpFileMap) = x;

            dwRetCode = WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);

            if (dwRetCode == WAIT_ABANDONED_0 ||
                dwRetCode == WAIT_ABANDONED_0 + 1 ||
                dwRetCode == WAIT_OBJECT_0 ||
                dwRetCode == WAIT_FAILED)
                break;

            if (count_request != 0) {
                result = *((LPSTR)lpFileMap);
                if ((char)result != 'E') {
                    printf("Result call %d: %d\n", count_request, result);
                } else {
                    printf("Error from child proccess\n");
                }
            }
            count_request++;

            if (chr == 0x20) {
                num = 0;
                ch_end = 0;
                continue;
            }


            if (chr == 0x0A) {
                if (ch_end == 2)
                    break;
                ++ch_end;
            }
        }
        num = 10 * num + (chr - '0');
    }
    printf("Finish calc\n");

    CloseHandle(hEventChar);
    CloseHandle(hEventTermination);
    UnmapViewOfFile(lpFileMap);
    CloseHandle(hFileMapping);

    return 0;
}
