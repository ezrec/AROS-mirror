/*
 * filter.c
 *
 * filtering filerequester for playmf
 *
 * based on an example by David N. Junod
 *
 * adapted by Tino Wildenhain
 */

#include <dos/dos.h>
#include <dos/dosextens.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <utility/hooks.h>
#include <libraries/asl.h>
#include <string.h>
#include <stdio.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/asl.h>

#include <libcore/compiler.h>

#ifndef __SASC
#  define __stdargs
#endif

/* Library Bases */
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
struct Library *AslBase;


/* external Prototypes */
extern ULONG LoadAndPlaySMF(UBYTE *smfname, UBYTE *linkname, ULONG Flags);
extern LONG __stdargs Message(UBYTE *Msg,UBYTE *Options,...);


/**************************** filterhook ******************************/

ULONG ASM Filter (REG (a0) struct Hook *h, REG (a2) struct FileRequester *fr, REG (a1) struct AnchorPath *ap)
{

    UBYTE buffer[300];
    UBYTE fbuf[20];
    BPTR file;
    int l = 0;


    if(fr->fr_Drawer!=NULL){
      strncpy (buffer, fr->fr_Drawer, sizeof (buffer));
    }else{
      buffer[0]=0;
    }

    AddPart (buffer, ap->ap_Info.fib_FileName, sizeof (buffer));
    if (ap->ap_Info.fib_DirEntryType >0 )
        return(TRUE);

    if ((file = Open (buffer, MODE_OLDFILE)))
    {
        l=Read(file,fbuf,sizeof(fbuf));

        Close (file);
    }

    if (l!=sizeof(fbuf))
        return (FALSE);

    if (fbuf[0]=='M')
        if (fbuf[1]=='T')
            if (fbuf[2]=='h')
                if (fbuf[3]=='d')
                    if (fbuf[14]=='M')
                        if (fbuf[15]=='T')
                            if (fbuf[16]=='r')
                                if (fbuf[17]=='k')
                                    return(TRUE);
    if (fbuf[0]=='X')
        if (fbuf[1]=='P')
            if (fbuf[2]=='K')
                if (fbuf[3]=='F')
                    if (fbuf[16]=='M')
                        if (fbuf[17]=='T')
                            if (fbuf[18]=='h')
                                if (fbuf[19]=='d')
                                    return(TRUE);

    return(FALSE);


}


/************************* main part *************************************/

LONG FileRequester (UBYTE *defaultdir, UBYTE *linkname, ULONG Flags)
{
    LONG ec = RETURN_FAIL;
    int  i;
    struct FileRequester *fr;
    struct Hook filter;
    UBYTE buffer[300];

    struct WBArg   *argptr;

        if ((AslBase = OpenLibrary ("asl.library", 38)))
        {

            ec = RETURN_ERROR;

            filter.h_Entry = (HOOKFUNC)Filter;
            if ((fr = AllocAslRequestTags (ASL_FileRequest,
                                          ASLFR_TitleText,      "Select MIDI file(s) to play",
                                          /* ASLFR_PositiveText,   "Open", */
                                          ASLFR_DoPatterns,     TRUE,
                                          ASLFR_RejectIcons,    TRUE,
                                          ASLFR_FilterFunc,     &filter,
                                          ASLFR_DoMultiSelect,  TRUE,
                                          ASLFR_InitialDrawer, defaultdir,
                                          TAG_DONE)))
            {
                ec = RETURN_OK;

                AslRequestTags (fr, TAG_DONE);

                if (fr->fr_NumArgs == 1)
                {
                    strncpy (buffer, fr->fr_Drawer, sizeof (buffer));
                    AddPart (buffer, fr->fr_File, sizeof (buffer));

                    ec=LoadAndPlaySMF(buffer,linkname,Flags);
                }

                if (fr->fr_NumArgs > 1)
                {
                    argptr = fr->fr_ArgList;
                    for (i = 0; i < fr->fr_NumArgs; i++)
                    {
                        strncpy (buffer, fr->fr_Drawer, sizeof (buffer));
                        AddPart (buffer, argptr->wa_Name, sizeof (buffer));

                        ec=LoadAndPlaySMF(buffer,linkname,Flags);

                        argptr++;

                        if (ec != RETURN_OK) break;
                    }
                }

                FreeAslRequest (fr);

            }
            else
            {

                Message("Couldn't allocate asl requester!", NULL);
            }
            CloseLibrary (AslBase);
        }
        else
        {
           Message("Couldn't open asl.library!", NULL);
        }
    return(ec);
}
