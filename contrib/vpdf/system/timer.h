
#ifndef	__TIMER_H
#define	__TIMER_H

#include <devices/timer.h>

//GLOBAL struct Library *TimerBase;

void TimerInit(void);
void TimerEnd(void);

void __timerGStart(void);
float __timerGGet(float fps);

void __timerGlobalStart(void);
float __timerGlobalGet(float fps);

double __timerGetTime(void);
void __timerGetSysTime(struct timeval *tv);

// PPC only (used in anim recording)
void __timerAdvanceFrame(void);
void __timerIndependentTiming(float fps);

#endif
