/*
**  AmigaOS specific
*/

#ifdef AMIGA

#include <exec/types.h>
#include <dos/dos.h>
#include <proto/dos.h>

LONG mkdir(STRPTR path, UWORD perm)
{
  BPTR thelock;

  if (thelock = CreateDir(path))
  {
    UnLock(thelock);
    return(0);
  }
  return(-1);
}

#endif /* AMIGA */
