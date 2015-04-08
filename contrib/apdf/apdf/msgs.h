#ifndef MSGS_H
#define MSGS_H

const char *getString(int);
const char *getKString(int);
char getKey(int);

#define STR(x)  getString(x)
#define KSTR(x) getKString(x)
#define KEY(x)  getKey(x)

#define CATCOMP_NUMBERS
#include "apdf_msgs.h"

#endif

