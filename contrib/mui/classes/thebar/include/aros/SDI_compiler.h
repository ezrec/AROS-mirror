#ifndef SDI_COMPILER_H
#define SDI_COMPILER_H

#include <exec/types.h>

#define VARARGS68K __stackparm
#define UNUSED __attribute__((unused)) /* for functions, variables and types */
#define USED   __attribute__((used))   /* for functions only! */

#endif
