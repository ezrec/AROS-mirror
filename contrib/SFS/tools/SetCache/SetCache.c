#include <dos/dos.h>
#include <dos/dosextens.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <utility/tagitem.h>

#ifdef __AROS__
#include <dos/filesystem.h>
#endif

#include "../../packets.h"
#include "../../query.h"
#include "../dosdoio.c"

#ifdef __AROS__
#define __AMIGADATE__   "(29.11.2005)"
BYTE AROS_DoPkt(struct IOFileSys *iofs, LONG action, LONG Arg1, LONG Arg2, LONG Arg3, LONG Arg4, LONG Arg5)
{
    iofs->IOFS.io_Command = SFS_SPECIFIC_MESSAGE;
    iofs->io_PacketEmulation->dp_Type = action;
    iofs->io_PacketEmulation->dp_Arg1 = Arg1;
    iofs->io_PacketEmulation->dp_Arg2 = Arg2;
    iofs->io_PacketEmulation->dp_Arg3 = Arg3;
    iofs->io_PacketEmulation->dp_Arg4 = Arg4;
    iofs->io_PacketEmulation->dp_Arg5 = Arg5;
    
    DosDoIO((struct IORequest *)iofs, SysBase);
    
    return iofs->io_PacketEmulation->dp_Res1;
}
#else
#endif 

static const char version[]={"\0$VER: SetCache 1.2 " __AMIGADATE__ "\r\n"};

int main()
{
    struct RDArgs *readarg;
    UBYTE template[]="DEVICE/A,LINES/N,READAHEAD/N,NOCOPYBACK/S\n";

    struct {char *name;
          ULONG *lines;
          ULONG *readahead;
          ULONG nocopyback;} arglist={NULL};

    if((DOSBase=(struct DosLibrary *)OpenLibrary("dos.library",37))!=0) {
        if((readarg=ReadArgs(template,(LONG *)&arglist,0))!=0) {
            struct MsgPort *msgport;
            struct DosList *dl;
#ifdef __AROS__
            struct IOFileSys *IOFS;
#endif
            UBYTE *devname=arglist.name;

            while(*devname!=0) {
                if(*devname==':') {
                    *devname=0;
                    break;
                }
            devname++;
            }

            dl=LockDosList(LDF_DEVICES|LDF_READ);
            if((dl=FindDosEntry(dl,arglist.name,LDF_DEVICES))!=0) {
                ULONG copyback=1;
                LONG errorcode;
#ifdef __AROS__
                msgport=CreateMsgPort();
                IOFS = (struct IOFileSys *)CreateIORequest(msgport, sizeof(struct IOFileSys));
                IOFS->io_PacketEmulation = AllocVec(sizeof(struct DosPacket), MEMF_PUBLIC|MEMF_CLEAR);
                IOFS->IOFS.io_Device = dl->dol_Device;
                IOFS->IOFS.io_Unit   = dl->dol_Unit;
#else
                msgport=dl->dol_Task;
#endif
                UnLockDosList(LDF_DEVICES|LDF_READ);

                if(arglist.lines!=0 || arglist.readahead!=0 || arglist.nocopyback!=0) {
                    struct TagItem tags[]={ASQ_CACHE_LINES, 0,
                                 ASQ_CACHE_READAHEADSIZE, 0,
                                 ASQ_CACHE_MODE, 0,
                                 TAG_END, 0};

#ifdef __AROS__
                    if((errorcode=AROS_DoPkt(IOFS, ACTION_SFS_QUERY, (LONG)&tags, 0, 0, 0, 0))!=DOSFALSE) {
#else
                    if((errorcode=DoPkt(msgport, ACTION_SFS_QUERY, (LONG)&tags, 0, 0, 0, 0))!=DOSFALSE) {
#endif
                        ULONG lines,readahead;

                        lines=tags[0].ti_Data;
                        readahead=tags[1].ti_Data;

                        if(arglist.nocopyback!=0) {
                            copyback=0;
                        }

                        if(arglist.lines!=0) {
                            lines=*arglist.lines;
                        }

                        if(arglist.readahead!=0) {
                            readahead=*arglist.readahead;
                        }

                        VPrintf("Setting cache to %ld lines ", &lines);
                        VPrintf("of %ld bytes and copyback mode ", &readahead);
                        if(copyback!=0) {
                            PutStr("enabled.\n");
                        }
                        else {
                            PutStr("disabled.\n");
                        }

#ifdef __AROS__
                        if((errorcode=AROS_DoPkt(IOFS, ACTION_SET_CACHE, lines, readahead, copyback, 0, 0))==DOSFALSE) {
#else
                        if((errorcode=DoPkt(msgport,ACTION_SET_CACHE, lines, readahead, copyback, 0, 0))==DOSFALSE) {
#endif
                            PrintFault(IoErr(),"error while setting new cache size");
                        }
                    }
                    else {
                        PrintFault(IoErr(),"error while reading old cache settings");
                    }
                }

                {
                    struct TagItem tags[]={ASQ_CACHE_LINES, 0,
                                 ASQ_CACHE_READAHEADSIZE, 0,
                                 ASQ_CACHE_MODE, 0};

#ifdef __AROS__
                    if((errorcode=AROS_DoPkt(IOFS, ACTION_SFS_QUERY, (LONG)&tags, 0, 0, 0, 0))!=DOSFALSE) {
#else
                    if((errorcode=DoPkt(msgport, ACTION_SFS_QUERY, (LONG)&tags, 0, 0, 0, 0))!=DOSFALSE) {
#endif
                        VPrintf("Current cache settings: %ld lines,", &tags[0].ti_Data);
                        VPrintf(" %ld bytes readahead, ", &tags[1].ti_Data);
                        if(tags[2].ti_Data==0) {
                            PutStr("no copyback.\n");
                        }
                        else {
                            PutStr("copyback.\n");
                        }
                    }
                }
#ifdef __AROS__
                FreeVec(IOFS->io_PacketEmulation);
                DeleteIORequest((struct IORequest *)IOFS);
                DeleteMsgPort(msgport);
#endif            
            }
            else {
                VPrintf("Couldn't find device '%s:'.\n",&arglist.name);
                UnLockDosList(LDF_DEVICES|LDF_READ);
            }

            FreeArgs(readarg);
        }
        else {
            PutStr("Wrong arguments!\n");
        }
        CloseLibrary((struct Library *)DOSBase);
    }
    return(0);
}
