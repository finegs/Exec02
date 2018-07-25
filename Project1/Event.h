#ifndef _EVENT_H
#define _EVENT_H

#include <stdlib.h>

#include "List.h"
#include "Queue.h"

#ifdef __cpluscplus
extern "C" {
#endif

	typedef struct Event_ {
		void *data;
	} Event;


#ifdef __cpluscplus
}
#endif


#endif