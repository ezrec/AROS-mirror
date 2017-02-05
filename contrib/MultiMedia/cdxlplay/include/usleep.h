#ifndef USLEEP_H
#define USLEEP_H

#include <unistd.h>
#ifdef AROS
int setup_usleep(void);
void cleanup_usleep(void);
int usleep(useconds_t usec);
#else
#define setup_usleep() 1
#define cleanup_usleep() 
#endif

#endif
