/*
    Copyright © 2014, The AROS Development Team.
    All rights reserved.
    
    $Id$
*/

#define NOXADPROTOS
#define DEBUG 1

#include <exec/types.h>
#include <exec/libraries.h>
#include <aros/libcall.h>
#include <aros/symbolsets.h>

#include <proto/dos.h>

#include LC_LIBDEFS_FILE

#include <aros/debug.h>

#include "privdefs.h"
#include "functions.h"
#include "version.h"

extern const struct xadClient * const RealFirstClient;

/****************************************************************************************/

static void InitClients(LIBBASETYPEPTR LIBBASE)
{
  struct DosLibrary *DOSBase = XADB(LIBBASE)->xmb_DOSBase;
  struct FileInfoBlock *fib;

  if((fib = (struct FileInfoBlock *) AllocDosObject(DOS_FIB, 0)))
  {
    BPTR lock;

    D(bug("[xadmaster.library]: %s()\n", __PRETTY_FUNCTION__));

    if((lock = Lock("LIBS:XAD", SHARED_LOCK)))
    {
      if(Examine(lock, fib))
      {
        if(fib->fib_DirEntryType > 0)
        {
          BPTR oldlock;

          oldlock = CurrentDir(lock);

          while(ExNext(lock, fib))
          {
            BPTR seg;
            BPTR *sl;

            if (seg = LoadSeg(fib->fib_FileName))
            {
              xadBOOL need = XADFALSE;
              sl = (BPTR *)BADDR(seg);
#if !defined(__AROS__)
              {
                xadUINT32 i, size = *((xadUINT32 *) (sl-4))-16; /* 16 --> xadForman size */

                /* scan the first segment for a forman structure */
                for(i = 0; !need && i < size; i += 2)
                {
                  /* divided into 2 parts to disable recognition of
                    xadmaster.library as client */
                  if((*((xadUINT16 *) (sl+4+i)) == 0x5841) &&
                  (*((xadUINT16 *) (sl+6+i)) == 0x4446))
                  {
                    const struct xadClient *xcl;

                    xcl = ((struct xadForeman *) (sl+4+i-4))->xfm_FirstClient;
                    need = xadAddClients(LIBBASE, xcl, XADCF_EXTERN);
                  }
                }
              }
#else
              while(seg)
              {
                STRPTR addr = (STRPTR)((IPTR)BADDR(seg) - sizeof(ULONG));
                ULONG size = *(ULONG *)addr;

                for(addr += sizeof(xadUINT32), size -= sizeof(xadUINT32);
                   size >= sizeof(struct xadForeman);
                   size -= 2, addr += 2)
                {
                  struct xadForeman *xcFM = (struct xadForeman *)addr;
                  if ( xcFM->xfm_ID == XADFOREMAN_ID )
                  {
                    if (xadAddClients(LIBBASE, xcFM->xfm_FirstClient, XADCF_EXTERN))
                      need = XADTRUE;
                  }
                }
                seg = *(BPTR *)BADDR(seg);
              }
#endif
              if(!need)
                UnLoadSeg(MKBADDR(sl));
              else /* add this seglist */
              {
                BPTR *sl2 = sl;

                while (*sl2)
                  sl2 = BADDR(*sl2);

                *sl2 = XADB(LIBBASE)->xmb_ClientSegList;
                XADB(LIBBASE)->xmb_ClientSegList = MKBADDR(sl);
              }
            } /* LoadSeg */
          } /* ExNext */
          SetIoErr(0);
          CurrentDir(oldlock);
        } /* Is Directory ? */
      } /* Examine */
      UnLock(lock);
    } /* Lock */
    FreeDosObject(DOS_FIB, fib);
  } /* AllocDosObject */
}

static int Init(LIBBASETYPEPTR LIBBASE)
{
    D(bug("[xadmaster.library]: %s()\n", __PRETTY_FUNCTION__));

    XADB(LIBBASE)->xmb_DefaultName = "unnamed.dat";

    XADB(LIBBASE)->xmb_InHookFH.h_Entry = ( HOOKFUNC )InHookFH;
    XADB(LIBBASE)->xmb_OutHookFH.h_Entry = ( HOOKFUNC )OutHookFH;
    XADB(LIBBASE)->xmb_InHookMem.h_Entry = ( HOOKFUNC )InHookMem;
    XADB(LIBBASE)->xmb_OutHookMem.h_Entry = ( HOOKFUNC )OutHookMem;
    XADB(LIBBASE)->xmb_InHookStream.h_Entry = ( HOOKFUNC )InHookStream;
    XADB(LIBBASE)->xmb_OutHookStream.h_Entry = ( HOOKFUNC )OutHookStream;
    XADB(LIBBASE)->xmb_InHookDisk.h_Entry = ( HOOKFUNC )InHookDisk;
    XADB(LIBBASE)->xmb_OutHookDisk.h_Entry = ( HOOKFUNC )OutHookDisk;
    XADB(LIBBASE)->xmb_InHookSplitted.h_Entry = ( HOOKFUNC )InHookSplitted;
    XADB(LIBBASE)->xmb_InHookDiskArc.h_Entry = ( HOOKFUNC )InHookDiskArc;

    XADB(LIBBASE)->xmb_FirstClient = NULL;
    XADB(LIBBASE)->xmb_System.xsi_Version = XADMASTERVERSION;
    XADB(LIBBASE)->xmb_System.xsi_Revision = XADMASTERREVISION;

    if((XADB(LIBBASE)->xmb_DOSBase = (struct DosLibrary *)OpenLibrary("dos.library", 37)))
    {
	if((XADB(LIBBASE)->xmb_UtilityBase = (struct UtilityBase *) OpenLibrary("utility.library", 37)))
	{
	    xadSize minsize = 0;
	    const struct xadClient *cl;

	    /* add internal clients */
	    xadAddClients(LIBBASE, RealFirstClient, 0);
	    /* load and add external clients */
	    InitClients(LIBBASE);

	    for(cl = XADB(LIBBASE)->xmb_FirstClient; cl; cl = cl->xc_Next)
	    {
		if(cl->xc_RecogSize > minsize)
		    minsize = cl->xc_RecogSize;
	    }
	    XADB(LIBBASE)->xmb_RecogSize = minsize; /* obsolete, compatibility only */
	    XADB(LIBBASE)->xmb_System.xsi_RecogSize = minsize;

	    MakeCRC16(XADB(LIBBASE)->xmb_CRCTable1, XADCRC16_ID1);
	    MakeCRC32(XADB(LIBBASE)->xmb_CRCTable2, XADCRC32_ID1);
	}
    }
    return TRUE;
}

/****************************************************************************************/

static int Expunge(LIBBASETYPEPTR LIBBASE)
{
    struct DosLibrary *DOSBase = XADB(LIBBASE)->xmb_DOSBase;

    D(bug("[xadmaster.library]: %s()\n", __PRETTY_FUNCTION__));

    xadFreeClients(XADB(LIBBASE));

    if(XADB(LIBBASE)->xmb_ClientSegList)
        UnLoadSeg(XADB(LIBBASE)->xmb_ClientSegList);

    if(XADB(LIBBASE)->xmb_UtilityBase)
        CloseLibrary((struct Library *)XADB(LIBBASE)->xmb_UtilityBase);
    if(XADB(LIBBASE)->xmb_DOSBase)
        CloseLibrary((struct Library *)XADB(LIBBASE)->xmb_DOSBase);

    return TRUE;
}

ADD2INITLIB(Init, 0);
ADD2EXPUNGELIB(Expunge, 0);
