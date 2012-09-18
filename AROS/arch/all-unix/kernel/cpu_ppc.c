#include <signal.h>

#include "kernel_base.h"
#include "kernel_intern.h"


struct SignalTranslation const sigs[] = {
    {SIGILL   ,  4,  7},
    {SIGTRAP  ,  9, 13},
    {SIGBUS   ,  2,  3},
    {SIGFPE   , 11,  7},
    {SIGSEGV  ,  2,  3},
#if defined(HOST_OS_linux) || defined(HOST_OS_arix)
    {SIGSTKFLT, 14,  3},
#endif
    {-1       , -1, -1}
};
