#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

#include "MyCircleQueue.h"

int displayMenu();

int main(int argc, char* argv[]) {
    int choice;
    int run;

    CirQue que;

    que.init = initCirQue;
    que.dispose = disposeCirQue;
    que.insert = insertCirQue;
    que.remove = removeCirQue;
    que.print = printCirQue;

    // init
    que.init(&que, 100);

    run = 1;

    do {
        choice = displayMenu();
        switch (choice) {
        case 1: // insert
        {
            int i;
            printf("Enter number to insert\n");
            scanf("%d", &i);
            que.insert(&que, i);
        }
        break;
        case 2: // delete
        {
            int i, n, d;
            printf("Enter number to delete\n");
            scanf("%d", &n);
            for (i = 0; i < n; i++)
            {
                que.remove(&que, &d);
                printf("Removed data[%d]=%d\n", i, d);
            }
        }
        break;
        case 3: // print
        {
            que.print(&que);
        }
        break;
        case 4: // exit
            run = 0;
            break;
        default:
            printf("\nInvalid Choice. Please enter again.\n");
        }
    } while (run);

    que.dispose(&que);

    return EXIT_SUCCESS;
}


int displayMenu() {
    int choice;

    printf("\n1(Insert Data)");
    printf(", 2(Delete Data)");
    printf(", 3(Display Data)");
    printf(", 4(Quit the Program)");
    printf("\nEnter your choice:");
    scanf("%d", &choice);
    return choice;
}

