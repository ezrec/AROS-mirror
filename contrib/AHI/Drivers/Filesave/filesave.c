
#define NO_PROTOS
#define NO_SAS_PRAGMAS
#include <iffp/8svx.h>
#undef NO_PROTOS
#undef NO_SAS_PRAGMAS

#include <exec/exec.h>
#include <devices/ahi.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <graphics/gfxbase.h>
#include <libraries/ahi_sub.h>
#include <libraries/asl.h>
#include <datatypes/datatypes.h>
#include <datatypes/soundclass.h>
#include <proto/asl.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/datatypes.h>
#include <proto/ahi_sub.h>
#include <stdlib.h>
#include "filesave.h"

#define __bswap_short(x) \
            ((((x) & 0xff00) >>  8) | (((x) & 0x00ff) << 8))

#define __bswap_long(x) \
            ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | \
             (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

unsigned short
bswap_short( unsigned short x )
{
  return (unsigned short) __bswap_short( x );
}

unsigned long
bswap_long( unsigned long x )
{
  return __bswap_long( x );
}

#define dd ((struct filesave *) AudioCtrl->ahiac_DriverData)

#define SAVEBUFFERSIZE 100000   // in samples (min)
#define RECBUFFERSIZE  10000    // in samples

struct AIFCheader {
  ULONG                 FORMid;
  ULONG                 FORMsize;
  ULONG                 AIFCid;

  ULONG                 FVERid;
  ULONG                 FVERsize;
  FormatVersionHeader   FVERchunk;

  ULONG                 COMMid;
  ULONG                 COMMsize;
  ExtCommonChunk        COMMchunk;

  ULONG                 SSNDid;
  ULONG                 SSNDsize;
  SampledSoundHeader    SSNDchunk;
};

struct AIFFheader {
  ULONG                 FORMid;
  ULONG                 FORMsize;
  ULONG                 AIFFid;

  ULONG                 COMMid;
  ULONG                 COMMsize;
  CommonChunk           COMMchunk;

  ULONG                 SSNDid;
  ULONG                 SSNDsize;
  SampledSoundHeader    SSNDchunk;
};

struct EIGHTSVXheader {
  ULONG                 FORMid;
  ULONG                 FORMsize;
  ULONG                 EIGHTSVXid;

  ULONG                 VHDRid;
  ULONG                 VHDRsize;
  Voice8Header          VHDRchunk;

  ULONG                 BODYid;
  ULONG                 BODYsize;
};

struct WAVEheader {
  ULONG                 FORMid;
  ULONG                 FORMsize;
  ULONG                 WAVEid;

  ULONG                 FORMATid;
  ULONG                 FORMATsize;
  FormatChunk           FORMATchunk;
  
  ULONG                 DATAid;
  ULONG                 DATAsize;
};

extern char __far _LibID[];
extern char __far _LibName[];

extern void KPrintF(char *fmt,...);

#define FREQUENCIES 23
const ULONG  frequency[FREQUENCIES] =
{
  5513,
  6615,
  8000,     // µ- and A-Law
  9600,     // DAT/5
  11025,    // CD/4
  12000,    // DAT/4
  14700,    // CD/3
  16000,    // DAT/3
  17640,    // CD/2.5
  18900,
  19200,    // DAT/2.5
  22050,    // CD/2
  24000,    // DAT/2
  27429,
  29400,    // CD/1.5
  32000,    // DAT/1.5
  33075,
  37800,
  44056,    // Some kind of video rate
  44100,    // CD
  48000,    // DAT
  88200,    // CD*2
  96000     // DAT*2
};

extern void SlaveEntry(void);
extern void RecSlaveEntry(void);
void ulong2extended (ULONG in, extended *ex);

struct DosLibrary     *DOSBase        = NULL;
struct Library        *UtilityBase    = NULL;
struct Library        *AslBase        = NULL;
struct IntuitionBase  *IntuitionBase  = NULL;
struct Library        *AHIsubBase     = NULL;
struct Library        *DataTypesBase  = NULL;
struct GfxBase        *GfxBase        = NULL;

int  __saveds __asm __UserLibInit (register __a6 struct Library *libbase)
{
  AHIsubBase = libbase;

  if(!(DOSBase = (struct DosLibrary *) OpenLibrary("dos.library", 37)))
  {
    Alert(AN_Unknown|AG_OpenLib|AO_DOSLib);
    return 1;
  }

  if(!(GfxBase = (struct GfxBase *) OpenLibrary("graphics.library", 37)))
  {
    Alert(AN_Unknown|AG_OpenLib|AO_GraphicsLib);
    return 1;
  }

  if(!(IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 37)))
  {
    Alert(AN_Unknown|AG_OpenLib|AO_Intuition);
    return 1;
  }

  if(!(UtilityBase = OpenLibrary("utility.library", 37)))
  {
    Alert(AN_Unknown|AG_OpenLib|AO_UtilityLib);
    return 1;
  }

// Don't fail if these ones don't open!

  if(!(AslBase = OpenLibrary("asl.library", 37)))
  {
    struct EasyStruct req = {
        sizeof (struct EasyStruct), 0, _LibName,
        "Cannot open 'asl.library' v37", "OK"};

   	EasyRequest( NULL, &req, NULL, NULL );
  }

  DataTypesBase = OpenLibrary("datatypes.library",39);

  return 0;
}

void __saveds __asm __UserLibCleanup (register __a6 struct Library *libbase)
{
  if(AslBase)       { CloseLibrary(AslBase); AslBase = NULL; }
  if(DOSBase)       { CloseLibrary((struct Library *)DOSBase); DOSBase = NULL; }
  if(GfxBase)       { CloseLibrary((struct Library *)GfxBase); GfxBase = NULL; }
  if(IntuitionBase) { CloseLibrary((struct Library *)IntuitionBase); IntuitionBase = NULL; }
  if(UtilityBase)   { CloseLibrary(UtilityBase); UtilityBase = NULL; }
  if(DataTypesBase) { CloseLibrary(DataTypesBase); DataTypesBase = NULL; }
}

ULONG __asm __saveds intAHIsub_AllocAudio(
    register __a1 struct TagItem *tagList,
    register __a2 struct AHIAudioCtrlDrv *AudioCtrl )
{
  char *ext = "";

  if(AslBase == NULL)
  {
    return AHISF_ERROR;
  }

  if(AudioCtrl->ahiac_DriverData = AllocVec(sizeof(struct filesave),MEMF_CLEAR))
  {
    dd->fs_AHIsubBase       = AHIsubBase;
    dd->fs_DisableSignal    = -1;
    dd->fs_EnableSignal     = -1;
    dd->fs_SlaveSignal      = -1;
    dd->fs_MasterSignal     = AllocSignal(-1);
    dd->fs_MasterTask       = (struct Process *) FindTask(NULL);
    dd->fs_RecSlaveSignal   = -1;
    dd->fs_RecMasterSignal  = AllocSignal(-1);
  }
  else
  {
    return AHISF_ERROR;
  }

  if((dd->fs_MasterSignal == -1) || (dd->fs_RecMasterSignal == -1))
  {
    return AHISF_ERROR;
  }

  dd->fs_Format = GetTagData(AHIDB_FileSaveFormat, FORMAT_8SVX, tagList);

  switch(dd->fs_Format)
  {
    case FORMAT_8SVX:
      ext = ".8SVX";
      break;

    case FORMAT_AIFF:
      ext = ".AIFF";
      break;

    case FORMAT_AIFC:
      ext = ".AIFC";
      break;

    case FORMAT_S16:
      break;

    case FORMAT_WAVE:
      ext = ".WAV";
      break;

    default:
      break;
  }

  if(!(dd->fs_FileReq = AllocAslRequestTags(ASL_FileRequest,
      ASLFR_InitialFile,  ext,
      ASLFR_DoSaveMode,   TRUE,
      ASLFR_RejectIcons,  TRUE,
      ASLFR_TitleText,    _LibID,
      TAG_DONE)))
  {
    return AHISF_ERROR;
  }

  if(!(dd->fs_RecFileReq = AllocAslRequestTags(ASL_FileRequest,
      ASLFR_RejectIcons,  TRUE,
      ASLFR_TitleText,    "Select a sound sample",
      TAG_DONE)))
  {
    return AHISF_ERROR;
  }

  return AHISF_KNOWHIFI|AHISF_KNOWSTEREO|AHISF_CANRECORD|AHISF_MIXING|AHISF_TIMING;
}

void __asm __saveds intAHIsub_FreeAudio(
    register __a2 struct AHIAudioCtrlDrv *AudioCtrl )
{
  if(AudioCtrl->ahiac_DriverData)
  {
    FreeAslRequest(dd->fs_FileReq);
    FreeAslRequest(dd->fs_RecFileReq);
    FreeSignal(dd->fs_MasterSignal);
    FreeSignal(dd->fs_RecMasterSignal);
    FreeVec(AudioCtrl->ahiac_DriverData);
    AudioCtrl->ahiac_DriverData = NULL;
  }
}

ULONG __asm __saveds intAHIsub_Start(
    register __d0 ULONG Flags,
    register __a2 struct AHIAudioCtrlDrv *AudioCtrl )
{
  AHIsub_Stop(Flags, AudioCtrl);

  if(Flags & AHISF_PLAY)
  {
    ULONG savebufferlength;

    if(!(dd->fs_MixBuffer = AllocVec(AudioCtrl->ahiac_BuffSize, MEMF_ANY)))
      return AHIE_NOMEM;

    dd->fs_SaveBufferSize = AudioCtrl->ahiac_MaxBuffSamples;

    // S16 has two buffers (L/R) instead
    if((AudioCtrl->ahiac_Flags & AHIACF_STEREO) && dd->fs_Format != FORMAT_S16)
    {
      dd->fs_SaveBufferSize <<=1;
    }

    if(dd->fs_SaveBufferSize < SAVEBUFFERSIZE)
    {
      dd->fs_SaveBufferSize = SAVEBUFFERSIZE;
    }

    savebufferlength = dd->fs_SaveBufferSize;


    switch(dd->fs_Format)
    {
      case FORMAT_8SVX:
        break;

      case FORMAT_AIFF:
        savebufferlength <<= 1;
        break;

      case FORMAT_AIFC:
        savebufferlength <<= 1;
        break;

      case FORMAT_S16:
        savebufferlength <<= 1;
        break;

      case FORMAT_WAVE:
        savebufferlength <<= 1;
        break;

      default:
        break;
    }


    if(!(dd->fs_SaveBuffer = AllocVec(savebufferlength, MEMF_ANY)))
    {
      return AHIE_NOMEM;
    }

    if ((AudioCtrl->ahiac_Flags & AHIACF_STEREO) && dd->fs_Format == FORMAT_S16)
    {
      if(!(dd->fs_SaveBuffer2 = AllocVec(savebufferlength, MEMF_ANY)))
      {
        return AHIE_NOMEM;
      }
    }

    if(AslRequestTags(dd->fs_FileReq,TAG_DONE))
    {

      Forbid();

      if(dd->fs_SlaveTask = CreateNewProcTags(
          NP_Entry,     SlaveEntry,
          NP_Name,      _LibName,
          NP_Priority,  -1,               // It's a number cruncher...
          NP_StackSize, 10000,
          TAG_DONE))
      {
        dd->fs_SlaveTask->pr_Task.tc_UserData = AudioCtrl;
      }

      Permit();

      if(dd->fs_SlaveTask)
      {
        Wait(1L<<dd->fs_MasterSignal);  // Wait for slave to come alive
        if(dd->fs_SlaveTask == NULL)    // Is slave alive or dead?
        {
          return AHIE_UNKNOWN;
        }
      }
      else
      {
        return AHIE_NOMEM;
      }
    }
    else
    {
      if(IoErr())
      {
        return AHIE_NOMEM;    //error occured
      }
      else
      {
        return AHIE_ABORTED;  //requester cancelled
      }
    }
  }

  if(Flags & AHISF_RECORD)
  {
    if(!(dd->fs_RecBuffer = AllocVec(RECBUFFERSIZE*4,MEMF_ANY)))
    {
      return AHIE_NOMEM;
    }

    if(AslRequestTags(dd->fs_RecFileReq,TAG_DONE))
    {
      Delay(TICKS_PER_SECOND);         // Wait for window to close etc...

      Forbid();

      if(dd->fs_RecSlaveTask = CreateNewProcTags(
          NP_Entry,     RecSlaveEntry,
          NP_Name,      _LibName,
          NP_Priority,  1,               // Make it steady...
          TAG_DONE))
      {
        dd->fs_RecSlaveTask->pr_Task.tc_UserData = AudioCtrl;
      }

      Permit();

      if(dd->fs_RecSlaveTask)
      {
        Wait(1L<<dd->fs_RecMasterSignal);  // Wait for slave to come alive
        if(dd->fs_RecSlaveTask == NULL)    // Is slave alive or dead?
        {
          return AHIE_UNKNOWN;
        }
      }
      else
      {
        return AHIE_NOMEM;
      }
    }
    else
    {
      if(IoErr())
      {
        return AHIE_NOMEM;    //error occured
      }
      else
      {
        return AHIE_ABORTED;  //requester cancelled
      }
    }
  }

  return AHIE_OK;
}


void __asm __saveds intAHIsub_Update(
    register __d0 ULONG Flags,
    register __a2 struct AHIAudioCtrlDrv *AudioCtrl )
{
}


void __asm __saveds intAHIsub_Stop(
    register __d0 ULONG Flags,
    register __a2 struct AHIAudioCtrlDrv *AudioCtrl )
{
  if(Flags & AHISF_PLAY)
  {
    if(dd->fs_SlaveTask)
    {
      if(dd->fs_SlaveSignal != -1)
      {
        Signal((struct Task *)dd->fs_SlaveTask,1L<<dd->fs_SlaveSignal); // Kill him!
      }
      Wait(1L<<dd->fs_MasterSignal);  // Wait for slave to die
    }
    FreeVec(dd->fs_MixBuffer);
    dd->fs_MixBuffer = NULL;
    FreeVec(dd->fs_SaveBuffer);
    FreeVec(dd->fs_SaveBuffer2);
    dd->fs_SaveBuffer = NULL;
    dd->fs_SaveBuffer2 = NULL;
  }

  if(Flags & AHISF_RECORD)
  {
    if(dd->fs_RecSlaveTask)
    {
      if(dd->fs_RecSlaveSignal != -1)
      {
        Signal((struct Task *)dd->fs_RecSlaveTask,1L<<dd->fs_RecSlaveSignal); // Kill him!
      }
      Wait(1L<<dd->fs_RecMasterSignal);  // Wait for slave to die
    }
    FreeVec(dd->fs_RecBuffer);
    dd->fs_RecBuffer = NULL;
  }
}



LONG __asm __saveds intAHIsub_GetAttr(
    register __d0 ULONG Attribute,
    register __d1 LONG Argument,
    register __d2 LONG Default,
    register __a1 struct TagItem *tagList,
    register __a2 struct AHIAudioCtrlDrv *AudioCtrl)
{
  int i;

  switch(Attribute)
  {
    case AHIDB_Bits:
      switch (GetTagData(AHIDB_FileSaveFormat,FORMAT_8SVX,tagList))
      {
        case FORMAT_8SVX:
          return 8;

        case FORMAT_AIFF:
          return 16;

        case FORMAT_AIFC:
          return 16;

        case FORMAT_S16:
          return 16;

        case FORMAT_WAVE:
          return 16;

        default:
          return Default;
      }

    case AHIDB_Frequencies:
      return FREQUENCIES;

    case AHIDB_Frequency: // Index->Frequency
      return (LONG) frequency[Argument];

    case AHIDB_Index: // Frequency->Index
      if(Argument <= frequency[0])
      {
        return 0;
      }
      if(Argument >= frequency[FREQUENCIES-1])
      {
        return FREQUENCIES-1;
      }
      for(i = 1;i<FREQUENCIES;i++)
      {
        if(frequency[i]>Argument)
        {
          if( (Argument-frequency[i-1]) < (frequency[i]-Argument) )
          {
            return i-1;
          }
          else
          {
            return i;
          }
        }
      }
      return 0;  // Will not happen

    case AHIDB_Author:
      return (LONG) "Martin 'Leviticus' Blom";

    case AHIDB_Copyright:
      return (LONG) "Public Domain";

    case AHIDB_Version:
      return (LONG) _LibID;

    case AHIDB_Record:
      return TRUE;

    case AHIDB_FullDuplex:
      return TRUE;

    case AHIDB_MaxRecordSamples:
      return RECBUFFERSIZE;

    case AHIDB_Realtime:
      return FALSE;

    case AHIDB_Inputs:
      return 1;

    case AHIDB_Input:
      return (LONG) "File";    // We have only one input!

    case AHIDB_Outputs:
      return 1;

    case AHIDB_Output:
      return (LONG) "File";    // We have only one output!

    default:
      return Default;
  }
}


ULONG __asm __saveds intAHIsub_HardwareControl(
    register __d0 ULONG attribute,
    register __d1 LONG argument,
    register __a2 struct AHIAudioCtrlDrv *AudioCtrl )
{
  return NULL;
}

/*
** Unused LVOs follows...
*/

ULONG __asm __saveds intAHIsub_SetVol(
    register __d0 UWORD channel,
    register __d1 Fixed volume,
    register __d2 sposition pan,
    register __a2 struct AHIAudioCtrlDrv *AudioCtrl,
    register __d3 ULONG Flags)
{
  return AHIS_UNKNOWN;
}

ULONG __asm __saveds intAHIsub_SetFreq(
    register __d0 UWORD channel,
    register __d1 ULONG freq,
    register __a2 struct AHIAudioCtrlDrv *AudioCtrl,
    register __d2 ULONG flags )
{
  return AHIS_UNKNOWN;
}

ULONG __asm __saveds intAHIsub_SetSound(
    register __d0 UWORD channel,
    register __d1 UWORD sound,
    register __d2 ULONG offset,
    register __d3 LONG length,
    register __a2 struct AHIAudioCtrlDrv *AudioCtrl,
    register __d4 ULONG flags )
{
  return AHIS_UNKNOWN;
}


ULONG __asm __saveds intAHIsub_SetEffect (
    register __a0 APTR effect,
    register __a2 struct AHIAudioCtrlDrv *AudioCtrl )
{
  return AHIS_UNKNOWN;
}

ULONG __asm __saveds intAHIsub_LoadSound(
    register __d0 UWORD sound,
    register __d1 ULONG type,
    register __a0 APTR info,
    register __a2 struct AHIAudioCtrlDrv *AudioCtrl )
{
  return AHIS_UNKNOWN;
}

ULONG __asm __saveds intAHIsub_UnloadSound(
    register __d0 UWORD sound,
    register __a2 struct AHIAudioCtrlDrv *AudioCtrl )
{
  return AHIS_UNKNOWN;
}



/*
** The slave process
*/

void __asm __saveds SlaveTask(register __a2 struct AHIAudioCtrlDrv *AudioCtrl)
// SlaveEntry() will set up register a2 and a6 for us.
{
  struct EIGHTSVXheader EIGHTSVXheader = // All NULLs will be filled later.
  { 
    ID_FORM, NULL, ID_8SVX,
    ID_VHDR, sizeof(Voice8Header),
    {
      NULL,
      0,
      0,
      NULL,
      1,
      sCmpNone,
      0x10000
    },
    ID_BODY, NULL
  };

  struct AIFFheader AIFFheader = // All NULLs will be filled later.
  { 
    ID_FORM, NULL, ID_AIFF,
    ID_COMM, sizeof(CommonChunk),
    {
      NULL,
      NULL,
      16,
      {
        NULL
      }
    },
    ID_SSND, NULL,
    {
      0,
      0
    }
  };

  struct AIFCheader AIFCheader = // All NULLs will be filled later.
  { 
    ID_FORM, NULL, ID_AIFC,
    ID_FVER, sizeof(FormatVersionHeader), 
    {
      AIFCVersion1
    },
    ID_COMM, sizeof(ExtCommonChunk),
    {
      NULL,
      NULL,
      16,
      {
        NULL
      },
      NO_COMPRESSION,
      sizeof("not compressed") - 1,
      'n','o','t',' ','c','o','m','p','r','e','s','s','e','d'
    },
    ID_SSND, NULL,
    {
      0,
      0
    }
  };

  struct STUDIO16FILE S16header = // All NULLs will be filled later.
  {
    S16FID,
    NULL,
    S16FINIT,
    S16_VOL_0,
    0,
    0,
    NULL,
    0,
    0,
    NULL,
    NULL,
    0,
    NULL,
    0,
    {
      0
    }
  };

  struct WAVEheader WAVEheader = // All NULLs will be filled later.
  {
    ID_RIFF, NULL, ID_WAVE,
    ID_fmt, __bswap_long( sizeof(FormatChunk) ),
    {
      __bswap_short( WAVE_PCM ),
      0,
      0,
      0,
      0,
      __bswap_short( 16 )
    },
    ID_data, NULL
  };

  struct EasyStruct req =
  {
    sizeof (struct EasyStruct),
    0,
    _LibID,
    "Rendering finished.\nTo futher improve the quality of the sample,\n"
    "you can raise the volume to %ld%%%sand render again.",
    "OK",
  };

  BPTR lock = NULL,cd = NULL,file = NULL, file2 = NULL;
  ULONG signals, i, maxVolume = 0, samplesAdd =0, samples = 0, length = 0;
  ULONG offset = 0, bytesInBuffer = 0, samplesWritten = 0, bytesWritten = 0;

// We cannot handle stereo 8SVXs!
  if( (dd->fs_Format == FORMAT_8SVX) &&
      (AudioCtrl->ahiac_Flags & AHIACF_STEREO) )
  {
    goto quit;
  }

  if((dd->fs_DisableSignal = AllocSignal(-1)) == -1)
  {
    goto quit;
  }
  if((dd->fs_EnableSignal = AllocSignal(-1)) == -1)
  {
    goto quit;
  }
  if((dd->fs_SlaveSignal = AllocSignal(-1)) == -1)
  {
    goto quit;
  }

  if(!(lock = Lock(dd->fs_FileReq->fr_Drawer, ACCESS_READ)))
  {
    goto quit;
  }

  cd = CurrentDir(lock);

  switch(dd->fs_Format)
  {
    case FORMAT_8SVX:
      if(!(file = Open(dd->fs_FileReq->fr_File, MODE_NEWFILE))) goto quit;
      Write(file, &EIGHTSVXheader, sizeof EIGHTSVXheader);
      break;

    case FORMAT_AIFF:
      if(!(file = Open(dd->fs_FileReq->fr_File, MODE_NEWFILE))) goto quit;
      Write(file, &AIFFheader, sizeof AIFFheader);
      break;

    case FORMAT_AIFC:
      if(!(file = Open(dd->fs_FileReq->fr_File, MODE_NEWFILE))) goto quit;
      Write(file, &AIFCheader, sizeof AIFCheader);
      break;

    case FORMAT_S16:
      if (AudioCtrl->ahiac_Flags & AHIACF_STEREO)
      {
        char filename[256];
        int len;

        strncpy (filename, dd->fs_FileReq->fr_File, sizeof(filename) - 3);
        len = strlen(filename);

        if(len >= 2 && filename[len - 2] == '_'
           && (filename[len - 1] == 'L' || filename[len - 1] == 'R'))
        {
          filename[len - 1] = 'L';
        }
        else
        {
          strcat (filename, "_L");
        }

        if(!(file = Open(filename, MODE_NEWFILE))) goto quit;

        filename[strlen(filename) - 1] = 'R';
        if(!(file2 = Open(filename, MODE_NEWFILE))) goto quit;

        Write(file, &S16header, sizeof S16header);
        Write(file2, &S16header, sizeof S16header);
      }
      else
      {
        if(!(file = Open(dd->fs_FileReq->fr_File, MODE_NEWFILE))) goto quit;
        Write(file, &S16header, sizeof S16header);
      }
      break;

    case FORMAT_WAVE:
      if(!(file = Open(dd->fs_FileReq->fr_File, MODE_NEWFILE))) goto quit;
      Write(file, &WAVEheader, sizeof WAVEheader);
      break;
  }

  // Everything set up. Tell Master we're alive and healthy.
  Signal((struct Task *)dd->fs_MasterTask,1L<<dd->fs_MasterSignal);

  for(;;)
  {
    signals = SetSignal(0L,0L);
    if(signals & (SIGBREAKF_CTRL_C | 1L<<dd->fs_SlaveSignal))
    {
      break;
    }

    if(signals & (1L<<dd->fs_EnableSignal | 1L<<dd->fs_DisableSignal) == 1L<<dd->fs_DisableSignal)
    {
      Wait(1L<<dd->fs_EnableSignal);
    }

    CallHookPkt(AudioCtrl->ahiac_PlayerFunc, AudioCtrl, NULL);
    CallHookPkt(AudioCtrl->ahiac_MixerFunc, AudioCtrl, dd->fs_MixBuffer);

    samplesAdd = AudioCtrl->ahiac_BuffSamples;
    samples    = samplesAdd;

    if(AudioCtrl->ahiac_Flags & AHIACF_STEREO)
    {
      samples <<= 1;
    }

// Search for loudest part in sample
    if(AudioCtrl->ahiac_Flags & AHIACF_HIFI)
    {
      for(i = 0; i < samples; i++)
        if(abs(((LONG *)dd->fs_MixBuffer)[i]) > maxVolume)
          maxVolume = abs(((LONG *)dd->fs_MixBuffer)[i]);
    }
    else
    {
      for(i = 0; i< samples; i++)
        if(abs(((WORD *)dd->fs_MixBuffer)[i]) > maxVolume)
          maxVolume = abs(((WORD *)dd->fs_MixBuffer)[i]);
    }

    if((AudioCtrl->ahiac_Flags & AHIACF_STEREO) && dd->fs_Format == FORMAT_S16)
    {
      samples >>= 1;  // Two buffers instead
    }

    if(offset+samples >= dd->fs_SaveBufferSize)
    {
      if(Write(file, dd->fs_SaveBuffer, bytesInBuffer) != bytesInBuffer)
      {
        break;
      }
      if(file2 != NULL) {
        if(Write(file2, dd->fs_SaveBuffer2, bytesInBuffer) != bytesInBuffer)
        {
          break;
        }
      }
      offset = 0;
      bytesInBuffer = 0;
    }

    switch(dd->fs_Format)
    {
      case FORMAT_8SVX:
        if(AudioCtrl->ahiac_Flags & AHIACF_HIFI)
        {
          BYTE *dest = &((BYTE *) dd->fs_SaveBuffer)[offset];
          LONG *source = dd->fs_MixBuffer;

          for(i = 0; i < samples; i++)
            *dest++ = *source++ >> 24;
        }
        else
        {
          BYTE *dest = &((BYTE *) dd->fs_SaveBuffer)[offset];
          WORD *source = dd->fs_MixBuffer;

          for(i = 0; i < samples; i++)
            *dest++ = *source++ >> 8;
        }
        length = samples;
        break;

      case FORMAT_AIFF:
      case FORMAT_AIFC:
        if(AudioCtrl->ahiac_Flags & AHIACF_HIFI)
        {
          WORD *dest = &((WORD *) dd->fs_SaveBuffer)[offset];
          LONG *source = dd->fs_MixBuffer;

          for(i = 0; i < samples; i++)
          {
            *dest++ = *source++ >> 16;
          }
        }
        else
        {
          WORD *dest = &((WORD *) dd->fs_SaveBuffer)[offset];
          WORD *source = dd->fs_MixBuffer;

          for(i = 0; i < samples; i++)
          {
            *dest++ = *source++;
          }
        }
        length = samples*2;
        break;

      case FORMAT_S16:
        switch(AudioCtrl->ahiac_Flags & (AHIACF_HIFI | AHIACF_STEREO))
        {
          case 0:
          {
            WORD *dest = &((WORD *) dd->fs_SaveBuffer)[offset];
            WORD *source = dd->fs_MixBuffer;

            for(i = 0; i < samples; i++)
            {
              *dest++ = *source++;
            }

            break;
          }

          case AHIACF_STEREO:
          {
            WORD *dest1 = &((WORD *) dd->fs_SaveBuffer)[offset];
            WORD *dest2 = &((WORD *) dd->fs_SaveBuffer2)[offset];
            WORD *source = dd->fs_MixBuffer;

            for(i = 0; i < samples; i++)
            {
              *dest1++ = *source++;
              *dest2++ = *source++;
            }

            break;
          }

          case AHIACF_HIFI:
          {
            WORD *dest = &((WORD *) dd->fs_SaveBuffer)[offset];
            LONG *source = dd->fs_MixBuffer;

            for(i = 0; i < samples; i++)
            {
              *dest++ = *source++ >> 16;
            }

            break;
          }

          case (AHIACF_HIFI | AHIACF_STEREO):
          {
            WORD *dest1 = &((WORD *) dd->fs_SaveBuffer)[offset];
            WORD *dest2 = &((WORD *) dd->fs_SaveBuffer2)[offset];
            LONG *source = dd->fs_MixBuffer;

            for(i = 0; i < samples; i++)
            {
              *dest1++ = *source++ >> 16;
              *dest2++ = *source++ >> 16;
            }

            break;
          }
        }

        length = samples*2;
        break;

      case FORMAT_WAVE:
        if(AudioCtrl->ahiac_Flags & AHIACF_HIFI)
        {
          WORD *dest = &((WORD *) dd->fs_SaveBuffer)[offset];
          LONG *source = dd->fs_MixBuffer;

          for(i = 0; i < samples; i++)
          {
            *dest++ = bswap_short( *source++ >> 16 );
          }
        }
        else
        {
          WORD *dest = &((WORD *) dd->fs_SaveBuffer)[offset];
          WORD *source = dd->fs_MixBuffer;

          for(i = 0; i < samples; i++)
          {
            *dest++ = bswap_short( *source++ );
          }
        }
        length = samples*2;
        break;
    }

    offset          += samples;
    samplesWritten  += samplesAdd;
    bytesWritten    += length;
    bytesInBuffer   += length;
  }

  Write(file, dd->fs_SaveBuffer, bytesInBuffer);
  if(file2 != NULL)
  {
    Write(file2, dd->fs_SaveBuffer2, bytesInBuffer);
  }

  switch(dd->fs_Format)
  {
    case FORMAT_8SVX:
      EIGHTSVXheader.FORMsize = sizeof(EIGHTSVXheader)-8+bytesWritten;
      EIGHTSVXheader.VHDRchunk.oneShotHiSamples = samplesWritten;
      EIGHTSVXheader.VHDRchunk.samplesPerSec = AudioCtrl->ahiac_MixFreq;
      EIGHTSVXheader.BODYsize = bytesWritten;
      if(bytesWritten & 1)
        FPutC(file,'\0');   // Pad to even
      Seek(file,0,OFFSET_BEGINNING);
      Write(file,&EIGHTSVXheader,sizeof EIGHTSVXheader);
      break;

    case FORMAT_AIFF:
      AIFFheader.FORMsize = sizeof(AIFFheader)-8+bytesWritten;
      AIFFheader.COMMchunk.numChannels = (AudioCtrl->ahiac_Flags & AHIACF_STEREO ? 2 : 1);
      AIFFheader.COMMchunk.numSampleFrames = samplesWritten;
      ulong2extended(AudioCtrl->ahiac_MixFreq,&AIFFheader.COMMchunk.sampleRate);
      AIFFheader.SSNDsize = sizeof(SampledSoundHeader)+bytesWritten;
      Seek(file,0,OFFSET_BEGINNING);
      Write(file,&AIFFheader,sizeof AIFFheader);
      break;

    case FORMAT_AIFC:
      AIFCheader.FORMsize = sizeof(AIFCheader)-8+bytesWritten;
      AIFCheader.COMMchunk.numChannels = (AudioCtrl->ahiac_Flags & AHIACF_STEREO ? 2 : 1);
      AIFCheader.COMMchunk.numSampleFrames = samplesWritten;
      ulong2extended(AudioCtrl->ahiac_MixFreq,&AIFCheader.COMMchunk.sampleRate);
      AIFCheader.SSNDsize = sizeof(SampledSoundHeader)+bytesWritten;
      Seek(file,0,OFFSET_BEGINNING);
      Write(file,&AIFCheader,sizeof AIFCheader);
      break;

    case FORMAT_S16:
      S16header.S16F_RATE = AudioCtrl->ahiac_MixFreq;
      S16header.S16F_SAMPLES0 =
      S16header.S16F_SAMPLES1 = samplesWritten;
      S16header.S16F_SAMPLES2 = samplesWritten - 1;
      if (file2 == NULL)
      {
        S16header.S16F_PAN = S16_PAN_MID;
      }
      else
      {
        S16header.S16F_PAN = S16_PAN_LEFT;
      }

      Seek(file, 0, OFFSET_BEGINNING);
      Write(file, &S16header, sizeof S16header);
      if(file2 != NULL)
      {
        S16header.S16F_PAN = S16_PAN_RIGHT;
        Seek(file2,0,OFFSET_BEGINNING);
        Write(file2, &S16header, sizeof S16header);
      }
      break;   

    case FORMAT_WAVE:
    {
      short num_channels;
      short block_align;
      
      num_channels = AudioCtrl->ahiac_Flags & AHIACF_STEREO ? 2 : 1;
      block_align  = num_channels * 16 / 8;
      
      WAVEheader.FORMsize                   = bswap_long( sizeof(WAVEheader)-8+bytesWritten );
      WAVEheader.FORMATchunk.numChannels    = bswap_short( num_channels );
      WAVEheader.FORMATchunk.samplesPerSec  = bswap_long( AudioCtrl->ahiac_MixFreq );
      WAVEheader.FORMATchunk.avgBytesPerSec = bswap_long( AudioCtrl->ahiac_MixFreq * block_align );
      WAVEheader.FORMATchunk.blockAlign     = bswap_short( block_align );
      WAVEheader.DATAsize                   = bswap_long( bytesWritten );
      Seek(file,0,OFFSET_BEGINNING);
      Write(file,&WAVEheader,sizeof WAVEheader);
      break;
    }
  }

  if(AudioCtrl->ahiac_Flags & AHIACF_HIFI)
    maxVolume >>=16;

  if(maxVolume != 0)
  {
    EasyRequest(NULL, &req, NULL, 3276800/maxVolume,
      AudioCtrl->ahiac_MixFreq<frequency[FREQUENCIES-1] ? ",\nincrease the mixing frequency " : "\n");
  }

quit:
  if(file)
  {
    Close(file);
  }
  if(file2)
  {
    Close(file2);
  }
  if(lock)
  {
    CurrentDir(cd);
    UnLock(lock);
  }

  Forbid();
  dd->fs_SlaveTask = NULL;
  FreeSignal(dd->fs_DisableSignal);
  FreeSignal(dd->fs_EnableSignal);
  FreeSignal(dd->fs_SlaveSignal);
  dd->fs_DisableSignal  = -1;
  dd->fs_EnableSignal   = -1;
  dd->fs_SlaveSignal    = -1;
  // Tell the Master we're dying
  Signal((struct Task *)dd->fs_MasterTask,1L<<dd->fs_MasterSignal);
  // Multitaking will resume when we are dead.
}

/*
** Apple's 80-bit SANE extended has the following format:

 1       15      1            63
+-+-------------+-+-----------------------------+
|s|       e     |i|            f                |
+-+-------------+-+-----------------------------+
  msb        lsb   msb                       lsb

The value v of the number is determined by these fields as follows:
If 0 <= e < 32767,              then v = (-1)^s * 2^(e-16383) * (i.f).
If e == 32767 and f == 0,       then v = (-1)^s * (infinity), regardless of i.
If e == 32767 and f != 0,       then v is a NaN, regardless of i.
*/

void ulong2extended (ULONG in, extended *ex)
{
  ex->exponent = 31+16383;
  ex->mantissa[1] = 0;
  while(!(in & 0x80000000))
  {
    ex->exponent--;
    in <<= 1;
  }
  ex->mantissa[0] = in;
}



/*
** The record slave process
*/

void __asm __saveds RecSlaveTask(register __a2 struct AHIAudioCtrlDrv *AudioCtrl)
// RecSlaveEntry() will set up register a2 and a6 for us.
{
  ULONG   signals;
  BPTR    lock = NULL,cd,file = NULL;
  Object *o = NULL;
  BYTE   *samples = NULL;
  ULONG   length = NULL;
  ULONG   count = 0,offs = 0,i;

  struct AHIRecordMessage RecordMessage = 
  {
    AHIST_S16S,
    NULL,
    RECBUFFERSIZE
  };

  RecordMessage.ahirm_Buffer = dd->fs_RecBuffer;

  if(!(lock = Lock(dd->fs_RecFileReq->fr_Drawer,ACCESS_READ)))
    goto quit;
  cd = CurrentDir(lock);

  if(DataTypesBase)
  {
    if (!(o = NewDTObject (dd->fs_RecFileReq->fr_File,
        DTA_GroupID,GID_SOUND,
        TAG_DONE)))
      goto quit;

    GetDTAttrs(o,
      SDTA_Sample,&samples,
      SDTA_SampleLength,&length,
      TAG_DONE);
  }
  else // datatypes.library not open. Open the selected file as raw 8 bit signed instead.
  {
    if(!(file = Open(dd->fs_RecFileReq->fr_File,MODE_OLDFILE)))
      goto quit;
    Seek(file,0,OFFSET_END);
    length = Seek(file,0,OFFSET_BEGINNING);
    if(!(samples = AllocVec(length,MEMF_ANY)))
      goto quit;
    if(length != Read(file,samples,length))
      goto quit;
  }

  if(!samples || !length )
    goto quit;

  if((dd->fs_RecSlaveSignal = AllocSignal(-1)) == -1)
    goto quit;

// Everything set up. Tell Master we're alive and healthy.
    Signal((struct Task *)dd->fs_MasterTask,1L<<dd->fs_RecMasterSignal);

    for(;;)
    {
      signals = SetSignal(0L,0L);
      if(signals & (SIGBREAKF_CTRL_C | 1L<<dd->fs_RecSlaveSignal))
        break;

      for(;;)
      {
        if(count+RECBUFFERSIZE-offs < length)
        {
// End of sample will not be reached; just fill to the end of dd->fs_RecBuffer.
          for(i = RECBUFFERSIZE-offs;i>0;i--)
          {
            dd->fs_RecBuffer[(offs)<<1] = 
            dd->fs_RecBuffer[((offs++)<<1)+1] = 
            samples[count++]<<8;
          }
          offs = 0;
          break;
        }
        else
        {
// End of sample will be reached. Fill part of buffer, and iterate (== don't break).
          for(i = length-count;i>0;i--)
          {
            dd->fs_RecBuffer[(offs)<<1] = 
            dd->fs_RecBuffer[((offs++)<<1)+1] = 
            samples[count++]<<8;
          }
          count = 0;
        }

      }

      CallHookPkt(AudioCtrl->ahiac_SamplerFunc,AudioCtrl,&RecordMessage);
      Delay(50*RECBUFFERSIZE/AudioCtrl->ahiac_MixFreq);
    }

quit:
// Get rid of object
  if(DataTypesBase)
  {
    if(o)
      DisposeDTObject (o);
  }
  else // datatypes.library not open.
  {
    if(samples)
      FreeVec(samples);
    if(file)
      Close(file);
  }
  CurrentDir(cd);
  if(lock)
    UnLock(lock);

  Forbid();
  dd->fs_RecSlaveTask = NULL;
  FreeSignal(dd->fs_RecSlaveSignal);
  dd->fs_RecSlaveSignal = -1;
  // Tell the Master we're dying
  Signal((struct Task *)dd->fs_MasterTask,1L<<dd->fs_RecMasterSignal);
  // Multitaking will resume when we are dead.
}
