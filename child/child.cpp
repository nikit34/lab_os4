#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>


HANDLE hEvent;
HANDLE hEventTermination;

CHAR lpEventName[] = "nameSync";
CHAR lpEventTerminationName[] = "nameEnd";
CHAR lpFileShareName[] = "nameFile";

HANDLE hFileMapping;

LPVOID lpFileMap;

int main() {
    CHAR chr;
    int x, divider = 0;
    int count_run = 0;
    int res;

    DWORD dwRetCode;

    hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, lpEventName);

    if (hEvent == NULL) {
        fprintf(stdout, "OpenEvent: Error %ld\n", GetLastError());
        _getch();
        return 0;
    }

    hEventTermination = OpenEvent(EVENT_ALL_ACCESS, FALSE, lpEventTerminationName);

    if (hEventTermination == NULL) {
        fprintf(stdout, "OpenEvent (Termination): Error %ld\n", GetLastError());
        _getch();
        return 0;
    }

    hFileMapping = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, lpFileShareName);

    if (hFileMapping == NULL) {
        fprintf(stdout, "OpenFileMapping: Error %ld\n", GetLastError());
        _getch();
        return 0;
    }

    lpFileMap = MapViewOfFile(hFileMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

    if (lpFileMap == 0) {
        fprintf(stdout, "MapViewOfFile: Error %ld\n", GetLastError());
        _getch();
        return 0;
    }

    while (TRUE) {
        x = *((LPSTR)lpFileMap);

        if (count_run == 0) {
            divider = x;
        }
        if (count_run > 0) {
            if (divider != 0) {
                res = x / divider;
                *((LPSTR)lpFileMap) = res;
            }
            else {
                *((LPSTR)lpFileMap) = 'E';
                 //res = 0;
            }
        }
        count_run++;

        SetEvent(hEvent);
    }

    SetEvent(hEvent);
    SetEvent(hEventTermination);
    CloseHandle(hEvent);
    CloseHandle(hEventTermination);
    UnmapViewOfFile(lpFileMap);
    CloseHandle(hFileMapping);

    return 0;
}

 
//int main() {
//    while (true) {
//        if (count_run == 0) {
//            divider = x;
//        }
//
//        if (count_run > 0) {
//            if (divider != 0) {
//                res = x / divider;
//                str_res = std::to_string(res);
//                WriteFile(hFile, str_res.c_str(), str_res.size(), &writedBytes, NULL);
//                WriteFile(writeHandle, &res, sizeof(int), &writedBytes, NULL);
//            }
//            else {
//                WriteFile(writeHandle, "E", 1, &writedBytes, NULL);
//                //res = 0;
//            }
//        }
//        count_run++;
//    }
//    return 0;
//}