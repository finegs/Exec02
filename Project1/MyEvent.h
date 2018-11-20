#ifndef _EVENT_H
#define _EVENT_H

#include <stdlib.h>

#include "MyList.h"
#include "MyQueue.h"

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