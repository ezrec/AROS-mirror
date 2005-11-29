#include <dos/dos.h>
#include <dos/dosextens.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <utility/tagitem.h>

#include "/fs/packets.h"

static const char version[]={"\0$VER: SFSformat 1.1 " __AMIGADATE__ "\r\n"};

LONG main() {
  struct RDArgs *readarg;
  UBYTE template[]="DEVICE=DRIVE/A/K,NAME/A/K,CASESENSITIVE/S,NORECYCLED/S,SHOWRECYCLED/S";

  struct {char *device;
          char *name;
          ULONG casesensitive;
          ULONG norecycled;
          ULONG showrecycled;} arglist={NULL};

  if((DOSBase=(struct DosLibrary *)OpenLibrary("dos.library",37))!=0) {
    if((readarg=ReadArgs(template,(LONG *)&arglist,0))!=0) {
      struct MsgPort *msgport;
      struct DosList *dl;
      UBYTE *devname=arglist.device;

      while(*devname!=0) {
        if(*devname==':') {
          *devname=0;
          break;
        }
        devname++;
      }

      dl=LockDosList(LDF_DEVICES|LDF_READ);
      if((dl=FindDosEntry(dl,arglist.device,LDF_DEVICES))!=0) {
        BPTR input;
        LONG errorcode=0;

        input=Input();
        msgport=dl->dol_Task;
        UnLockDosList(LDF_DEVICES|LDF_READ);

        PutStr("Press RETURN to begin formatting or CTRL-C to abort: ");
        Flush(Output());

        if(IsInteractive(input)!=DOSFALSE) {
          for(;;) {
            if((WaitForChar(input,100)==DOSTRUE)) {
              if(errorcode==0 && (errorcode=DoPkt(msgport,ACTION_INHIBIT,DOSTRUE,0,0,0,0))!=DOSFALSE) {

                {
                  struct TagItem tags[5];
                  struct TagItem *tag=tags;

                  tag->ti_Tag=ASF_NAME;
                  tag->ti_Data=(ULONG)arglist.name;
                  tag++;

                  if(arglist.casesensitive!=0) {
                    tag->ti_Tag=ASF_CASESENSITIVE;
                    tag->ti_Data=TRUE;
                    tag++;
                  }

                  if(arglist.norecycled!=0) {
                    tag->ti_Tag=ASF_NORECYCLED;
                    tag->ti_Data=TRUE;
                    tag++;
                  }

                  if(arglist.showrecycled!=0) {
                    tag->ti_Tag=ASF_SHOWRECYCLED;
                    tag->ti_Data=TRUE;
                    tag++;
                  }

                  tag->ti_Tag=TAG_END;
                  tag->ti_Data=0;

                  if((errorcode=DoPkt(msgport, ACTION_SFS_FORMAT, (LONG)&tags, 0, 0, 0, 0))==DOSFALSE) {
                    PrintFault(IoErr(),"error while initializing the drive");
                  }
                }

                DoPkt(msgport,ACTION_INHIBIT,DOSFALSE,0,0,0,0);
              }
              else {
                PrintFault(IoErr(),"error while locking the drive");
              }
              break;
            }
            else if(SetSignal(0L,SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C) {
              PutStr("\n***Break\n");
              break;
            }
          }

          while(WaitForChar(input,0)==DOSTRUE) {
            FGetC(input);
          }
        }
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
