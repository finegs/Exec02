#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "List.h"

void list_init(List *list, void(*destroy)(void *data)) {
	list->size = 0;
	list->destroy = destroy;
	list->head = list->tail = NULL;
	return;
}

void list_destroy(List *list) {
	void *data;

	while (list_size(list) > 0) {
		if (list_rem_next(list, NULL, (void**)&data) == 0 
			&& list->destroy != NULL) {
			list->destroy(data);
		}
	}

	list->head = NULL;
	list->tail = NULL;

	//memset(list, 0, sizeof(List));

	return;
}


int list_ins_next(List *list, ListElmt *element, const void *data) {
	ListElmt *newElement;

	if ((newElement = (ListElmt*)malloc(sizeof(ListElmt))) == NULL) {
		return -1;
	}

	newElement->data = (void*)data;
	if (element == NULL) {
		if (list_size(list) == 0) {
			list->tail = newElement;
		}

		newElement->next = list->head;
		list->head = newElement;
	}
	else {
		if (element->next == NULL) {
			list->tail = newElement;
		}
		newElement->next = element->next;
		element->next = newElement;
	}

	list->size++;
	return 0;
}


int list_rem_next(List *list, ListElmt *element, void **data) {
	ListElmt *oldElement;

	if (list_size(list) == 0) return -1;

	if (element == NULL) {
		*data = list->head->data;
		oldElement = list->head;
		list->head = list->head->next;

		if (list_size(list) == 1) {
			list->tail = NULL;
		}
	}
	else {
		if (element->next == NULL) return -1;

		*data = element->next->data;
		oldElement = element->next;
		element->next = element->next->next;

		if (element->next == NULL) {
			list->tail = element;
		}
	}

	oldElement->data = NULL;
	oldElement->next = NULL;
	free(oldElement);

	list->size--;

	return 0;
}

int list_prt(List *list, char m) {
	int i;
	ListElmt *element;
	if (!list) return -1;
	if (list_size(list) < 1) {
		printf("list is empty\n");
		return 0;
	}

	switch (m) {
	case '1':
	{
		for (i = 0, element = list_head(list); element; i++, element = element->next) {
			printf("list[%d]=\"%s\"\n", i, (char*)element->data);
		}
	}
	break;
	case '2':
	{
		for (i = 0, element = list_head(list); element; i++, element = element->next) {
			printf("idx=%d\ndata=\"%s\"\nlen=%d%s", i, (char*)element->data, (int)strlen((char*)element->data), (element->next ? "\n\n" : "\n"));
		}
	}
	break;
	default:
		printf("Unsupported mode : %c", m);
		break;
	}
	return 0;
}

int list_info(List *list, char m) {
	if (!list) {
		printf("list is NULL\n");
		return -0;
	}

	printf("sizeof(list)=%d\n", list_size(list));
	return 0;
}