#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>

#include "Timer.h"


double CurrentTime(void)
{
     struct timeval tv;
     struct timezone tz;

     tz.tz_minuteswest = 0;
     tz.tz_dsttime = DST_NONE;
     gettimeofday(&tv, &tz);

     return (double) tv.tv_sec + (double) tv.tv_usec / 1000000.0;
}
