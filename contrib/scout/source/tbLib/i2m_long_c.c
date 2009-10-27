#include <exec/types.h>

ULONG i2m_long( ULONG d )
{
    return (ULONG)(((d & 0xff000000) >> 24) |
                   ((d & 0x00ff0000) >> 8) |
                   ((d & 0x0000ff00) << 8) |
                   ((d & 0x000000ff) << 24));
}

