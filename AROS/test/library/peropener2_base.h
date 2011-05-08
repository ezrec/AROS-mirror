#include <exec/libraries.h>

struct PerOpener2Base
{
    struct Library lib;
    
    struct Library *PeropenerBase;
};
