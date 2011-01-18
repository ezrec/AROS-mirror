/***************************************************************************

 Mailtext - An MUI Custom Class for eMessage display
 Copyright (C) 1996-2001 by Olaf Peters
 Copyright (C) 2002-2006 by Mailtext Open Source Team

 GCC & OS4 Portage by Alexandre Balaban

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 Mailtext OpenSource project :  http://sourceforge.net/projects/mailtext/

 $Id$

 Common functions and structures needed for an MUI (mcc) library.

***************************************************************************/

#define __USE_SYSBASE 1
#include <proto/exec.h>
#include <exec/resident.h>

#include <SDI_compiler.h>

#ifndef __AMIGADATE__
  #ifdef __GNUC__
    #define __AMIGADATE__	"(" __DATE__ ")"
  #endif
#endif /* __AMIGADATE__ */

#ifndef SUPERCLASS
  #define SUPERCLASS   MUIC_Mccprefs
  #define PREFSCLASS   1
  #ifndef ONLYGLOBAL
    #define ONLYGLOBAL FALSE
  #endif
#endif

#ifdef __amigaos4__
  #define GETINTERFACE(iface, base) (iface = (APTR)GetInterface((struct Library *)(base), "main", 1L, NULL))
  #define DROPINTERFACE(iface)      (DropInterface((struct Interface *)iface), iface = NULL)
  #define SYSCPU "OS4/PPC"
#else
  #define SYSCPU "OS3/68020"
#endif



static BOOL SAVEDS ASM  UserLibOpen   (REG(a6, const struct Library *const base) );
static BOOL SAVEDS ASM  UserLibClose  (REG(a6, const struct Library *const base));
static BOOL             ClassInitFunc (const struct Library *const base);
static VOID             ClassExitFunc (const struct Library *const base);
static ULONG SAVEDS ASM Dispatcher    (REG(a0, struct IClass *const cl), REG(a2, Object *const obj), REG(a1, const Msg msg));

#if defined(__amigaos4__)
  struct MUI_CustomClass * SAVEDS ASM MCC_Query(UNUSED struct Interface *Self, const LONG which);

  struct LibraryHeader * mtLibInit    (struct Library *base, BPTR segList, struct ExecIFace *ISys);
  uint32                 mtLibObtain  (struct Interface *Self);
  uint32                 mtLibRelease (struct Interface *Self);
  struct LibraryHeader * mtLibOpen    (struct LibraryManagerInterface *Self, UNUSED ULONG version);
  BPTR                   mtLibClose   (struct LibraryManagerInterface *Self);
  BPTR                   mtLibExpunge (struct LibraryManagerInterface *Self);
#else
  struct MUI_CustomClass * SAVEDS ASM MCC_Query(REG(d0, const LONG which));

  struct LibraryHeader * SAVEDS ASM mtLibInit(REG(a0, const BPTR Segment), REG(a6, struct ExecBase *sb));
  BPTR SAVEDS ASM                   mtLibExpunge(REG(a6, struct LibraryHeader *const base));
  struct LibraryHeader * SAVEDS ASM mtLibOpen(REG(a6, struct LibraryHeader *const base));
  BPTR SAVEDS ASM                   mtLibClose(REG(a6, struct LibraryHeader *const base));
  LONG SAVEDS ASM                   mtLibExtFunc(VOID);
#endif // __amigaos4__



LONG _start(void)
{
   return(RETURN_FAIL);
}

LONG SAVEDS ASM mtLibExtFunc(VOID)
{
  return(0);
}



const char mtLibName[] = CLASS;
const char mtLibID[] = "$VER: " CLASS " " VERSIONSTR " " __AMIGADATE__ " ©" COPYRIGHT " " AUTHOR " [" SYSCPU "]\n";

struct LibraryHeader
{
  struct Library         lh_Library;
  UWORD                  lh_Pad;
  BPTR                   lh_Segment;
  struct SignalSemaphore lh_Semaphore;
};

#if defined(__amigaos4__)

  struct MUIMasterIFace  *IMUIMaster     = NULL;
  struct ExecIFace       *IExec          = NULL;
  struct UtilityIFace    *IUtility       = NULL;
  struct DOSIFace        *IDOS           = NULL;
  struct GraphicsIFace   *IGraphics      = NULL;
  struct IntuitionIFace  *IIntuition     = NULL;

  static APTR lib_manager_vectors[] = {
  	  mtLibObtain,
  	  mtLibRelease,
  	  NULL,
  	  NULL,
  	  mtLibOpen,
  	  mtLibClose,
  	  mtLibExpunge,
  	  NULL,
  	  (APTR)-1,
  };

  static struct TagItem lib_managerTags[] = {
  	  { MIT_Name,	  (uint32)"__library"		  },
  	  { MIT_VectorTable,  (uint32)lib_manager_vectors },
  	  { MIT_Version,	  1						  },
  	  { TAG_END,	  0						  }
  };

  void *main_vectors[] = {
  	  (void *) mtLibObtain,
  	  (void *) mtLibRelease,
  	  (void *) NULL,
  	  (void *) NULL,
      (void *) MCC_Query,
      (void *) -1
  };

  static struct TagItem lib_mainTags[] = {
  	  { MIT_Name,	      (uint32)"main"		  },
  	  { MIT_VectorTable,  (uint32)main_vectors	  },
  	  { MIT_Version,	  1						  },
  	  { TAG_END,	      0						  }
  };

  static APTR libInterfaces[] = {
  	  lib_managerTags,
  	  lib_mainTags,
  	  NULL
  };

  extern uint32 VecTable68K[];

  static struct TagItem libCreateTags[] = {
  	  { CLT_DataSize,	  (uint32)sizeof(struct LibraryHeader) },
  	  { CLT_InitFunc,	  (uint32)mtLibInit				 },
  	  { CLT_Interfaces,	  (uint32)libInterfaces          },
      { CLT_Vector68K,    (uint32)VecTable68K            },
  	  { TAG_END,		  0	                             }
  };

#else // !__amigaos4__

 const APTR _LibFuncTab[] =
  {
   mtLibOpen,
   mtLibClose,
   mtLibExpunge,
   mtLibExtFunc,
   MCC_Query,
   (APTR)-1
  };
#endif

  static struct Resident USED_VAR _LibRomTag = {
  	  RTC_MATCHWORD,			  // rt_MatchWord
  	  &_LibRomTag,				  // rt_MatchTag
  	  &_LibRomTag+1,			  // rt_EndSkip
      #if defined(__amigaos4__)
  	  RTF_NATIVE | RTF_AUTOINIT,  // rt_Flags for OS4
      #else
      0,                          // rt_Flags for 68k
      #endif
  	  VERSION,					  // rt_Version
  	  NT_LIBRARY,				  // rt_Type
  	  0,						  // rt_Pri
  	  (char *)mtLibName,			  // rt_Name
  	  (char *)mtLibID,             // rt_IdString
      #if defined(__amigaos4__)
  	  libCreateTags               // rt_Init for OS4
      #else
      mtLibInit                    // rt_Init for 68k
      #endif
  };


 struct Library 	    *MUIClassBase   = NULL;
 struct Library 	    *MUIMasterBase  = NULL;
 struct MUI_CustomClass *ThisClass      = NULL;
 struct ExecBase	    *SysBase        = NULL;
 struct IntuitionBase	*IntuitionBase  = NULL;
 #if defined(__SASC) || defined(_DCC)
   struct Library	    *UtilityBase    = NULL;
 #else
   struct UtilityBase 	*UtilityBase    = NULL;
 #endif
 struct DosLibrary	    *DOSBase        = NULL;
 struct GfxBase 	    *GfxBase        = NULL;



#if !defined(__amigaos4__)

 #ifdef __MORPHOS__
   struct LibraryHeader * mtLibInit(struct LibraryHeader *base, BPTR SegList, struct ExecBase *sb)
   {
 #else
   struct LibraryHeader * ASM SAVEDS mtLibInit(REG(a0, BPTR SegList), REG(a6, struct ExecBase *sb))
   {
 #endif
   struct LibraryHeader *base;

   SysBase = sb;

   if (!(sb->AttnFlags & (AFF_68020 | AFF_68030 | AFF_68040 | AFF_68060)))
    {
     return(NULL);
    }

   base = (struct LibraryHeader *)MakeLibrary((APTR)_LibFuncTab,NULL,NULL,sizeof(struct LibraryHeader),NULL);
   if (base != NULL)
    {
     base->lh_Library.lib_Node.ln_Type = NT_LIBRARY;
     base->lh_Library.lib_Node.ln_Name = (char *)mtLibName;
     base->lh_Library.lib_Flags        = LIBF_CHANGED | LIBF_SUMUSED;
     base->lh_Library.lib_Version      = VERSION;
     base->lh_Library.lib_Revision     = REVISION;
     base->lh_Library.lib_IdString     = (char *)mtLibID;
     base->lh_Segment = SegList;
     InitSemaphore(&base->lh_Semaphore);
     AddLibrary((struct Library *)base);
    }
   return(base);
  }

#else // __amiagos4__

 struct LibraryHeader * mtLibInit (struct Library *base, BPTR segList, struct ExecIFace *ISys)
 {
      IExec = ISys;
  	  SysBase = (struct ExecBase*)ISys->Data.LibBase;

      D(BUG("ENTER %s\n", __FUNCTION__ ));

  	  base->lib_Node.ln_Type = NT_LIBRARY;
  	  base->lib_Node.ln_Pri  = 0;
  	  base->lib_Node.ln_Name = (char *)mtLibName;
  	  base->lib_Flags        = LIBF_SUMUSED|LIBF_CHANGED;
  	  base->lib_Version      = VERSION;
  	  base->lib_Revision     = REVISION;
  	  base->lib_IdString     = (char *)mtLibID;

      ((struct LibraryHeader *)base)->lh_Segment = segList;
      InitSemaphore(&((struct LibraryHeader *)base)->lh_Semaphore);

      D(BUG("EXIT %s with base = %lx\n", __FUNCTION__, base ));

  	  return (struct LibraryHeader *)base;
 }
#endif // __amigaos4__

#ifdef __amigaos4__
 BPTR mtLibExpunge(struct LibraryManagerInterface *Self)
  {
   struct LibraryHeader *base = (struct LibraryHeader *)Self->Data.LibBase;
#elif __MORPHOS__
 BPTR mtLibExpunge(void)
  {
   struct LibraryHeader *base = (void *)REG_A6;
 #else
 BPTR SAVEDS ASM mtLibExpunge(REG(a6, struct LibraryHeader * const base))
  {
#endif
   BPTR rc;
   struct Library *lib = &base->lh_Library;

   if (lib->lib_OpenCnt != 0)
    {
     lib->lib_Flags |= LIBF_DELEXP;
     return((BPTR)NULL);
    }
   Remove((struct Node *)base);
   rc = base->lh_Segment;
   FreeMem((UBYTE *)base-lib->lib_NegSize, lib->lib_NegSize+lib->lib_PosSize);
   return(rc);
  }



#ifdef __amigaos4__
 struct LibraryHeader * mtLibOpen(struct LibraryManagerInterface *Self, UNUSED ULONG version)
 {
   struct LibraryHeader *base = (struct LibraryHeader *)Self->Data.LibBase;

   D(BUG("ENTERING mtLibOpen\n"));

#elif __MORPHOS__
 struct LibraryHeader * mtLibOpen( void )
 {
   struct LibraryHeader *base = (void *)REG_A6;
#else
 struct LibraryHeader * SAVEDS ASM mtLibOpen(REG(a6, struct LibraryHeader *const base))
  {
#endif

   struct LibraryHeader *rc;

   ObtainSemaphore(&base->lh_Semaphore);
   base->lh_Library.lib_OpenCnt++;
   base->lh_Library.lib_Flags &= ~LIBF_DELEXP;
   if (UserLibOpen((struct Library *)base))
    {
     rc = base;
    }
   else
    {
     rc = NULL;
     base->lh_Library.lib_OpenCnt--;
    }
   ReleaseSemaphore(&base->lh_Semaphore);
   return(rc);
  }


#ifdef __amigaos4__
 BPTR mtLibClose(struct LibraryManagerInterface *Self)
  {
   struct LibraryHeader *base = (struct LibraryHeader *)Self->Data.LibBase;
#elif __MORPHOS__
 BPTR mtLibClose(void)
  {
   struct LibraryHeader *base = (struct LibraryHeader *)REG_A6;
#else
 BPTR ASM SAVEDS mtLibClose(REG(a6, struct LibraryHeader *base))
  {
#endif
   struct ExecBase *SysBase = *((struct ExecBase **)4);
   BPTR rc = (BPTR)NULL;

   ObtainSemaphore(&base->lh_Semaphore);
   UserLibClose((struct Library *)base);
   if(base->lh_Library.lib_OpenCnt > 0)
     base->lh_Library.lib_OpenCnt--;
   ReleaseSemaphore(&base->lh_Semaphore);
   if ((base->lh_Library.lib_OpenCnt == 0) &&
       (base->lh_Library.lib_Flags & LIBF_DELEXP))
    {
     #if defined(__amigaos4__)
       rc = mtLibExpunge(Self);
     #elif defined(__MORPHOS__)
       rc = mtLibExpunge();
     #else
       rc = mtLibExpunge(base);
     #endif
    }
   return(rc);
  }



 static BOOL SAVEDS ASM UserLibOpen(REG(a6, const struct Library *const base))
 {
   if (base->lib_OpenCnt == 1)
   {
     MUIMasterBase = OpenLibrary(MUIMASTER_NAME,(ULONG)base->lib_Version);
     if (MUIMasterBase != NULL
        #if defined(__amigaos4__)
           && GETINTERFACE(IMUIMaster, MUIMasterBase)
        #endif
        )
     {
       ThisClass = MUI_CreateCustomClass((struct Library *const)base,SUPERCLASS,NULL,sizeof(struct Data),Dispatcher);
       if (ThisClass != NULL)
       {

         D(BUG("CustomClass created\n"));

    	 UtilityBase   =
                             #if !defined(__SASC) && !defined(_DCC)
                             (struct UtilityBase *)
                             #endif
                             ThisClass->mcc_UtilityBase;
    	 DOSBase       = (struct DosLibrary *)ThisClass->mcc_DOSBase;
    	 GfxBase       = (struct GfxBase *)ThisClass->mcc_GfxBase;
    	 IntuitionBase = (struct IntuitionBase *)ThisClass->mcc_IntuitionBase;

         D(BUG("UtilityBase = %lx, DOSBase = %lx, GfxBase = %lx, IntuitionBase = %lx\n", UtilityBase, DOSBase, GfxBase, IntuitionBase ));

         if( UtilityBase && DOSBase && GfxBase && IntuitionBase
           #if defined(__amigaos4__)
           &&
           GETINTERFACE(IUtility, UtilityBase) &&
           GETINTERFACE(IDOS, DOSBase) &&
           GETINTERFACE(IGraphics, GfxBase) &&
           GETINTERFACE(IIntuition, IntuitionBase)
           #endif
           )
         {
           D(BUG("Interface obtained\n"));
    	   if (ClassInitFunc(base) == TRUE);
           {
             D(BUG("ClassInitFunc succeeded\n"));
    	     return(TRUE);
    	   }
         }
    	 if (!MUI_DeleteCustomClass(ThisClass))
    	 {
    	   D(BUG("Custom class deletion failed\n"));
    	 }

          #if defined(__amigaos4__)
         DROPINTERFACE(IIntuition);
         DROPINTERFACE(IGraphics);
         DROPINTERFACE(IDOS);
         DROPINTERFACE(IUtility);
          #endif
       }
        #if defined(__amigaos4__)
       DROPINTERFACE(IMUIMaster);
        #endif
       CloseLibrary(MUIMasterBase);
     }
     ThisClass	   = NULL;
     MUIMasterBase = NULL;
     IntuitionBase = NULL;
     UtilityBase   = NULL;
     DOSBase	   = NULL;
     GfxBase	   = NULL;
     return(FALSE);
   }
   return(TRUE);
 }


 static BOOL SAVEDS ASM UserLibClose(REG(a6, const struct Library *const base))
  {
   if (base->lib_OpenCnt == 1)
    {
     ClassExitFunc(base);
     if (ThisClass != NULL)
      {
       if (!MUI_DeleteCustomClass(ThisClass))
    	{
    	 /* can't delete public custom class */
    	}
      }

     #if defined(__amigaos4__)
    DROPINTERFACE(IIntuition);
    DROPINTERFACE(IGraphics);
    DROPINTERFACE(IDOS);
    DROPINTERFACE(IUtility);
     #endif

     if (MUIMasterBase != NULL)
      {
       #if defined(__amigaos4__)
        DROPINTERFACE(IMUIMaster);
       #endif
       CloseLibrary(MUIMasterBase);
      }
     ThisClass	     = NULL;
     MUIMasterBase   = NULL;
     IntuitionBase   = NULL;
     UtilityBase     = NULL;
     DOSBase	     = NULL;
     GfxBase	     = NULL;
    }
   return(TRUE);
  }



#if defined(__amigaos4__)
 struct MUI_CustomClass * SAVEDS ASM MCC_Query( struct Interface *Self, const LONG which)
#else  // !__amigaos4__
 struct MUI_CustomClass * SAVEDS ASM MCC_Query(REG(d0, const LONG which))
#endif // !__amigaos4__
  {
   switch (which)
    {
     #ifndef PREFSCLASS
     case 0 : return(ThisClass);	/* Pointer to Custom Class */
     #else
     case 1 : return(ThisClass);	/* Pointer to Preference Class */
     case 2 : return(
		     #ifndef PREFSIMAGE_DEPTH
		     NULL
		     #else
		     (struct MUI_CustomClass *)
		     MUI_NewObject(MUIC_Bodychunk,
				     MUIA_Bodychunk_Body,	     PrefsImage_body,
				     MUIA_Bodychunk_Compression, PREFSIMAGE_COMPRESSION,
				     MUIA_Bodychunk_Depth,	     PREFSIMAGE_DEPTH,
				     MUIA_Bodychunk_Masking,	 PREFSIMAGE_MASKING,
				     MUIA_Bitmap_Width, 	     PREFSIMAGE_WIDTH,
				     MUIA_Bitmap_Height,	     PREFSIMAGE_HEIGHT,
				     MUIA_FixWidth,		         PREFSIMAGE_WIDTH,
				     MUIA_FixHeight,		     PREFSIMAGE_HEIGHT,
				     MUIA_Bitmap_SourceColors,	 PrefsImage_colors,
				     MUIA_Bitmap_Transparent,	 0,
				   TAG_DONE
				  )
		     #endif
		    );			/* Pref Image Pointer */
     case 3 : return(ONLYGLOBAL);
     #endif
    }
   return(NULL);
  }

#if defined(__amigaos4__)
 #include <exec/emulation.h>

 uint32 mtLibObtain( struct Interface *Self ) {
    return( Self->Data.RefCount++ );
 }

 uint32 mtLibRelease( struct Interface *Self ) {
    return( Self->Data.RefCount-- );
 }

 STATIC ULONG stub_OpenPPC(ULONG *regarray)
 {
	struct Library *Base = (struct Library *) regarray[REG68K_A6/4];
	struct ExtendedLibrary *ExtLib = (struct ExtendedLibrary *) ((ULONG)Base + Base->lib_PosSize);
	struct LibraryManagerInterface *Self = (struct LibraryManagerInterface *) ExtLib->ILibrary;

	return (ULONG)mtLibOpen(Self, 0);
 }

 struct EmuTrap stub_Open = { TRAPINST, TRAPTYPE, stub_OpenPPC };


 STATIC ULONG stub_ClosePPC(ULONG *regarray)
 {
	struct Library *Base = (struct Library *) regarray[REG68K_A6/4];
	struct ExtendedLibrary *ExtLib = (struct ExtendedLibrary *) ((ULONG)Base + Base->lib_PosSize);
	struct LibraryManagerInterface *Self = (struct LibraryManagerInterface *) ExtLib->ILibrary;

	return (ULONG)mtLibClose(Self);
 }

 struct EmuTrap stub_Close = { TRAPINST, TRAPTYPE, stub_ClosePPC };


 STATIC ULONG stub_ExpungePPC(ULONG *regarray)
 {
	return 0UL;
 }

 struct EmuTrap stub_Expunge = { TRAPINST, TRAPTYPE, stub_ExpungePPC };


 STATIC ULONG stub_ReservedPPC(ULONG *regarray)
 {
	return 0UL;
 }

 struct EmuTrap stub_Reserved = { TRAPINST, TRAPTYPE, stub_ReservedPPC };


 static ULONG stub_MCC_QueryPPC(ULONG *regarray)
 {
	struct Library *Base = (struct Library *) regarray[REG68K_A6/4];
	struct ExtendedLibrary *ExtLib = (struct ExtendedLibrary *) ((ULONG)Base + Base->lib_PosSize);

	return (ULONG)MCC_Query(
        ExtLib->MainIFace,
		(LONG)regarray[REG68K_D0/4]
	);
 }

 struct EmuTrap stub_MCC_Query = { TRAPINST, TRAPTYPE, stub_MCC_QueryPPC };


 ULONG VecTable68K[] = {
	(ULONG)&stub_Open,
	(ULONG)&stub_Close,
	(ULONG)&stub_Expunge,
	(ULONG)&stub_Reserved,

	(ULONG)&stub_MCC_Query,
	(ULONG)-1
};

#endif // __amigaos4__



 /* Disable CTRL-C break handling */
 void REGARGS __chkabort(void)
  {}

 void REGARGS _CXBRK(void)
  {}

