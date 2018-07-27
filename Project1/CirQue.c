#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

#define SIZE 8

typedef struct CirQue_ {
	int *data;
	int front;
	int rear;
	int count;
	int capacity;

	int(*init)(struct CirQue_ *que, int capacity);
	int(*dispose)(struct CirQue_ *que);
	int(*insert)(struct CirQue_ *que, const int newData);
	int(*delete)(struct CirQue_ *que, int* data);
	int(*print)(struct CirQue_ *que);
} CirQue;

#define cirque_rear(que) ((que)->rear)
#define cirque_front(que) ((que)->front)
#define cirque_count(que) ((que)->count)
#define cirque_capacity(que) ((que)->capacity)
#define cirque_data(que, idx) (*((que)->data+idx))

int initCirQue(CirQue *que, int capacity) {
	if (NULL == (que->data = (int*)malloc(sizeof(int)*capacity))) { return -1; }
	return EXIT_SUCCESS;
}

int disposeCirQue(CirQue *que) {
	if (!que) return -1;
	free(que->data);
	free(que);
	return EXIT_SUCCESS;
}

int insertCirQue(CirQue *que, int newData) {
	if (que->count == que->capacity) {
		return -1;
	}
	else {
		*(que->data + que->rear) = newData;
		que->rear = (que->rear + 1) % que->capacity;
		que->count++;
		return 0;
	}
}

int deleteCirQue(CirQue *que, int *data) {
	if (cirque_count(que) == 0) {
		return -1;
	}
	else {
		*data = *(que->data + que->front);
		que->front = (que->front + 1) % que->capacity;
		que->count--;
		return EXIT_SUCCESS;
	}
}

int printCirQue(CirQue *que) {
	int i, j;
	if (cirque_count(que) < 1) {
		printf("queue size=%d\n", cirque_count(que));
		return -1;
	}
	else {
		printf("queue size=%d\n", cirque_count(que));
		j = cirque_count(que);
		for (i = cirque_front(que); j != 0; j--) {
			printf("%d%s", (int)cirque_data(que, i), j > 1 ? ", " : "");
		}
		printf("\n");
	}
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

int main(int argc, char* argv[]) {
	int choice;
	int run;

	CirQue que;

	que.init = initCirQue;
	que.dispose = disposeCirQue;
	que.insert = insertCirQue;
	que.delete = deleteCirQue;
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
			int i,n,d;
			printf("Enter number to delete\n");
			scanf("%d", &n);
			for (i = 0; i < n; i++)
			{
				que.delete(&que, &d);
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