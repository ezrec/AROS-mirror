/*
     AHI - The AHI preferences program
     Copyright (C) 1996-1999 Martin Blom <martin@blom.org>
     
     This program is free software; you can redistribute it and/or
     modify it under the terms of the GNU General Public License
     as published by the Free Software Foundation; either version 2
     of the License, or (at your option) any later version.
     
     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
     
     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/* $Id$
 * $Log$
 * Revision 1.1  2000/04/01 00:23:41  bernie
 * Oops, forgot all these...
 *
 * Revision 4.11  1999/08/29 23:43:48  lcs
 * Added support for ahigp_AntiClickTime.
 *
 * Revision 4.10  1999/04/22 19:41:08  lcs
 * Removed SAS/C smakefile.
 * I had the copyright date screwed up: Changed to 1996-1999 (which is only
 * partly correct, but still better than 1997-1999....)
 *
 * Revision 4.9  1999/03/28 22:30:38  lcs
 * AHI is now GPL/LGPL software.
 * Make target bindist work correctly when using a separate build directory.
 * Small first steps towards a WarpOS PPC version.
 *
 * Revision 4.8  1999/01/09 23:14:06  lcs
 * Switched from SAS/C to gcc
 *
 * Revision 4.7  1997/06/24 21:49:49  lcs
 * Increased version number to match the catalogs (4.5).
 *
 * Revision 4.6  1997/05/09 14:02:17  lcs
 * Program version 4.4
 *
 * Revision 4.5  1997/05/04 22:13:29  lcs
 * Version label now public
 *
 * Revision 4.4  1997/04/27 16:16:06  lcs
 * Added "Mastervolume with(out) clipping".
 *
 * Revision 4.3  1997/04/09 03:00:06  lcs
 * Fixed globaloptions and "Restore"
 *
 * Revision 4.2  1997/04/07 01:36:51  lcs
 * Localized it, bug fixes
 *
 */

#include <config.h>
#include <CompilerSpecific.h>

#include <devices/ahi.h>
#include <workbench/startup.h>
#include <proto/ahi.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "ahi.h"
#include "ahiprefs_Cat.h"
#include "support.h"
#include "gui.h"
#include "version.h"

#define DBSTEP      0.5

const char *Version = "$VER: AHI preferences " VERS "\r\n";

#define TEMPLATE "FROM,EDIT/S,USE/S,SAVE/S,PUBSCREEN/K"

struct List      *UnitList   = NULL;
struct List      *ModeList   = NULL;
char            **Units      = NULL;
char            **Modes      = NULL;
char            **Inputs     = NULL;
char            **Outputs    = NULL;

struct state state = { 0 };
struct args  args =  {NULL, FALSE, FALSE, FALSE, NULL };

BOOL SaveIcons;

static char freqBuffer[16];
static char chanBuffer[16];
static char outvolBuffer[16];
static char monvolBuffer[16];
static char gainBuffer[16];

static char authorBuffer[32];
static char copyrightBuffer[32];
static char driverBuffer[32];
static char versionBuffer[32];


/******************************************************************************
**** main() function **********************************************************
******************************************************************************/

int main(int argc, char **argv) {
  struct RDArgs *rdargs = NULL;
  int i;
  char pubscreen[32];

  if(argc) {
    rdargs=ReadArgs( TEMPLATE , (LONG *) &args, NULL);
    SaveIcons  = FALSE;
  }
  else {
    struct WBStartup *WBenchMsg = (struct WBStartup *)argv;
    struct WBArg *wbarg;
    LONG olddir;
    struct DiskObject *dobj;
    char **toolarray;
    char *s;

    SaveIcons  = TRUE;

    for(i=0, wbarg=WBenchMsg->sm_ArgList;
      i < WBenchMsg->sm_NumArgs;
      i++, wbarg++) {

      olddir = -1;
      if((wbarg->wa_Lock)&&(*wbarg->wa_Name))
          olddir = CurrentDir(wbarg->wa_Lock);


      if((*wbarg->wa_Name) && (dobj=GetDiskObject(wbarg->wa_Name))) {
        toolarray = (char **)dobj->do_ToolTypes;

        s = (char *) FindToolType(toolarray,"CREATEICONS");

        if( s != NULL ) {
          if( MatchToolValue(s,"NO") ||
              MatchToolValue(s,"FALSE")) {
            SaveIcons = FALSE;
          }
        }

        s = (char *) FindToolType(toolarray,"PUBSCREEN");

        if( s != NULL ) {
          strncpy(pubscreen, s, sizeof pubscreen);
          args.pubscreen = pubscreen;
        }

        s = (char *) FindToolType(toolarray,"ACTION");

        if( s != NULL ) {
          if(MatchToolValue(s,"EDIT")) {
            args.edit = TRUE;
          }
          else if(MatchToolValue(s,"USE")) {
            args.use = TRUE;
          }
          else if(MatchToolValue(s,"SAVE")) {
            args.save = TRUE;
          }
        }

        FreeDiskObject(dobj);
      }

      if((i>0)&&(*wbarg->wa_Name)) {
        args.from = wbarg->wa_Name;
      }

      if(olddir != -1) {
        CurrentDir(olddir); /* CD back where we were */
      }
    }
  
  }

  if(args.from == NULL) {
    args.from = ENVFILE;
  }

  if(args.edit) {
    args.use  = FALSE;
    args.save = FALSE;
  }

  if((SetSignal(0L,SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C) == 0) {
    if(Initialize()) {

      NewSettings(args.from);

      if((!args.use && !args.save) || args.edit) {
        if(BuildGUI(args.pubscreen)) {
          EventLoop();
        }
        CloseGUI();
      }

      if(args.use || args.save) {
        SaveSettings(ENVFILE, UnitList);
      }

      if(args.save) {
        SaveSettings(ENVARCFILE, UnitList);
      }
    }
  }

  if(rdargs) {
    FreeArgs(rdargs);
  }

  FreeVec(Units);
  FreeVec(Modes);
  FreeVec(Outputs);
  FreeVec(Inputs);
  FreeList(ModeList);
  FreeList(UnitList);

  CleanUp();

  return 0;
}

/******************************************************************************
**** Given a file name, load it and fill all structures ***********************
******************************************************************************/

void NewSettings(char *name) {
  FreeVec(Units);
  FreeList(UnitList);

  globalprefs.ahigp_DebugLevel       = AHI_DEBUG_NONE;
  globalprefs.ahigp_DisableSurround  = FALSE;
  globalprefs.ahigp_DisableEcho      = FALSE;
  globalprefs.ahigp_FastEcho         = FALSE;
  globalprefs.ahigp_MaxCPU           = (90 << 16) / 100;
  globalprefs.ahigp_ClipMasterVolume = FALSE;
  globalprefs.ahigp_AntiClickTime    = 0;

  UnitList = GetUnits(name);
  Units = List2Array((struct List *) UnitList);

  NewUnit(0);
}

/******************************************************************************
**** Given a unit, fill the state structure ***********************************
******************************************************************************/

void NewUnit(int selectedunit) {
  struct UnitNode *unit;
  ULONG id, modeselected;
  struct ModeNode *mode;

  unit = (struct UnitNode *) GetNode(selectedunit, UnitList);

  state.UnitSelected     = selectedunit;

  id   = unit->prefs.ahiup_AudioMode;

  FreeVec(Modes);
  FreeList(ModeList);
  ModeList = GetModes( &unit->prefs);
  Modes = List2Array((struct List *) ModeList);

  modeselected = 0;
  mode = (struct ModeNode *) ModeList->lh_Head;
  while(mode->node.ln_Succ) {
    if(id == mode->ID)
      break;
    modeselected++;
    mode = (struct ModeNode *) mode->node.ln_Succ;
  }

  if(mode->node.ln_Succ == NULL) {
    modeselected = 0;
  }
  
  NewMode(modeselected);
}


/******************************************************************************
**** Given a mode, fill all that depends on it in the state structure *********
******************************************************************************/

void NewMode(int selectedmode) {
  struct UnitNode *unit;
  ULONG id;
  Fixed MinOutVol = 0, MaxOutVol = 0, MinMonVol = 0, MaxMonVol = 0;
  Fixed MinGain = 0, MaxGain = 0;
  double Min, Max, Current;
  int offset;

  state.ModeSelected = selectedmode;

  unit = (struct UnitNode *) GetNode(state.UnitSelected, UnitList);

  id = ((struct ModeNode *) GetNode(selectedmode, ModeList))->ID;

  AHI_GetAudioAttrs(id, NULL,
      AHIDB_IndexArg,         unit->prefs.ahiup_Frequency,
      AHIDB_Index,            (ULONG) &state.FreqSelected,
      AHIDB_Frequencies,      (ULONG) &state.Frequencies,
      AHIDB_MaxChannels,      (ULONG) &state.Channels,
      AHIDB_Inputs,           (ULONG) &state.Inputs,
      AHIDB_Outputs,          (ULONG) &state.Outputs,
      AHIDB_MinOutputVolume,  (ULONG) &MinOutVol,
      AHIDB_MaxOutputVolume,  (ULONG) &MaxOutVol,
      AHIDB_MinMonitorVolume, (ULONG) &MinMonVol,
      AHIDB_MaxMonitorVolume, (ULONG) &MaxMonVol,
      AHIDB_MinInputGain,     (ULONG) &MinGain,
      AHIDB_MaxInputGain,     (ULONG) &MaxGain,

      AHIDB_BufferLen,        32,
      AHIDB_Author,           (ULONG) authorBuffer,
      AHIDB_Copyright,        (ULONG) copyrightBuffer,
      AHIDB_Driver,           (ULONG) driverBuffer,
      AHIDB_Version,          (ULONG) versionBuffer,
      TAG_DONE);

  state.ChannelsSelected = unit->prefs.ahiup_Channels;
  state.InputSelected    = unit->prefs.ahiup_Input;
  state.OutputSelected   = unit->prefs.ahiup_Output;

  // Limit channels
  state.Channels = min(state.Channels, 32);

  if(unit->prefs.ahiup_Unit == AHI_NO_UNIT) {
    state.ChannelsDisabled = TRUE;
  }
  else {
    state.ChannelsDisabled = FALSE;
  }

  if(MinOutVol == 0) {
    MinOutVol = 1;
    state.OutVolMute = TRUE;
    state.OutVols    = 1;
  }
  else {
    state.OutVolMute = FALSE;
    state.OutVols    = 0;
  }

  if(MinMonVol == 0) {
    MinMonVol = 1;
    state.MonVolMute = TRUE;
    state.MonVols    = 1;
  }
  else {
    state.MonVolMute = FALSE;
    state.MonVols    = 0;
  }


  if(MinGain == 0) {
    MinGain = 1;
    state.GainMute = TRUE;
    state.Gains    = 1;
  }
  else {
    state.GainMute = FALSE;
    state.Gains    = 0;
  }

  if(MaxOutVol == 0) {
    state.OutVolSelected = 0;
    state.OutVolOffset   = 0;
  }
  else {
    Current = 20 * log10( unit->prefs.ahiup_OutputVolume / 65536.0 );
    Min = floor(20 * log10( MinOutVol / 65536.0 ) / DBSTEP + 0.5) * DBSTEP;
    Max = floor(20 * log10( MaxOutVol / 65536.0 ) / DBSTEP + 0.5) * DBSTEP;
    state.OutVolSelected = (Current - Min) / DBSTEP + 0.5 + state.OutVols;
    state.OutVols += ((Max - Min) / DBSTEP) + 1;
    state.OutVolOffset = Min;
  }

  if(MaxMonVol == 0) {
    state.MonVolSelected = 0;
    state.MonVolOffset   = 0;
  }
  else {
    Current = 20 * log10( unit->prefs.ahiup_MonitorVolume / 65536.0 );
    Min = floor(20 * log10( MinMonVol / 65536.0 ) / DBSTEP + 0.5) * DBSTEP;
    Max = floor(20 * log10( MaxMonVol / 65536.0 ) / DBSTEP + 0.5) * DBSTEP;
    state.MonVolSelected = (Current - Min) / DBSTEP + 0.5 + state.MonVols;
    state.MonVols += ((Max - Min) / DBSTEP) + 1;
    state.MonVolOffset = Min;
  }

  if(MaxGain == 0) {
    state.GainSelected = 0;
    state.GainOffset   = 0;
  }
  else {
    Current = 20 * log10( unit->prefs.ahiup_InputGain / 65536.0 );
    Min = floor(20 * log10( MinGain / 65536.0 ) / DBSTEP + 0.5) * DBSTEP;
    Max = floor(20 * log10( MaxGain / 65536.0 ) / DBSTEP + 0.5) * DBSTEP;
    state.GainSelected = (Current - Min) / DBSTEP + 0.5 + state.Gains;
    state.Gains += ((Max - Min) / DBSTEP) + 1;
    state.GainOffset = Min;
  }

  // Make sure everything is within bounds!

  state.FreqSelected = max(state.FreqSelected, 0);  
  state.FreqSelected = min(state.FreqSelected, state.Frequencies);

  state.ChannelsSelected = max(state.ChannelsSelected, 1);
  state.ChannelsSelected = min(state.ChannelsSelected, state.Channels);
  
  state.OutVolSelected = max(state.OutVolSelected, 0);
  state.OutVolSelected = min(state.OutVolSelected, state.OutVols);
  
  state.MonVolSelected = max(state.MonVolSelected, 0);
  state.MonVolSelected = min(state.MonVolSelected, state.MonVols);
  
  state.GainSelected = max(state.GainSelected, 0);
  state.GainSelected = min(state.GainSelected, state.Gains);

  state.InputSelected = max(state.InputSelected, 0);
  state.InputSelected = min(state.InputSelected, state.Inputs);

  state.OutputSelected = max(state.OutputSelected, 0);
  state.OutputSelected = min(state.OutputSelected, state.Outputs);

  // Remove any \r's or \n's from version string

  offset = strlen(versionBuffer);
  while((offset > 0) &&
        ((versionBuffer[offset-1] == '\r') ||
         (versionBuffer[offset-1] == '\n'))) {
    versionBuffer[offset-1] = '\0';
    offset--;
  }

  FreeVec(Inputs);
  FreeVec(Outputs);
  Inputs   = GetInputs(id);
  Outputs  = GetOutputs(id);
}


/******************************************************************************
**** Fill the AHIUnitPrefs structure from state *******************************
******************************************************************************/

void FillUnit() {
  struct UnitNode *unit;
  ULONG id;
  double db;

  unit = (struct UnitNode *) GetNode(state.UnitSelected, UnitList);

  if(unit->prefs.ahiup_Unit != AHI_NO_UNIT) {
    unit->prefs.ahiup_Channels    = state.ChannelsSelected;
  }
  else {
    unit->prefs.ahiup_Channels    = 0;
  }

  id = ((struct ModeNode *) GetNode(state.ModeSelected, ModeList))->ID;

  unit->prefs.ahiup_AudioMode     = id;

  AHI_GetAudioAttrs(id, NULL,
      AHIDB_FrequencyArg, state.FreqSelected,
      AHIDB_Frequency,    (ULONG) &unit->prefs.ahiup_Frequency,
      TAG_DONE);

  db = state.OutVolOffset + DBSTEP * 
      (state.OutVolSelected - (state.OutVolMute ? 1 : 0) );
  unit->prefs.ahiup_OutputVolume = pow(10.0, db/20) * 65536 + 0.5;
  if(state.OutVolMute && (state.OutVolSelected == 0))
    unit->prefs.ahiup_OutputVolume = 0;

  db = state.MonVolOffset + DBSTEP * 
      (state.MonVolSelected - (state.MonVolMute ? 1 : 0) );
  unit->prefs.ahiup_MonitorVolume = pow(10.0, db/20) * 65536 + 0.5;
  if(state.MonVolMute && (state.MonVolSelected == 0))
    unit->prefs.ahiup_MonitorVolume = 0;

  db = state.GainOffset + DBSTEP * 
      (state.GainSelected - (state.GainMute ? 1 : 0) );
  unit->prefs.ahiup_InputGain = pow(10.0, db/20) * 65536 + 0.5;
  if(state.GainMute && (state.GainSelected == 0))
    unit->prefs.ahiup_InputGain = 0;

  unit->prefs.ahiup_Input         = state.InputSelected;
  unit->prefs.ahiup_Output        = state.OutputSelected;
}


/******************************************************************************
**** Routines to show state in human readable from ****************************
******************************************************************************/

char *getFreq(void) {
  LONG freq = 0;

  AHI_GetAudioAttrs(
      ((struct ModeNode *) GetNode(state.ModeSelected, ModeList))->ID, NULL,
      AHIDB_FrequencyArg, state.FreqSelected,
      AHIDB_Frequency,    (ULONG) &freq,
      TAG_DONE);

  sprintf(freqBuffer, "%ld Hz", freq);
  return freqBuffer;
}


char *getChannels(void) {
  if(state.ChannelsDisabled) {
    sprintf(chanBuffer, (char *) msgOptNoChannels);
  }
  else {
    sprintf(chanBuffer, "%ld", state.ChannelsSelected);
  }
  return chanBuffer;
}

char *getOutVol(void) {
  int selected = state.OutVolSelected;

  if(state.OutVolMute) {
    if(selected == 0) {
      sprintf(outvolBuffer, (char *) msgOptMuted);
      return outvolBuffer;
    }
    else {
      selected--;
    }
  }

  sprintf(outvolBuffer, "%+4.1f dB", state.OutVolOffset + (selected * DBSTEP));
  return outvolBuffer;
}

char *getMonVol(void) {
  int selected = state.MonVolSelected;

  if(state.MonVolMute) {
    if(selected == 0) {
      sprintf(monvolBuffer, (char *) msgOptMuted);
      return monvolBuffer;
    }
    else {
      selected--;
    }
  }

  sprintf(monvolBuffer, "%+4.1f dB", state.MonVolOffset + (selected * DBSTEP));
  return monvolBuffer;
}

char *getGain(void) {
  int selected = state.GainSelected;

  sprintf(gainBuffer, "%+4.1f dB", state.GainOffset + (selected * DBSTEP));
  return gainBuffer;
}

char *getInput(void) {

  if(Inputs[0]) {
    return Inputs[state.InputSelected];
  }
  return (char *) msgOptNoInputs;
}

char *getOutput(void) {

  if(Outputs[0]) {
    return Outputs[state.OutputSelected];
  }
  return (char *) msgOptNoOutputs;
}

ULONG getAudioMode(void) {

  return ((struct ModeNode *) GetNode(state.ModeSelected, ModeList))->ID;
}

char *getRecord(void) {
  ULONG record = FALSE, fullduplex = FALSE;

  AHI_GetAudioAttrs(
      ((struct ModeNode *) GetNode(state.ModeSelected, ModeList))->ID, NULL,
      AHIDB_Record,     (ULONG) &record,
      AHIDB_FullDuplex, (ULONG) &fullduplex,
      TAG_DONE);
  
  return (char *) (record ? (fullduplex ? msgPropRecordFull : msgPropRecordHalf )
                          : msgPropRecordNone);
}

char *getAuthor(void) {
  return authorBuffer;
}

char *getCopyright(void) {
  return copyrightBuffer;
}

char *getDriver(void) {
  return driverBuffer;
}

char *getVersion(void) {
  return versionBuffer;
}
