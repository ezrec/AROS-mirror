/**************************
 *
 * schedul_fx.c
 *
 * things dealing with time.
 * corTeX / Optimum ~ 1998
 *
 **************************/

#include <exec/types.h>

#include "schedul_fx.h"

unsigned long int frame_count;

static void timerclear(struct timeval *tv)
{
  tv->tv_sec = 0;
  tv->tv_usec = 0;
}

void opti_sched_init(){
  gettimeofday( &demo_start_time, NULL);
  sequence_start_time = 
           frame_time = 
    demo_current_time = demo_start_time;

  timerclear( &demo_elapsed_time);
  timerclear( &frame_elapsed_time);
  frame_count = 0;
}
void opti_sched_update(){
  gettimeofday( &demo_current_time, NULL);
  opti_diff_timeval( &demo_start_time,
		     &demo_current_time,
		     &demo_elapsed_time);

  opti_diff_timeval( &sequence_start_time,
		     &demo_current_time,
		     &sequence_elapsed_time);
} 
                               
void opti_sched_nextframe(){
  struct timeval frame_tmp;
  
  gettimeofday( &frame_tmp, NULL);
  opti_diff_timeval(&frame_time,
		    &frame_tmp,
		    &frame_elapsed_time);
  frame_time=frame_tmp;
  frame_count++;

}
void opti_sched_nextsequence(){
  gettimeofday( &sequence_start_time, NULL);
  timerclear (&sequence_elapsed_time);
  frame_count = 0;
}


float opti_scale_time(float secscale,struct timeval*elapsed){
  return (secscale * elapsed->tv_sec + secscale * elapsed->tv_usec / 1000000);
}

void  opti_diff_timeval(struct timeval*old, struct timeval*new,
			    struct timeval*dst){
  dst->tv_sec = new->tv_sec - old->tv_sec; 
  if (new->tv_usec > old->tv_usec)
    dst->tv_usec = new->tv_usec - old->tv_usec; 
  else {
    dst->tv_usec = 1000000+ new->tv_usec - old->tv_usec; 
    dst->tv_sec--;
  }
}

