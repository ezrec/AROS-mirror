#ifndef PARABOLIC_PROTOS_H
#define PARABOLIC_PROTOS_H

APTR Par_InitFireworks(struct Globals *glob, struct Prefs *pref);
void Par_RethinkWindow(APTR data);
void Par_TimePassed(APTR data);
BOOL Par_IsIdle(APTR data);
void Par_DrawFireworks(APTR data, UWORD Mask);
void Par_NoteOn(APTR data, UBYTE chn, UBYTE note, UBYTE vel, LONG reltime);
void Par_NoteOff(APTR data, UBYTE chn, UBYTE note, LONG reltime);
void Par_ReleaseNotes(APTR data, LONG reltime);
void Par_FreeNoteData(APTR data);
void Par_ExitFireworks(APTR data);

void Par_DrawFireworks2(APTR data, UWORD Mask);

#endif /* PARABOLIC_PROTOS_H */
