#include <dos/rdargs.h>
#include <dos/dosasl.h>

/************************************************************************/
/*			      Touch V1.01				*/
/* Written and Copyright ©1993 by Dave Schreiber.  All Rights Reserved. */
/*									*/
/* This is an Amigaized version of the Unix utility with the same name. */
/* Usage:								*/
/*    Touch FILE/A/M							*/
/*									*/
/* Touch sets the date/time of all specified files to the current date	*/
/* and time.  This uses Workbench 2.0 wildcard pattern-matching, so all */
/* 2.0 wildcards are valid.						*/
/*									*/
/* To compile (SAS/C V6.0):                                             */
/*    smake								*/
/*									*/
/* Version history:							*/
/*    1.01 - Fixed an enforcer hit					*/
/*	     July 17, 1993						*/
/*    1.00 - Initial Release.						*/
/*	     July 17, 1993						*/
/************************************************************************/

#include <exec/types.h>
#include <dos/dos.h>
#include <exec/memory.h>

#include <proto/dos.h>
#include <proto/exec.h>

void touchFiles(char *pattern);

char trashBuf[512];

struct RDArgs ra=
{
   {NULL,0,0},
   NULL,
   trashBuf,
   512,
   "FILE/A/M"
};

char *version="$VER: Touch V1.01 (17.7.93)";

char *copyright="Copyright 1993 by Dave Schreiber.  All Rights Reserved";

int main(void);

main(void)
{
   ULONG args[2];
   char **filenames;
   int c;

   /*Get the list of filenames/patterns*/
   args[0]=NULL;
   ReadArgs("FILE/A/M",args,&ra);
   filenames=(char **)args[0];

   /*Update the date and time of each name matching any given pattern*/
   if(filenames!=NULL)
      for(c=0;filenames[c]!=NULL;c++)
	 touchFiles(filenames[c]);

   /*Free resources allocated by ReadArgs()*/
   FreeArgs(&ra);

   return 0;
}

void touchFiles(char *pattern)
{
   struct AnchorPath *anchor;
   struct DateStamp currentDate;
   BPTR origDir;

   /*Allocate a structure required by MatchFirst()*/
   anchor=(struct AnchorPath *)AllocMem(sizeof(struct AnchorPath),MEMF_CLEAR);
   if(anchor==NULL)
      return;

   /*Get first file*/
   if(MatchFirst(pattern,anchor)==0)
   {
      /*Update date/time*/
      origDir=CurrentDir(anchor->ap_Current->an_Lock);
      DateStamp(&currentDate);
      SetFileDate(anchor->ap_Info.fib_FileName,&currentDate);
      CurrentDir(origDir);

      /*Update the rest of the files*/
      while(MatchNext(anchor)==0)
      {
	 origDir=CurrentDir(anchor->ap_Current->an_Lock);
	 DateStamp(&currentDate);
	 SetFileDate(anchor->ap_Info.fib_FileName,&currentDate);
	 CurrentDir(origDir);
      }
   }

   /*All done*/
   MatchEnd(anchor);
   FreeMem(anchor,sizeof(struct AnchorPath));

   return;
}

