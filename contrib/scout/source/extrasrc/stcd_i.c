#include "extra.h"
#include <stdlib.h>

int stcd_i(const char *in, int *value)
{
    if (in) {
        char *ptr;

        switch (*in) {
            case '+':
            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                *value = strtol(in, &ptr, 10);
                return ptr - in;
                break;
        }
    }

    *value = 0;

    return 0;
}
