#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>

#define MAX_THREADS 32

// The function getrandom returns a random number between min and max, which must be in integer range.
#define getrandom( min , max ) (SHORT)((rand() % (int)(((max) +1 ) - (min))) + (min))

int main(int argc, char* argv[]); // Thread 1 : main
void readInput();				 // Keyboard input, thread dispatch
void bounceProc(void* MyID);     // Thread 2 to n : display
void clearScreen();				 // Screen Clear
void stopThread();               // stopThread
void shutDownAll();              // Program shutdown
void writeTitle(int threadNum);  // Display title bar information

HANDLE hConsoleOut;                 // Handle to the console
HANDLE hRunMutex;                   // "Keep Running" mutex
HANDLE hScreenMutex;                // "Screen Update" mutex
int    threadNr;                    // Number of threads started
int    threadDeleteNr;              // Number of thread to Delete
CONSOLE_SCREEN_BUFFER_INFO csbiInfo; // Console Information

int main(int argc, char* argv[]) {

    hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsoleOut, &csbiInfo);
    clearScreen();
    writeTitle(0);

    hScreenMutex = CreateMutex(NULL, FALSE, NULL);
    hRunMutex = CreateMutex(NULL, TRUE, NULL);
    threadNr = 0;
    threadDeleteNr = 0;

    readInput();

    if (hScreenMutex)
        CloseHandle(hScreenMutex);
    if (hRunMutex)
        CloseHandle(hRunMutex);
    if (hConsoleOut)
        CloseHandle(hConsoleOut);

    return EXIT_SUCCESS;
}

void stopThread() { // stopThread threads
    while (threadDeleteNr > 0) {
        ReleaseMutex(hRunMutex);
        threadDeleteNr--;
    }
}

void shutDownAll() { // Shutdown all threads
    while (threadNr > 0) {
        ReleaseMutex(hRunMutex);
        threadNr--;
    }

    WaitForSingleObject(hScreenMutex, INFINITE);
    clearScreen();
}

void readInput() { // Dispatch and Count threads
    int keyInfo;

    do {
        keyInfo = _getch();
        if (tolower(keyInfo) == 'a' &&
            threadNr < MAX_THREADS) {
            threadNr++;
            _beginthread(bounceProc, 0, &threadNr);
            writeTitle(threadNr);
        }
        if (tolower(keyInfo) == 'd' && 
            threadNr > 0) {
            threadDeleteNr++;
            stopThread();
        }
    } while (tolower(keyInfo) != 'q');

    shutDownAll();
}

void bounceProc(void* pMyId) {
    char myCell, oldCell;
    WORD myAttrib, oldAttrb;
    char    blankCell = 0x20;
    COORD   coords, delta;
    COORD   old = { 0,0 };
    DWORD   dummy;

    char    *myId = (char*)pMyId;

    srand((unsigned int)*myId * 3);
    coords.X = getrandom(0, csbiInfo.dwSize.X - 1);
    coords.Y = getrandom(0, csbiInfo.dwSize.Y - 1);
    delta.X = getrandom(-3, 3);
    delta.Y = getrandom(-3, 3);

    if (*myId > 16) {
        myCell = 0x01;      // outline face
    }
    else {
        myCell = 0x02;      // solid face
    }

    myAttrib = *myId & 0x0F;    // force black background

    do {
        // Wait for display to be avaiable, then lock it
        WaitForSingleObject(hScreenMutex, INFINITE);

        // If we still occupy the old screen position, blank it out.
        ReadConsoleOutputCharacter(hConsoleOut, &oldCell, 1, old, &dummy);
        ReadConsoleOutputCharacter(hConsoleOut, &oldAttrb, 1, old, &dummy);
        if ((oldCell == myCell) && (oldAttrb == myAttrib)) {
            WriteConsoleOutputCharacter(hConsoleOut, &blankCell, 1, old, &dummy);
        }

        WriteConsoleOutputCharacter(hConsoleOut, &myCell, 1, coords, &dummy);
        WriteConsoleOutputCharacter(hConsoleOut, &myAttrib, 1, coords, &dummy);
        ReleaseMutex(hScreenMutex);

        // Increment the coordinates for next placement of the block.
        old.X = coords.X;
        old.Y = coords.Y;
        coords.X += delta.X;
        coords.Y += delta.Y;

        // If we are about to go off the screen, reverse direction
        if (coords.X < 0 || coords.X >= csbiInfo.dwSize.X) {
            delta.X = -delta.X;
            Beep(400, 50);
        }
        if (coords.Y < 0 || coords.Y>csbiInfo.dwSize.Y) {
            delta.Y = -delta.Y;
            Beep(600, 50);
        }
    }
    // Repeat while RunMutex is still taken.
    while (WaitForSingleObject(hRunMutex, 75L) == WAIT_TIMEOUT);
}

void writeTitle(int threadNum) {
    enum {
        sizeOfThreadMsg = 80
    };

    char NthreadMsg[sizeOfThreadMsg];

    sprintf_s(NthreadMsg, sizeOfThreadMsg, "Threads running: %02d, Press 'A' "
        "to start a thread, 'D' to stop a thread, 'Q' to quit.", threadNum);
    SetConsoleTitle(NthreadMsg);
}

void clearScreen() {
    DWORD   dummy;
    COORD   home = { 0, 0 };
    FillConsoleOutputCharacter(hConsoleOut, ' ', csbiInfo.dwSize.X * csbiInfo.dwSize.Y, home, &dummy);
}
