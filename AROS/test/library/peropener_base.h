#include <exec/libraries.h>

struct PerOpenerBase
{
    struct Library lib;
    
    IPTR testvalue;
};
