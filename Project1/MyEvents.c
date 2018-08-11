#include <stdlib.h>
#include <string.h>

#include "MyEvent.h"
#include "MyEvents.h"
#include "MyQueue.h"

int receive_event(Queue *queue, const Event *event) {
	Event *newEvent;

	if ((newEvent = (Event*)malloc(sizeof(Event))) == NULL) {
		return -1;
	}

	memcpy(newEvent, event, sizeof(Event));

	if (queue_enqueue(queue, newEvent) != 0) {
		return -1;
	}
	return 0;
}


int process_event(Queue *queue, int(*dispatch)(Event *event)) {
	Event *event;

	if (queue_size(queue) < 1) return -1;
	else {
		if (queue_dequeue(queue, (void**)&event) != 0) {
			return -1;
		}
		else {
			dispatch(event);
			free(event);
		}
	}

	return 0;
}