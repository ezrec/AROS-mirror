#ifndef FWMODES_H
#define FWMODES_H

#include "fireworks.h"

enum
{
  LinearMode,
  ParabolicMode,
  ParabolicMode2,
  NUM_FWMODES
};

struct FWDefinition{
  APTR (*InitFireworks)(struct Globals *glob, struct Prefs *pref);
  void (*RethinkWindow)(APTR data);
  void (*TimePassed)(APTR data);
  BOOL (*IsIdle)(APTR data);
  void (*DrawFireworks)(APTR data, UWORD Mask);
  void (*NoteOn)(APTR data, UBYTE chn, UBYTE note, UBYTE vel, LONG reltime);
  void (*NoteOff)(APTR data, UBYTE chn, UBYTE note, LONG reltime);
  void (*ReleaseNotes)(APTR data, LONG reltime);
  void (*FreeNoteData)(APTR data);
  void (*ExitFireworks)(APTR data);
};

extern struct FWDefinition FWDefinitions[NUM_FWMODES];

#endif /* FWMODES_H */
