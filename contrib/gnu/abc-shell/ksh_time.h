#ifndef KSH_TIME_H
# define KSH_TIME_H

/* Wrapper around the ugly time.h,sys/time.h includes/ifdefs */
/* $Id$ */

#include <sys/time.h>
#include <time.h>

#ifndef CLK_TCK
# define CLK_TCK 60                     /* 60HZ */
#endif
#endif /* KSH_TIME_H */
