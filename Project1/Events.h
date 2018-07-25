#ifndef _EVENTS_H
#define _EVENTS_H

#include <stdlib.h>

#include "List.h"
#include "Queue.h"
#include "Event.h"

#ifdef __cpluscplus
extern "C" {
#endif

	int receive_event(Queue *queue, const Event *event);
	int process_event(Queue *queue, int(*dispatch)(Event *event));

#ifdef __cpluscplus
}
#endif

#endif