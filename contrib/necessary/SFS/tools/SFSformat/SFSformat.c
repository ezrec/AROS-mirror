#include <dos/dos.h>
#include <dos/dosextens.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <utility/tagitem.h>

#ifdef __AROS__
#include <dos/filesystem.h>
#endif

#include "../../packets.h"

#ifdef __AROS__
#define __AMIGADATE__   "(29.11.2005)"

#include "../dosdoio.c"

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

static const char version[]={"\0$VER: SFSformat 1.1 " __AMIGADATE__ "\r\n"};

LONG main()
{
    struct RDArgs *readarg;
    UBYTE template[]="DEVICE=DRIVE/A/K,NAME/A/K,CASESENSITIVE/S,NORECYCLED/S,SHOWRECYCLED/S";
    UBYTE choice='N';

    struct {
        char *device;
        char *name;
        ULONG casesensitive;
        ULONG norecycled;
        ULONG showrecycled;
    } arglist={NULL};

    if((DOSBase=(struct DosLibrary *)OpenLibrary("dos.library",37))!=0)
    {
        if((readarg=ReadArgs(template,(LONG *)&arglist,0))!=0)
        {
            struct MsgPort *msgport;
            struct DosList *dl;
#ifdef __AROS__
            struct IOFileSys *IOFS;
#endif      
            UBYTE *devname=arglist.device;

            while(*devname!=0)
            {
                if(*devname==':')
                {
//                    *devname=0;
                    break;
                }
                devname++;
            }

            dl=LockDosList(LDF_DEVICES|LDF_READ);
            if((dl=FindDosEntry(dl,arglist.device,LDF_DEVICES))!=0)
            {
                BPTR input;
                LONG errorcode=0;

                input=Input();
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
                        
                Printf("About to format drive %s. ", arglist.device);
                Printf("This will destroy all data on the drive!\n");
                Printf("Are you sure? (y/N)"); Flush(Output());
        
                Read(Input(), &choice, 1);
                
                if(choice == 'y' || choice == 'Y')
                {
                    Printf("a");
                    if(Inhibit(arglist.device,DOSTRUE))
                    {
                    Printf("b");
                        {
                            struct TagItem tags[5];
                            struct TagItem *tag=tags;

                            tag->ti_Tag=ASF_NAME;
                            tag->ti_Data=(ULONG)arglist.name;
                            tag++;

                            if(arglist.casesensitive!=0)
                            {
                                tag->ti_Tag=ASF_CASESENSITIVE;
                                tag->ti_Data=TRUE;
                                tag++;
                            }

                            if(arglist.norecycled!=0)
                            {
                                tag->ti_Tag=ASF_NORECYCLED;
                                tag->ti_Data=TRUE;
                                tag++;
                            }

                            if(arglist.showrecycled!=0)
                            {
                                tag->ti_Tag=ASF_SHOWRECYCLED;
                                tag->ti_Data=TRUE;
                                tag++;
                            }

                            tag->ti_Tag=TAG_END;
                            tag->ti_Data=0;

#ifdef __AROS__
                            if((errorcode=AROS_DoPkt(IOFS, ACTION_SFS_FORMAT, (LONG)&tags, 0, 0, 0, 0))==DOSFALSE)
                            {
#else
                            if((errorcode=DoPkt(msgport, ACTION_SFS_FORMAT, (LONG)&tags, 0, 0, 0, 0))==DOSFALSE)
                            {
#endif
                                PrintFault(IoErr(),"error while initializing the drive");
                            }
                        }

                        Inhibit(arglist.device,DOSFALSE);
                    }
                    else {
                        PrintFault(IoErr(),"error while locking the drive");
                    }
                }
                else if(SetSignal(0L,SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C)
                {
                    PutStr("\n***Break\n");
                }
#ifdef __AROS__
                FreeVec(IOFS->io_PacketEmulation);
                DeleteIORequest((struct IORequest *)IOFS);
                DeleteMsgPort(msgport);
#endif  
            }
            else {
                VPrintf("Unknown device %s\n",&arglist.device);
                UnLockDosList(LDF_DEVICES|LDF_READ);
            }
            FreeArgs(readarg);
        }
        else {
            PutStr("wrong args!\n");
        }

        CloseLibrary((struct Library *)DOSBase);
    }
    return(0);
}
