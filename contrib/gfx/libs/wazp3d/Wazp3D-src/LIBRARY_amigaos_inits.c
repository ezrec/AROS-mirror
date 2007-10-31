#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/cybergraphics.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/asl.h>

#include <exec/resident.h>
#include <exec/initializers.h>
#include <exec/execbase.h>
#include <exec/libraries.h>
#include <dos/dos.h>
#include <intuition/intuitionbase.h>
#include <stdarg.h> /* for varargs */
#include <stdio.h>
#include <devices/timer.h>
#include <clib/timer_protos.h>
#include <pragmas/timer_pragmas.h>
#include <cybergraphx/cybergraphics.h>

/*======================================================================================*/
#define REM(t) LibAlert(#t);
BOOL OpenAmigaLibraries();
void CloseAmigaLibraries();
void LibAlert(UBYTE *t);
/*======================================================================================*/
/*     Name:            SDI_compiler.h
    Versionstring:    $VER: SDI_compiler.h 1.4 (30.03.2000)
    Author:        SDI
    Distribution:    PD
    Description:    defines to hide compiler stuff

 1.1   25.06.98 : created from data made by Gunter Nikl
 1.2   17.11.99 : added VBCC
 1.3   29.02.00 : fixed VBCC REG define
 1.4   30.03.00 : fixed SAVEDS for VBCC
*/
/*======================================================================================*/
/* first "exceptions" */

#if defined(__MAXON__)
  #define STDARGS
  #define REGARGS
  #define SAVEDS
  #define INLINE inline
#elif defined(__VBCC__)
  #define STDARGS
  #define REGARGS
  #define INLINE
  #define REG(reg,arg) __reg(#reg) arg
#elif defined(__STORM__)
  #define STDARGS
  #define REGARGS
  #define INLINE inline
#elif defined(__SASC)
  #define ASM(arg) arg __asm
#elif defined(__GNUC__)
  #define REG(reg,arg) arg __asm(#reg)
  #define LREG(reg,arg) register REG(reg,arg)
#endif

/* then "common" ones */

#if !defined(ASM)
  #define ASM(arg) arg
#endif
#if !defined(REG)
  #define REG(reg,arg) register __##reg arg
#endif
#if !defined(LREG)
  #define LREG(reg,arg) register arg
#endif
#if !defined(CONST)
  #define CONST const
#endif
#if !defined(SAVEDS)
  #define SAVEDS __saveds
#endif
#if !defined(INLINE)
  #define INLINE __inline
#endif
#if !defined(REGARGS)
  #define REGARGS __regargs
#endif
#if !defined(STDARGS)
  #define STDARGS __stdargs
#endif

#define D0(arg)  REG(d0,arg)
#define D1(arg)  REG(d1,arg)
#define D2(arg)  REG(d2,arg)
#define D3(arg)  REG(d3,arg)
#define D4(arg)  REG(d4,arg)
#define D5(arg)  REG(d5,arg)
#define D6(arg)  REG(d6,arg)
#define D7(arg)  REG(d7,arg)

#define A0(arg)  REG(a0,arg)
#define A1(arg)  REG(a1,arg)
#define A2(arg)  REG(a2,arg)
#define A3(arg)  REG(a3,arg)
#define A4(arg)  REG(a4,arg)
#define A5(arg)  REG(a5,arg)
#define A6(arg)  REG(a6,arg)
#define A7(arg)  REG(a7,arg)
/*======================================================================================*/
#include "LIBRARY_name.h"
/*======================================================================================*/
#ifdef _M68060
    #define CPUTXT    "060"
#elif defined(_M68040)
    #define CPUTXT    "040"
#elif defined(_M68030)
    #define CPUTXT    "030"
#elif defined(_M68020)
    #define CPUTXT    "020"
#else
    #define CPUTXT    ""
#endif
/*======================================================================================*/
#define LIBNAME     NAMETXT ".library"
#define IDSTRING NAMETXT " " VERSTXT " (" DATETXT ") " CPUTXT "\r\n"
#define FULLNAME LIBNAME " " VERSTXT " (" DATETXT ")680" CPUTXT
/*======================================================================================*/
/*    SegList pointer definition    */

#if defined(_AROS)
    typedef struct SegList * SEGLISTPTR;
#elif defined(__VBCC__)
    typedef APTR SEGLISTPTR;
#else
    typedef BPTR SEGLISTPTR;
#endif
/*======================================================================================*/
/*    library base private structure. The official one does not contain all the private fields! */
struct ExampleBase
    {
    struct Library        LibNode;
    ULONG                NumCalls;
    struct ExecBase *        SysBase;
    SEGLISTPTR            SegList;
    };
/*======================================================================================*/
struct ExecBase *SysBase=NULL;
struct IntuitionBase *IntuitionBase=NULL;
struct DosLibrary *DOSBase=NULL;
struct GfxBase *GfxBase=NULL;
struct Library *CyberGfxBase=NULL;
struct Library *CGXVideoBase=NULL;
struct Library *MathIeeeSingBasBase=NULL;
struct Library *MathIeeeDoubBasBase=NULL;
struct Library *AslBase=NULL;
struct Library *UtilityBase=NULL;
struct Library *GadToolsBase;
struct Library *TimerBase=NULL;
struct timerequest *tr;
BOOL LibDebug;
void WAZP3D_Settings();
/*======================================================================================*/
/* First executable routine of this library; must return an error to the unsuspecting caller */
LONG ReturnError(void)
{
    if(!OpenAmigaLibraries())     return -1;
    LibAlert(LIBNAME " is not a program ! Copy it to libs:");
    CloseAmigaLibraries();
    return -1;
}
/*======================================================================================*/
struct LibInitData {
 UBYTE i_Type;     UBYTE o_Type;     UBYTE    d_Type;    UBYTE p_Type;
 UBYTE i_Name;     UBYTE o_Name;     STRPTR d_Name;
 UBYTE i_Flags;    UBYTE o_Flags;    UBYTE    d_Flags;    UBYTE p_Flags;
 UBYTE i_Version;    UBYTE o_Version;    UWORD    d_Version;
 UBYTE i_Revision; UBYTE o_Revision; UWORD    d_Revision;
 UBYTE i_IdString; UBYTE o_IdString; STRPTR d_IdString;
 ULONG endmark;
};
/*======================================================================================*/
extern const ULONG LibInitTable[4]; /* the prototype */
/* The library loader looks for this marker in the memory the library code
   and data will occupy. It is responsible setting up the Library base data structure.  */
/*======================================================================================*/
const struct Resident RomTag = {
    RTC_MATCHWORD,                /* Marker value. */
    (struct Resident *)&RomTag,        /* This points back to itself. */
    (struct Resident *)LibInitTable,     /* This points somewhere behind this marker. */
    RTF_AUTOINIT,                /* The Library should be set up according to the given table. */
    VERSION,                    /* The version of this Library. */
    NT_LIBRARY,                    /* This defines this module as a Library. */
    0,                        /* Initialization priority of this Library; unused. */
    LIBNAME,                    /* Points to the name of the Library. */
    IDSTRING,                    /* The identification string of this Library. */
    (APTR)&LibInitTable            /* This table is for initializing the Library. */
};
/*======================================================================================*/
/*    your own library's accessables functions    */
#include "LIBRARY_functions_glue.c"
/*======================================================================================*/
/* The mandatory reserved library function */
ULONG LibReserved(void)
{ 
    WAZP3D_Settings();
    return 0;
}
/*======================================================================================*/
/* Open the library, as called via OpenLibrary() */
ASM(struct Library *) LibOpen(REG(a6, struct ExampleBase * ExampleBase))
{
    REM(LibOpen)
    /* Prevent delayed expunge and increment opencnt */
    ExampleBase->LibNode.lib_Flags &= ~LIBF_DELEXP;
    ExampleBase->LibNode.lib_OpenCnt++;
    return &ExampleBase->LibNode;
}
/*==================================================================================*/
void LibAlert(UBYTE *t)
{
void *Data=&t+1L;
struct EasyStruct EasyStruct;
ULONG IDCMPFlags;

    if(!LibDebug ) return;
    if(SysBase==0) return;
    EasyStruct.es_StructSize=sizeof(struct EasyStruct);
    EasyStruct.es_Flags=0L;
    EasyStruct.es_Title="Message:";
    EasyStruct.es_TextFormat=t;
    EasyStruct.es_GadgetFormat="OK";

    IDCMPFlags=0L;
    (void)EasyRequestArgs(NULL,&EasyStruct,&IDCMPFlags,Data);
    return;
}
/*======================================================================================*/
/* Expunge the library, remove it from memory */
ASM(SEGLISTPTR) LibExpunge(REG(a6, struct ExampleBase * ExampleBase))
{
    REM(LibExpunge)

    if(!ExampleBase->LibNode.lib_OpenCnt)
        {
        SEGLISTPTR SegList;
        SegList = ExampleBase->SegList;
        WAZP3D_Close();
        CloseAmigaLibraries();

        /* Remove the library from the public list */
        Remove((struct Node *) ExampleBase);

        /* Free the vector table and the library data */
        FreeMem((STRPTR) ExampleBase - ExampleBase->LibNode.lib_NegSize,
        ExampleBase->LibNode.lib_NegSize +
        ExampleBase->LibNode.lib_PosSize);

        return SegList;
        }
    else
        ExampleBase->LibNode.lib_Flags |= LIBF_DELEXP;

    /* Return the segment pointer, if any */
    return 0;
}
/*======================================================================================*/
/* Close the library, as called by CloseLibrary() */
ASM(SEGLISTPTR) LibClose(REG(a6, struct ExampleBase * ExampleBase))
{
    REM(LibClose)
    WAZP3D_Close();
    if(!(--ExampleBase->LibNode.lib_OpenCnt))
        {
        if(ExampleBase->LibNode.lib_Flags & LIBF_DELEXP)
            return LibExpunge(ExampleBase);
        }
    return 0;
}
/*======================================================================================*/
/* Initialize library */
ASM(struct Library *) LibInit(REG(a0, SEGLISTPTR SegList),
REG(d0, struct ExampleBase * ExampleBase), REG(a6, struct ExecBase *SysBaseLocale))
{

    LibDebug=FALSE;
    SysBase=SysBaseLocale;
    REM(LibInit)
#ifdef _M68060
    if(!(SysBase->AttnFlags & AFF_68060))
    return 0;
#elif defined (_M68040)
    if(!(SysBase->AttnFlags & AFF_68040))
    return 0;
#elif defined (_M68030)
    if(!(SysBase->AttnFlags & AFF_68030))
    return 0;
#elif defined (_M68020)
    if(!(SysBase->AttnFlags & AFF_68020))
    return 0;
#endif

    /* Remember stuff */
    ExampleBase->SegList = SegList;
    ExampleBase->SysBase = SysBase;

    if(OpenAmigaLibraries())
        {
        LibAlert(FULLNAME);
        return &ExampleBase->LibNode;
        }

    /* Free the vector table and the library data */
    FreeMem((STRPTR) ExampleBase - ExampleBase->LibNode.lib_NegSize,
    ExampleBase->LibNode.lib_NegSize + ExampleBase->LibNode.lib_PosSize);
    return 0;
}
/*==================================================================================*/
void *Libmalloc(ULONG size )
{
void *pt;

pt=AllocVec(size,MEMF_FAST | MEMF_CLEAR);
if(pt==NULL)
    return(AllocVec(size,MEMF_ANY | MEMF_CLEAR));
else
    return(pt); 
}
/*==================================================================================*/
void Libfree(void *p)
    {FreeVec(p);}
/*==================================================================================*/
void *Libmemcpy(void *s1,const void *s2,LONG n)
{
  CopyMem((APTR)s2,(APTR)s1,n);
  return s1;
}
/*==================================================================================*/
UBYTE *Libstrcpy(UBYTE *s1,UBYTE *s2)
{ UBYTE *s=s1;
  do
    *s++=*s2;
  while(*s2++!='\0');
  return s1;
}
/*==================================================================================*/
UBYTE *Libstrcat(UBYTE *s1,UBYTE *s2)
{
  UBYTE *s=s1;

  while(*s++)
    ;
  --s;
  while((*s++=*s2++))
    ;
  return s1;
}
/*==================================================================================*/
ULONG Libstrlen(UBYTE *string)
{
UBYTE *s=string;

  while(*s++)
    ;
  return ~(string-s);
}
/*==================================================================================*/
void Libprintf(UBYTE *string, ...)
{
UBYTE buffer[256];
va_list args;

    if(!LibDebug ) return;
    if(SysBase==0) return;
    va_start(args, string);
    RawDoFmt(string, args, (void (*))"\x16\xc0\x4e\x75", buffer);
    va_end(args);
    Write(Output(), buffer, Libstrlen(buffer));
}

/*==================================================================================*/
void Libsavefile(UBYTE *filename,void *pt,ULONG size)
{
BPTR file;

    if (file = Open(filename,MODE_NEWFILE)) 
    { 
    Write(file,pt,size);
    Close(file); 
    }
}
/*==================================================================================*/
BOOL   OpenAmigaLibraries()
{
/* Initialize the standards libraries We assume that ROM libraries open OK */

REM(OpenAmigaLibraries)
    DOSBase=(struct DosLibrary *)    OpenLibrary("dos.library",36L);
    GfxBase= (struct GfxBase *)    OpenLibrary("graphics.library",0L);
    IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 0L);
    AslBase=                OpenLibrary("asl.library", 36L);
    MathIeeeSingBasBase=        OpenLibrary("mathieeesingbas.library",34);
    MathIeeeDoubBasBase=        OpenLibrary("mathieeedoubbas.library",34);
    UtilityBase=(struct Library *)OpenLibrary("utility.library",36L);
    GadToolsBase =             OpenLibrary("gadtools.library", 37);

    CyberGfxBase =            OpenLibrary("cybergraphics.library", 0L);
    if (CyberGfxBase==NULL)
        {REM(Error opening cybergraphics.library);return(FALSE);}

    tr = (struct timerequest *)calloc(1, sizeof(struct timerequest));
    if (OpenDevice(TIMERNAME, UNIT_MICROHZ, (struct IORequest *) tr, 0L) != 0)
        {REM(Error opening timer.device);return(FALSE);}
    TimerBase = (struct Library *)tr->tr_node.io_Device;

    WAZP3D_Init();
    return(TRUE);
}
/*======================================================================================*/
void   CloseAmigaLibraries()
{
#define LIBCLOSE(Lbase)     if(Lbase!=NULL)    {CloseLibrary( ((struct Library *)Lbase) );Lbase=NULL;}

    CloseDevice((struct IORequest *)tr);Libfree(tr);
    LIBCLOSE(CyberGfxBase)
    LIBCLOSE(IntuitionBase)
    LIBCLOSE(GfxBase)
    LIBCLOSE(UtilityBase)
    LIBCLOSE(AslBase)
    LIBCLOSE(MathIeeeDoubBasBase)
    LIBCLOSE(MathIeeeSingBasBase)
    LIBCLOSE(DOSBase)
    LibDebug=FALSE;    /* with OS libraries closed cant print anymore */
}
/*======================================================================================*/
/* This is the table of functions that make up the library. The first
     four are mandatory, everything following it are user callable
     routines. The table is terminated by the value -1. */

static const APTR LibVectors[] = {
    (APTR) LibOpen,
    (APTR) LibClose,
    (APTR) LibExpunge,
    (APTR) LibReserved,

#include "LIBRARY_functions_names.h"

    (APTR) -1
};
/*======================================================================================*/
static const struct LibInitData LibInitData = {
#ifdef __VBCC__        /* VBCC does not like OFFSET macro */
 0xA0,    8, NT_LIBRARY,                0,
 0x80, 10, LIBNAME,
 0xA0, 14, LIBF_SUMUSED|LIBF_CHANGED, 0,
 0x90, 20, VERSION,
 0x90, 22, REVISION,
 0x80, 24, IDSTRING,
#else
 0xA0, (UBYTE) OFFSET(Node,    ln_Type),        NT_LIBRARY,         0,
 0x80, (UBYTE) OFFSET(Node,    ln_Name),        LIBNAME,
 0xA0, (UBYTE) OFFSET(Library, lib_Flags),    LIBF_SUMUSED|LIBF_CHANGED, 0,
 0x90, (UBYTE) OFFSET(Library, lib_Version),    VERSION,
 0x90, (UBYTE) OFFSET(Library, lib_Revision), REVISION,
 0x80, (UBYTE) OFFSET(Library, lib_IdString), IDSTRING,
#endif
 0
};
/*======================================================================================*/
/* The following data structures and data are responsible for*/
/*     setting up the Library base data structure and the library*/
/*     function vector.*/
/*======================================================================================*/
const ULONG LibInitTable[4] = {
    (ULONG)sizeof(struct ExampleBase), /* Size of the base data structure */
    (ULONG)LibVectors,             /* Points to the function vector */
    (ULONG)&LibInitData,             /* Library base data structure setup table */
    (ULONG)LibInit                 /* The address of the routine to do the setup */
};
/*======================================================================================*/
