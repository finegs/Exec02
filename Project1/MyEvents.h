#ifndef _EVENTS_H
#define _EVENTS_H

#include <stdlib.h>

#include "MyList.h"
#include "MyQueue.h"
#include "MyEvent.h"

#ifdef __cpluscplus
extern "C" {
#endif

	int receive_event(Queue *queue, const Event *event);
	int process_event(Queue *queue, int(*dispatch)(Event *event));

#ifdef __cpluscplus
}
#endif

#endif