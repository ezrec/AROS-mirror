/*
** This program uses the device interface to play a sampled sound.
** The input is read from THE DEFAULT INPUT, make sure you
** start it with "PlayTest pri < mysample.raw" !
** Where pri is a number from -128 to +127 (may be omitted)
** The sample should be 8 bit signed, mono (see TYPE).
*/

#include <devices/ahi.h>
#include <dos/dosasl.h>
#include <exec/memory.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/ahi.h>
#include <stdlib.h>

#define FREQUENCY  8000
#define TYPE       AHIST_M8S
#define BUFFERSIZE 20000

struct MsgPort    *AHImp     = NULL;
struct AHIRequest *AHIio     = NULL;
struct AHIRequest *AHIio2    = NULL;
BYTE               AHIDevice = -1;

__far BYTE buffer1[BUFFERSIZE];
__far BYTE buffer2[BUFFERSIZE];

void cleanup(LONG rc)
{
  if(!AHIDevice)
    CloseDevice((struct IORequest *)AHIio);
  DeleteIORequest((struct IORequest *)AHIio);
  DeleteIORequest((struct IORequest *)AHIio2);
  DeleteMsgPort(AHImp);
  exit(rc);
}

void main(int argc, char *argv[])
{
  BYTE *p1=buffer1,*p2=buffer2;
  void *tmp;
  ULONG signals,length;
  struct AHIRequest *link = NULL;
  LONG priority = 0;
  BYTE pri;

  if(argc == 2)
  {
    StrToLong(argv[1], &priority);
  }
  pri = priority;
  Printf("Sound priority: %ld\n", pri);

  if(AHImp=CreateMsgPort()) {
    if(AHIio=(struct AHIRequest *)CreateIORequest(AHImp,sizeof(struct AHIRequest))) {
      AHIio->ahir_Version = 4;
      AHIDevice=OpenDevice(AHINAME,0,(struct IORequest *)AHIio,NULL);
    }
  }

  if(AHIDevice) {
    Printf("Unable to open %s/0 version 4\n",AHINAME);
    cleanup(RETURN_FAIL);
  }

// Make a copy of the request (for double buffering)
  AHIio2 = AllocMem(sizeof(struct AHIRequest), MEMF_ANY);
  if(! AHIio2) {
    cleanup(RETURN_FAIL);
  }
  CopyMem(AHIio, AHIio2, sizeof(struct AHIRequest));

  SetIoErr(0);

  for(;;) {

// Fill buffer
    length = Read(Input(),p1,BUFFERSIZE);

// Play buffer
    AHIio->ahir_Std.io_Message.mn_Node.ln_Pri = pri;
    AHIio->ahir_Std.io_Command  = CMD_WRITE;
    AHIio->ahir_Std.io_Data     = p1;
    AHIio->ahir_Std.io_Length   = length;
    AHIio->ahir_Std.io_Offset   = 0;
    AHIio->ahir_Frequency       = FREQUENCY;
    AHIio->ahir_Type            = TYPE;
    AHIio->ahir_Volume          = 0x10000;          // Full volume
    AHIio->ahir_Position        = 0x8000;           // Centered
    AHIio->ahir_Link            = link;
    SendIO((struct IORequest *) AHIio);

    if(link) {

// Wait until the last buffer is finished (== the new buffer is started)
      signals=Wait(SIGBREAKF_CTRL_C | (1L << AHImp->mp_SigBit));

// Check for Ctrl-C and abort if pressed
      if(signals & SIGBREAKF_CTRL_C) {
        SetIoErr(ERROR_BREAK);
        break;
      }

// Remove the reply and abort on error
      if(WaitIO((struct IORequest *) link)) {
        SetIoErr(ERROR_WRITE_PROTECTED);
        break;
      }
    }

// Check for end-of-sound, and wait until it is finished before aborting
    if(length != BUFFERSIZE) {
      WaitIO((struct IORequest *) AHIio);
      break;
    }

    link = AHIio;

// Swap buffer and request pointers, and restart
    tmp    = p1;
    p1     = p2;
    p2     = tmp;

    tmp    = AHIio;
    AHIio  = AHIio2;
    AHIio2 = tmp;
  }
  

// Abort any pending iorequests
  AbortIO((struct IORequest *) AHIio);
  WaitIO((struct IORequest *) AHIio);

  if(link) { // Only if the second request was started
    AbortIO((struct IORequest *) AHIio2);
    WaitIO((struct IORequest *) AHIio2);
  }

  if(IoErr()) {
    PrintFault(IoErr(), argv[0] );
    cleanup(RETURN_ERROR);
  }

  cleanup(RETURN_OK);
}
