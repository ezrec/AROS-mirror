
#include <exec/libraries.h>
#include <devices/timer.h>
#include <exec/memory.h>
#include <proto/timer.h>
#include <exec/io.h>
#include <dos/dos.h>
#include <proto/exec.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "timer.h"

static int timer_initialized = 0;

struct Library *TimerBase = NULL;
static struct timerequest *timeio;

static struct timeval tv0, tv1;
static struct timeval gtv0, gtv1;

// autoinitialization/autotermination functions

void TimerInit(void)
{
    if (timer_initialized == 0)
    {
        //printf("Timer base initialized\n");

        timeio = AllocVec(sizeof(struct timerequest), MEMF_CLEAR | MEMF_PUBLIC | MEMF_ANY);
        OpenDevice(TIMERNAME, UNIT_MICROHZ, (struct IORequest *) timeio, 0);
        TimerBase = (void*) (timeio->tr_node.io_Device);

        timer_initialized = 1;

		atexit(&TimerEnd);
    }
}

void TimerEnd(void)
{
    if (timer_initialized)
    {
        if (timeio) CloseDevice((struct IORequest*) timeio);
        FreeVec(timeio);
        timer_initialized = 0;
    }
}

void __timerGStart(void)
{
    GetSysTime(&tv0);
}

void __timerGlobalStart(void)
{
    GetSysTime(&gtv0);
}

double __timerGetTime(void)
{
    double	t;
    GetSysTime(&tv1);

    t = (double) tv1.tv_secs;
    t += (double) tv1.tv_micro / 1000000.0;

    return t;
}

void __timerGetSysTime(struct timeval *tv)
{
    GetSysTime(tv);
}

float __timerGGet(float fps)
{
	double time0, time1, time;

    GetSysTime(&tv1);

    time0 = (double) tv0.tv_secs;
    time0 += (double) tv0.tv_micro / 1000000.0;

    time1 = (double) tv1.tv_secs;
    time1 += (double) tv1.tv_micro / 1000000.0;

    time = time1 - time0; 		// in seconds
    time = time * fps;

    return time;

}

float __timerGlobalGet(float fps)
{
    double	time0, time1, time;

    //t1=(float)clock();
    //return  (float)((t1-t0)*fps/(float)CLOCKS_PER_SEC);

    GetSysTime(&gtv1);

    time0 = (double) gtv0.tv_secs;
    time0 += (double) gtv0.tv_micro / 1000000.0;

    time1 = (double) gtv1.tv_secs;
    time1 += (double) gtv1.tv_micro / 1000000.0;

    time = time1 - time0; 		// in seconds
    time = time * fps;

    return time;
}

