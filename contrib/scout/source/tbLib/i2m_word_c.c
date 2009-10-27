#include <exec/types.h>

UWORD i2m_word( UWORD d )
{
    return (UWORD)(((d & 0x00ff) << 8) | ((d & 0xff00) >> 8));
}

