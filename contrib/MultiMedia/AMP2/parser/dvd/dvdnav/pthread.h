/*
 * pthread.h
 */

#ifndef PTHREAD_H
#define PTHREAD_H

typedef unsigned long pthread_mutex_t;

#define pthread_mutex_init(a, b)
#define pthread_mutex_destroy(a)
#define pthread_mutex_lock(a)
#define pthread_mutex_unlock(a)

/* FIXME: Doesn't belong here */

#define snprintf(err_str, max_len, format, args...) sprintf(err_str, format, ## args);

#endif
