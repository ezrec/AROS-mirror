#include "symbols.y"

/** serial.c

            Written by Stephen Vermeulen (403) 282-7990

            PO Box 3295, Station B,
            Calgary, Alberta, CANADA, T2M 4L8

    various serial support routines
 **/

/** open the needed libraries **/

open_libs()
{
  IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 0L);
  GfxBase = (struct GfxBase *) OpenLibrary("graphics.library", 0L);
  if (!GfxBase || !IntuitionBase) return(FALSE);
  return(TRUE);
}

close_libs()
{
  if (IntuitionBase) CloseLibrary(IntuitionBase);
  if (GfxBase)       CloseLibrary(GfxBase);
}

/** open up a serial port of given name and unit, returns a pointer to
    a MySer structure if successful, NULL if not.
 **/

struct MySer *open_ser(name, unit)
char *name;
int unit;
{
  struct MySer *ms;

  ms = (struct MySer *) AllocMem(SIZE(MySer), MEMF_CLEAR);
  if (ms)
  {
    if (ms->readport = CreatePort(NULL, 0L))
    {
      if (ms->writeport = CreatePort(NULL, 0L))
      {
        if (ms->readio = (struct IOExtSer *)
            CreateExtIO(ms->readport, SIZE(IOExtSer)) )
        {
          if (ms->writeio = (struct IOExtSer *)
             CreateExtIO(ms->writeport, SIZE(IOExtSer)) )
          {
            if (!OpenDevice(name, (long) unit, ms->readio, 0L))
            {
              ms->writeio->IOSer.io_Device = ms->readio->IOSer.io_Device;
              ms->writeio->IOSer.io_Unit = ms->readio->IOSer.io_Unit;
              return(ms);
            }
            DeleteExtIO(ms->writeio, SIZE(IOExtSer));
          }
          DeleteExtIO(ms->readio, SIZE(IOExtSer));
        }
        DeletePort(ms->writeport);
      }
      DeletePort(ms->readport);
    }
    FreeMem(ms, SIZE(MySer));
  }
  return(NULL);
}

close_ser(ms)
struct MySer *ms;
{
  if (ms)
  {
    CloseDevice(ms->readio);
    DeleteExtIO(ms->writeio, SIZE(IOExtSer));
    DeleteExtIO(ms->readio, SIZE(IOExtSer));
    DeletePort(ms->writeport);
    DeletePort(ms->readport);
    FreeMem(ms, SIZE(MySer));
  }
}
