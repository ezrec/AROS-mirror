/*

   Written by Olivier LAVIALE (gofromiel@gofromiel.com)

   This tool creates a custom library header for a library based class from
   a  template  library  header (built in the executable). F_CreateCCLib is
   automatically used  by  F_Create.rexx  (F_Update.rexx  exactly)  so  you
   should rarely use it.

   Remember to set the (s)cript bit of the file if it isn't, this  will  be
   easier then using EXECUTE to create the executable.

   WARNING: The name of the library header to create MUST start with 'FC_'.
   
$VER: 03.10 (2005/07/27)

    Added '_SysBase' symbol. It is initialized during lib_open().

*/

#define USE_SYSBASE

///Header
#include <exec/memory.h>
#include <dos/dos.h>
#include <dos/rdargs.h>
#include <utility/hooks.h>
#include <libraries/feelin.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/locale.h>

struct LocaleBase                  *LocaleBase;

char version[] = "$VER: F_CreateCCLib 3.10 (2005/07/27) by Olivier LAVIALE (gofromiel@gofromiel.com)";

/* Template */

///include
static char include[] =
"*\n" \
"* The following definitions are directly taken from exec includes for speed\n" \
"* and portability reason. I hope this will work for everybody, let me know.\n" \
"*\n" \
"\n" \
"* from 'exec/types.i'\n" \
"\n" \
"STRUCTURE MACRO\n" \
"\\1        EQU                 0\n" \
"SOFFSET   SET                 \\2\n" \
"          ENDM\n" \
"\n" \
"BYTE     MACRO\n" \
"\\1       EQU       SOFFSET\n" \
"SOFFSET  SET       SOFFSET+1\n" \
"         ENDM\n" \
"\n" \
"UBYTE    MACRO\n" \
"\\1       EQU       SOFFSET\n" \
"SOFFSET  SET       SOFFSET+1\n" \
"         ENDM\n" \
"\n" \
"UWORD    MACRO\n" \
"\\1       EQU       SOFFSET\n" \
"SOFFSET  SET       SOFFSET+2\n" \
"         ENDM\n" \
"\n" \
"ULONG    MACRO\n" \
"\\1       EQU       SOFFSET\n" \
"SOFFSET  SET       SOFFSET+4\n" \
"         ENDM\n" \
"\n" \
"APTR     MACRO\n" \
"\\1       EQU       SOFFSET\n" \
"SOFFSET  SET       SOFFSET+4\n" \
"         ENDM\n" \
"\n" \
"LABEL    MACRO\n" \
"\\1       EQU       SOFFSET\n" \
"         ENDM\n" \
"\n" \
"BITDEF   MACRO\n" \
"         BITDEF0 \\1,\\2,B_,\\3\n" \
"\\@BITDEF SET     1 << \\3\n" \
"         BITDEF0 \\1,\\2,F_,\\@BITDEF\n" \
"         ENDM\n" \
"\n" \
"BITDEF0  MACRO\n" \
"\\1\\3\\2   EQU     \\4\n" \
"         ENDM\n" \
"\n" \
"* from 'exec/initializer.i'\n" \
"\n" \
"INITBYTE    MACRO\n" \
"        IFLE    (\\1)-255\n" \
"        DC.B    $a0,\\1\n" \
"        DC.B    \\2,0\n" \
"        MEXIT\n" \
"        ENDC\n" \
"        DC.B    $e0,0\n" \
"        DC.W    \\1\n" \
"        DC.B    \\2,0\n" \
"        ENDM\n" \
"\n" \
"INITWORD    MACRO\n" \
"        IFLE    (\\1)-255\n" \
"        DC.B    $90,\\1\n" \
"        DC.W    \\2\n" \
"        MEXIT\n" \
"        ENDC\n" \
"        DC.B    $d0,0\n" \
"        DC.W    \\1\n" \
"        DC.W    \\2\n" \
"        ENDM\n" \
"\n" \
"INITLONG    MACRO\n" \
"        IFLE    (\\1)-255\n" \
"        DC.B    $80,\\1\n" \
"        DC.L    \\2\n" \
"        MEXIT\n" \
"        ENDC\n" \
"        DC.B    $c0,0\n" \
"        DC.W    \\1\n" \
"        DC.L    \\2\n" \
"        ENDM\n" \
"\n" \
"* from 'exec/nodes.i'\n" \
"\n" \
"   STRUCTURE   LN,0\n" \
"      APTR     LN_SUCC\n" \
"      APTR     LN_PRED\n" \
"      UBYTE    LN_TYPE\n" \
"      BYTE     LN_PRI\n" \
"      APTR     LN_NAME\n" \
"   LABEL       LN_SIZE\n" \
"\n" \
"NT_LIBRARY  EQU 9\n" \
"\n" \
"* from 'exec/libraries.i'\n" \
"\n" \
" STRUCTURE LIB,LN_SIZE\n" \
"    UBYTE   LIB_FLAGS\n" \
"    UBYTE   LIB_pad\n" \
"    UWORD   LIB_NEGSIZE\n" \
"    UWORD   LIB_POSSIZE\n" \
"    UWORD   LIB_VERSION\n" \
"    UWORD   LIB_REVISION\n" \
"    APTR    LIB_IDSTRING\n" \
"    ULONG   LIB_SUM\n" \
"    UWORD   LIB_OPENCNT\n" \
"    LABEL   LIB_SIZE\n" \
"\n" \
"    BITDEF  LIB,CHANGED,1\n" \
"    BITDEF  LIB,SUMUSED,2\n" \
"    BITDEF  LIB,DELEXP,3\n" \
"\n" \
"* from 'exec/resident.i'\n" \
"\n" \
"RTC_MATCHWORD   EQU $4AFC\n" \
"\n" \
"    BITDEF RT,AUTOINIT,7\n" \
"\n" \
"* from 'exec/exec_lib.i'\n" \
"\n" \
"_LVORemove         EQU -$00FC\n" \
"_LVOFreeMem        EQU -$00D2\n" \
"\n" \
"*\n" \
"* our library base structure\n" \
"*\n" \
"\n" \
"    STRUCTURE      FCCBase,LIB_SIZE\n" \
"       UWORD       fcc_pad0\n" \
"       ULONG       fcc_Seglist\n" \
"       APTR        fcc_SysBase\n" \
"       LABEL       FCCBase_SIZE\n" \
"\n" \
"*** done ********************************************************************\n" \
"\n";
//+
///library_0
static char library_0[] =
"*\n"\
"* start of library interface\n"\
"*\n"\
"         SECTION   text,CODE\n"\
"\n"\
"         xref      _FCC_Query\n"\
"\n"\
"         MOVEQ     #-1,D0\n"\
"         RTS\n"\
"\n"\
"RomTag   Dc.w  RTC_MATCHWORD\n"\
"         Dc.l  RomTag\n"\
"         Dc.l  lib_EndCode\n"\
"         Dc.b  RTF_AUTOINIT\n"\
"         Dc.b  CLASS_VERSION\n"\
"         Dc.b  NT_LIBRARY\n"\
"         Dc.b  0\n"\
"         Dc.l  FCC_Name\n"\
"         Dc.l  FCC_Version + 6\n"\
"         Dc.l  lib_InitTable\n"\
"\n"\
"lib_EndCode:\n"\
"\n";
//+
///library_1
static char library_1[] =
"         Even\n"\
"\n"\
"lib_InitTable\n"\
"         Dc.l FCCBase_SIZE\n"\
"         Dc.l lib_FuncTable\n"\
"         Dc.l lib_DataTable\n"\
"         Dc.l LibInit\n"\
"\n"\
"lib_DataTable\n"\
"         INITBYTE  LN_TYPE,NT_LIBRARY\n"\
"         INITLONG  LN_NAME,FCC_Name\n"\
"         INITBYTE  LIB_FLAGS,LIBF_SUMUSED!LIBF_CHANGED\n"\
"         INITWORD  LIB_VERSION,CLASS_VERSION\n"\
"         INITWORD  LIB_REVISION,CLASS_REVISION\n"\
"         INITLONG  LIB_IDSTRING,FCC_Version + 6\n"\
"         Dc.l      0\n"\
"\n"\
"lib_FuncTable\n"\
"         Dc.l  LibOpen\n"\
"         Dc.l  LibClose\n"\
"         Dc.l  LibExpunge\n"\
"         Dc.l  LibNull\n"\
"\n"\
"         Dc.l  _FCC_Query\n"\
"\n"\
"         Dc.l  -1\n"\
"\n"\
"LibInit:                               ; (FCCBase,Seglist,ExecBase)(d0,a0,a6)\n"\
"         MOVE.L    a5,-(sp)\n"\
"         MOVEA.L   d0,a5\n"\
"         MOVE.L    a0,(fcc_Seglist,a5)\n"\
"         MOVE.L    a6,(fcc_SysBase,a5)\n"
#ifdef USE_SYSBASE
"         MOVE.L    a6,_SysBase\n"
#endif
"         MOVE.L    a5,d0\n"\
"         MOVEA.L   (sp)+,a5\n"\
"         RTS\n"\
"\n"\
"LibOpen:                               ; (FCCBase,Version)(a6,d0)\n"\
"         ADDQ.W    #1,(LIB_OPENCNT,a6)\n"\
"         BCLR      #LIBB_DELEXP,(LIB_FLAGS,a6)\n"\
"         MOVE.L    a6,d0\n"\
"         RTS\n"\
"\n"\
"LibClose:                              ; (FCCBase)(a6)\n"\
"         MOVEQ     #0,d0\n"\
"         SUBQ.W    #1,(LIB_OPENCNT,a6)\n"\
"         BNE.B     1$\n"\
"         BTST      #LIBB_DELEXP,(LIB_FLAGS,a6)\n"\
"         BEQ.B     1$\n"\
"\n"\
"         BSR       LibExpunge\n"\
"1$       RTS\n"\
"\n"\
"LibExpunge:                            ; (FCCBase)(a6)\n"\
"         MOVEM.L   d2/a5-a6,-(sp)\n"\
"         MOVEA.L   a6,a5\n"\
"         MOVEA.L   (fcc_SysBase,a5),a6\n"\
"\n"\
"         TST.W     (LIB_OPENCNT,a5)\n"\
"         BEQ.B     1$\n"\
"\n"\
"         BSET      #LIBB_DELEXP,(LIB_FLAGS,a5)\n"\
"         MOVEQ     #0,d0\n"\
"         BRA.B     .end\n"\
"\n"\
"1$       MOVE.L    (fcc_Seglist,a5),d2\n"\
"         MOVEA.L   a5,a1\n"\
"         JSR       _LVORemove(a6)\n"\
"\n"\
"         MOVEQ     #0,d0\n"\
"         MOVEA.L   a5,a1\n"\
"         MOVE.W    (LIB_NEGSIZE,a5),d0\n"\
"         SUBA.L    d0,a1\n"\
"         ADD.W     (LIB_POSSIZE,a5),d0\n"\
"         JSR       _LVOFreeMem(a6)\n"\
"\n"\
"         MOVE.L    d2,d0\n"\
".end     MOVEM.L   (sp)+,d2/a5-a6\n"\
"         RTS\n"\
"\n"\
"LibNull:\n"\
"         MOVEQ     #0,d0\n"\
"         RTS\n"\
"\n"\
"         SECTION   __MERGED,BSS\n"\
"\n"\
"         xdef      ___base\n"
"         xdef      _SysBase\n"
"\n"
"___base  ds.l      1\n"
"_SysBase ds.l      1\n"
"\n"
"         END";
//+
//+

///DateFunc
SAVEDS ASM(void) DateFunc
(
   REG(a0,struct Hook *Hook),
   REG(a1,ULONG Char)
)
{
   STRPTR string = Hook -> h_Data;

   *string++ = (UBYTE)(Char);

   Hook -> h_Data = string;
}
//+
///DateString
STRPTR DateString(void)
{
   struct DateStamp                 ds;
   struct Hook                      hook;
   static char                      date[] = "2003/02/17\0\0\0\0";
   struct Locale                   *l;

   if (LocaleBase = (APTR)(OpenLibrary("locale.library",39)))
   {
      DateStamp(&ds);

      hook.h_MinNode.mln_Succ = NULL;
      hook.h_MinNode.mln_Pred = NULL;
      hook.h_Entry           = (HOOKFUNC) DateFunc;
      hook.h_Data            = &date;

      if (l = OpenLocale(NULL))
      {
         FormatDate(l,"(20%y/%m/%d)",&ds,&hook);

         CloseLocale(l);
      }
      CloseLibrary(LocaleBase);
   }
   return (STRPTR)(&date);
}
//+

enum  {
      
      ARG_NAME,
      ARG_VER,
      ARG_REV,
      ARG_AUTHOR,
      ARG_DEST,
      ARG_DATE
 
      };
      
struct MyArgs
{
    STRPTR                          Name;
    ULONG                          *Version;
    ULONG                          *Revision;
    STRPTR                          Author;
    STRPTR                          Destination;
    STRPTR                          Date;
};
 
///Main
int main(void)
{
    struct RDArgs *rdargs;
    struct MyArgs args = { NULL, 0, 0, NULL, "library.a", NULL };
    BPTR dst;

    if ((rdargs = ReadArgs("N=NAME/A,V=VER/A/N,R=REV/A/N,C=COPYRIGHT/A/K,DST=DESTINATION/K,DATE/K",(LONG *)(&args),NULL)))
    {
        if ((dst = Open(args.Destination,MODE_NEWFILE)))
        {
            FPrintf(dst,"*\n* %s %ld.%ld\n* Generated by CreateCCLib\n*\n\n",args.Name,*args.Version,*args.Revision);
            FPrintf(dst,"CLASS_VERSION EQU %ld\n",*args.Version);
            FPrintf(dst,"CLASS_REVISION EQU %02.ld\n\n",*args.Revision);

            FPuts(dst,include);
            FPuts(dst,library_0);

            FPrintf(dst,"FCC_Name\n");
            FPrintf(dst,"         dc.b \"%s.fc\",0\n",args.Name + 3);
            FPrintf(dst,"FCC_Version\n");
            FPrintf(dst,"         dc.b \"$");
            FPrintf(dst,"VER: %s %ld.%ld %s %s\",0\n\n",args.Name,*args.Version,*args.Revision,(args.Date) ? args.Date : DateString(),args.Author);

            FPuts(dst,library_1);

            Close(dst);
        }
        else
        {
            Printf("Unable to Create '%s'\n",args.Destination);
        }

        FreeArgs(rdargs);
    }
    else
    {
        PrintFault(IoErr(),"CreateCCLib");
    }

    return 0;
}
//+
