#ifndef SCHEDULE_FX_H
#define SCHEDULE_FX_H


/*
 * somes things to schedule the demo (timers,
 * scheduling tables, function...)
 */


#include <sys/time.h>
#include <unistd.h>


  
struct timeval   demo_start_time;   /* self-describing name... */
struct timeval   demo_current_time; /* idem */
struct timeval   demo_elapsed_time; /* duration since demo begining, used */
                                    /* for sequences transitions */


struct timeval    frame_time;        /* time of last frame computation ending */
struct timeval    frame_elapsed_time;/* duration between the two frames */

extern unsigned long int frame_count;    /* number of frames in sequence */
struct timeval    sequence_start_time;   /* start-date of current sequence */
struct timeval    sequence_elapsed_time; /* duration since sequence begining */


/* update functions */

void      opti_sched_init();    /* sets  demo_start_time & frame_time */
void      opti_sched_update();  /* update demo_current_time, demo_elapsed_time, */
                                /* & sequence_elapsed_time */
void      opti_sched_nextframe(); /* update frame_time & frame_elapsed_time & frame_count */
void      opti_sched_nextsequence(); /* sets sequence_start_time & frame_count=0 */

/* utility functions */

/* converts a delta-time into a float, used for parametric
   functions based on real-time.
   simply returns secscal * (time.tv_sec + time.tv_usec/10^6)
*/
float     opti_scale_time(float secscale,struct timeval*elapsed);  

/* compute time difference */
void      opti_diff_timeval(struct timeval*old, struct timeval*new,
			    struct timeval*dst);

#endif
