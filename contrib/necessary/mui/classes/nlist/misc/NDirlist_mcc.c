
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <dos/dos.h>
#include <dos/rdargs.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <exec/io.h>
#include <exec/resident.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <libraries/mui.h>

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>

#include <clib/muimaster_protos.h>

/*
#include <proto/muimaster.h>
#include <inline/muimaster.h>
*/

extern struct TagItem *FindTagItem(Tag,struct TagItem *);


#include "NDirlist_priv_mcc.h"
#include "NDirlist_mcc.rev.h"

#include <MUI/NDirlist_mcc.h>

#ifndef SAVEDS
#define SAVEDS
#endif

#ifndef __stdargs
#define __stdargs
#endif


/****************************************************************************************/
/****************************************************************************************/
/******************************                    **************************************/
/******************************   NDirlist Class   **************************************/
/******************************                    **************************************/
/****************************************************************************************/
/****************************************************************************************/

#define ZERO NULL

#define EXALLBUFFERSIZE 2048


static const ULONG tricky=0x16c04e75; /* move.b d0,(a3)+ ; rts */

static void mysprintf(STRPTR buffer,STRPTR fmt,...)
{ RawDoFmt(fmt,(ULONG *)&fmt+1,(void (*)())&tricky,buffer); } /* Not very clean, but ... */

static void NDL_DisplayFunc(void);
static const struct Hook NDL_DisplayHook = { {0,0}, (void *)NDL_DisplayFunc, NULL, NULL };

static void NDL_DisplayFunc(void)
{ register char **a2 __asm("a2");   char **array = a2;
  register struct DirEntry *a1 __asm("a1");    struct DirEntry *myptr = a1;

  if (myptr)
  { array[0]  = myptr->Name;
    array[1]  = myptr->Size;
    array[2]  = myptr->Prot;
    array[3]  = myptr->Date;
    array[4]  = myptr->Time;
    array[5]  = myptr->Comment;
    if (myptr->ed_Type >= 0)
    { array[0+DISPLAY_ARRAY_MAX]  = "\0333";
      array[1+DISPLAY_ARRAY_MAX]  = "\0333";
      array[2+DISPLAY_ARRAY_MAX]  = "\0333";
      array[5+DISPLAY_ARRAY_MAX]  = "\0333";
    }
  }
  else
  { array[0]  = "Name";
    array[1]  = "\033cSize";
    array[2]  = "\033cProt.";
    array[3]  = "\033cDate";
    array[4]  = "\033cTime";
    array[5]  = "Comment";
  }
}


static LONG NDL_Compare(struct NDLData *data,struct DirEntry *de1,struct DirEntry *de2)
{
  if (de1->ed_Type > de2->ed_Type)
    return (-1);
  else if (de1->ed_Type < de2->ed_Type)
    return (1);
  return ((LONG) stricmp(de1->Name,de2->Name));
}


static void NDL_FreeFiles(Object *obj,struct NDLData *data)
{
  if (data->NDirlist_DirEntry)
  { LONG n;
    for (n = 0;n < data->NDirlist_DirEntryCurrent;n++)
      FreeVec(data->NDirlist_DirEntry[n]);
    FreeVec(data->NDirlist_DirEntry);
    data->NDirlist_DirEntry = NULL;
  }
}



/*
  struct ExAllData *p;

    data->NDirlist_eac->eac_LastKey = 0;
    data->NDirlist_eac->eac_MatchString = NULL;
    data->NDirlist_eac->eac_MatchFunc = NULL;

    do {
      more = ExAll(fl, data->NDirlist_ed, EXALLBUFFERSIZE, ED_COMMENT, data->NDirlist_eac);

      if ((!more) && (IoErr() != ERROR_NO_MORE_ENTRIES))
      {
        break;
      }
      if (data->NDirlist_eac->eac_Entries == 0) {
        continue;
      }
      p = data->NDirlist_ed;
      do {
        if (!data->NDirlist_DirEntry)
        { data->NDirlist_DirEntry = (struct DirEntry **) AllocVec(sizeof(struct DirEntry *)*(400+1),0L);
          data->NDirlist_DirEntryCurrent = 0;
          data->NDirlist_DirEntryLast = 400;
        }
        else if (data->NDirlist_DirEntryCurrent >= data->NDirlist_DirEntryLast)
        { struct DirEntry **old_de = data->NDirlist_DirEntry;
          data->NDirlist_DirEntry = (struct DirEntry **) AllocVec(sizeof(struct DirEntry *)*(data->NDirlist_DirEntryLast*2+1),0L);
          if (data->NDirlist_DirEntry)
          { LONG n;
            for (n = 0;n < data->NDirlist_DirEntryCurrent;n++)
              data->NDirlist_DirEntry[n] = old_de[n];
            FreeVec(old_de);
          }
          else
            data->NDirlist_DirEntry = old_de;
        }
        if (data->NDirlist_DirEntry && (data->NDirlist_DirEntryCurrent < data->NDirlist_DirEntryLast))
        {
          dt.dat_Stamp.ds_Days = p->ed_Days;
          dt.dat_Stamp.ds_Minute = p->ed_Mins;
          dt.dat_Stamp.ds_Tick = p->ed_Ticks;
          if (!DateToStr(&dt))
          { strcpy(date,"????");
            strcpy(time,"????");
          }
          strcpy(prot,"-------");
          if (!(p->ed_Prot & FIBF_DELETE))  prot[6] = 'd';
          if (!(p->ed_Prot & FIBF_EXECUTE)) prot[5] = 'e';
          if (!(p->ed_Prot & FIBF_WRITE))   prot[4] = 'w';
          if (!(p->ed_Prot & FIBF_READ))    prot[3] = 'r';
          if (p->ed_Prot & FIBF_ARCHIVE)    prot[2] = 'a';
          if (p->ed_Prot & FIBF_PURE)       prot[1] = 'p';
          if (p->ed_Prot & FIBF_SCRIPT)     prot[0] = 's';
          if (p->ed_Type < 0)
            mysprintf(size,"%ld",p->ed_Size);
          else
            strcpy(size,"(dir)");

          lname = strlen(p->ed_Name)+1;
          lsize = strlen(size)+1;
          ldate = strlen(date)+1;
          ltime = strlen(time)+1;
          lprot = strlen(prot)+1;
          while ((ldate > 1) && (date[ldate-2] == ' '))
          { ldate--;
            date[ldate-1] = '\0';
          }
          if (p->ed_Comment)
            lcomment = strlen(p->ed_Comment);
          else
            lcomment = 0;
          direntry = (struct DirEntry *) AllocVec(sizeof(struct DirEntry)+lname+lsize+ldate+ltime+lprot+lcomment+1,0L);
          if (direntry)
          {
            direntry->Name = (UBYTE *) (direntry + 1);
            direntry->Size = direntry->Name + lname;
            direntry->Date = direntry->Size + lsize;
            direntry->Time = direntry->Date + ldate;
            direntry->Prot = direntry->Time + ltime;
            direntry->Comment = direntry->Prot + lprot;
            direntry->ed_Type = p->ed_Type;
            direntry->ed_Size = p->ed_Size;
            direntry->ed_Prot = p->ed_Prot;
            direntry->ed_Days = p->ed_Days;
            direntry->ed_Mins = p->ed_Mins;
            direntry->ed_Ticks = p->ed_Ticks;
            strcpy(direntry->Name,p->ed_Name);
            strcpy(direntry->Size,size);
            strcpy(direntry->Date,date);
            strcpy(direntry->Time,time);
            strcpy(direntry->Prot,prot);
            if (p->ed_Comment)
              strcpy(direntry->Comment,p->ed_Comment);
            else
              direntry->Comment[0] = '\0';

            data->NDirlist_DirEntry[data->NDirlist_DirEntryCurrent] = direntry;
            data->NDirlist_DirEntryCurrent++;
          }
        }
        else
          bad = TRUE;

        data->NDirlist_Total += p->ed_Size;
        p = p->ed_Next;
      } while (p);

    } while (more);
*/

static void NDL_ReadFiles(Object *obj,struct NDLData *data)
{
  BPTR fl;
  struct DirEntry *direntry;
  struct FileInfoBlock *fib = data->NDirlist_fib;
  BOOL bad = FALSE;

  if (((fl = Lock(data->NDirlist_Directory,SHARED_LOCK)) != ZERO) &&
      (Examine(fl,fib) != DOSFALSE) &&
      (fib->fib_DirEntryType >= 0) &&
      (fib->fib_DirEntryType != ST_SOFTLINK))
  {
    struct DateTime dt;
    UBYTE size[30];
    UBYTE date[30];
    UBYTE time[20];
    UBYTE prot[10];
    USHORT lname,lsize,ldate,ltime,lprot,lcomment;

    dt.dat_Format = FORMAT_DOS;
    dt.dat_Flags = DTF_SUBST;
    dt.dat_StrDay = size;
    dt.dat_StrDate = date;
    dt.dat_StrTime = time;

    data->NDirlist_Total = 0;

    while (ExNext(fl,fib) != DOSFALSE)
    {
      if (!data->NDirlist_DirEntry)
      { data->NDirlist_DirEntry = (struct DirEntry **) AllocVec(sizeof(struct DirEntry *)*(400+1),0L);
        data->NDirlist_DirEntryCurrent = 0;
        data->NDirlist_DirEntryLast = 400;
      }
      else if (data->NDirlist_DirEntryCurrent >= data->NDirlist_DirEntryLast)
      { struct DirEntry **old_de = data->NDirlist_                                                _sfmt.c                               378
                                                                           _sfmt.c                               394
                                                                           _sfmt.c                               3f2
                                                                           _sfmt.c                               474
                                                                           toupper.o                               e
                                                                           atoi.c                                 18
                                                                           atol.c                                 18
                                                                           strtol.o                               24
                                                                           _strupr.c                               e
                                                                           stpblk.o                                8
YAM                     21-Jun-01 22:33:57  Xref Section              Page 3-248

symbol                                                   (Def) Module  Off
                                                                           strnicmp.c                             22
                                                                           YAM_WR.c                             1926
                                                                           YAM_UT.c                             167e
                                                                           YAM_UT.c                             1744
                                                                           YAM_UT.c                             1790
                                                                           YAM_UT.c                             17da
                                                                           YAM_UT.c                             381c
                                                                           YAM_UT.c                             49ae
                                                                           YAM_rexx_rxif.c                      2c2e
                                                                           YAM_rexx_rxif.c                      33b0
                                                                           YAM_rexx.c                            494
                                                                           YAM_rexx.c                            714
                                                                           YAM_rexx.c                            bd6
                                                                           YAM_RE.c                             3ef0
                                                                           YAM_RE.c                             417c
                                                                           YAM_RE.c                             421a
                                                                           YAM_RE.c                             484a
                                                                           YAM_RE.c                             6350
                                                                           YAM_RE.c                             7964
                                                                           YAM_MAf.c                            1256
                                                                           YAM_FO.c                              46c
                                                                           YAM_COs.c                            1a46
                                                                           YAM_AB.c                             1df6
___curdir                        curdir.c                                0 c                                      fe
                                                                           c                                     1b2
                                                                           c                                     258
___cxexst                        _cxexst                                 0 YAM_RE.c                             3fea
                                                                           YAM_EA.c                              4ce
                                                                           YAM_AB.c                              510
                                                                           YAM_AB.c                             125c
                                                                           YAM_AB.c                             1a88
                                                                           YAM_AB.c                             1e64
___dtors                         _ctdt.o                                 c _fpinit.c                              26
___exitfuncs                     exitfunc.c                              0 exit.c                                  c
___firstfile                     _ufb.c                                  0 _iob.c                                  a
                                                                           fopen.c                                14
                                                                           fopen.c                                3e
                                                                           fopen.c                                44
                                                                           fflush.c                               b0
___fmask                         _fmode.c                                4 freopen.c                              ba
                                                                           _open.c                               1a0
___fmode                         _fmode.c                                0 _iob.c                                1e6
                                                                           freopen.c                              1e
___fpinit                        _fpinit.c                               0 c                                     1d0
                                                                           c                                     2a6
___fpterm                        _fpinit.c                              22 c                                     218
                                                                           c                                     2a0
___iob                           _iob.c                                  0 puts.c                                  c
                                                                           puts.c                                 1c
                                                                           printf.c                               22
                                                                           printf.c                               62
                                                                           printf.c                               6c
                                                                           tinyprintf.c                           8a
YAM                     21-Jun-01 22:33:57  Xref Section              Page 3-249

symbol                                                   (Def) Module  Off
                                                                           writes.c                               18
                                                                           YAM_rexx_rxif.c                       280
___main                          _main.c                                 0 c                                     1d8
                                                                           c                                     2ac
___nufbs                         _ufb.c                                  0 _iob.c                                1e0
                                                                           _chkufb.c                              12
                                                                           _open.c                                68
                                                                           _open.c                                6c
___open                          _open.c                                 0 freopen.c                              c2
___sigfunc                       sigfunc.c                               0 _ufb.c                                  8
___stack                         YAM_global.c                           10 _cxexst                                3a
                                                                           c                                      5c
                                                                           c                                      62
                                                                           c                                      8e
___stdinptr                      stdioptrs.c                             0 _iob.c                                  e
                                                                           fread.c                                92
                                                                           fgetc.c                                 e
___stdiov37                      _stdv36.c                               0 _iob.c                                 dc
                                                                           _iob.c                                126
___stdiowin                      _stdwin.c                               0 _iob.c                                 90
                                                                           _iob.c                                 a2
                                                                           _iob.c                                100
___stdAUNCH = { 631, "Launch the external editor\n"\
	"when you start a new message" };
const struct FC_Type _MSG_HELP_CO_ST_EDITOR = { 632, "Filename of your\n"\
	"favorite text editor" };
const struct FC_Type _MSG_HELP_CO_ST_FWDSTART = { 633, "Text used to introduce a forwarded message.\n"\
	"Use the button at the right side to insert a variable." };
const struct FC_Type _MSG_HELP_CO_ST_FWDEND = { 634, "Text to insert at the\n"\
	"end a forwarded message." };
const struct FC_Type _MSG_HELP_CO_ST_REPLYTEXT = { 635, "Texts used in replies to messages.\n"\
	"Use the button at the right side to insert a variable." };
const struct FC_Type _MSG_HELP_CO_ST_AREPLYTEXT = { 636, "Use these texts in replies to messages\n"\
	"if the pattern below matches the recipient address." };
const struct FC_Type _MSG_HELP_CO_ST_AREPLYPAT = { 637, "