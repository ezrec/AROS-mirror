#ifndef LINEAR_PROTOS_H
#define LINEAR_PROTOS_H

APTR Lin_InitFireworks(struct Globals *glob, struct Prefs *pref);
void Lin_RethinkWindow(APTR data);
void Lin_TimePassed(APTR data);
BOOL Lin_IsIdle(APTR data);
void Lin_DrawFireworks(APTR data, UWORD Mask);
void Lin_NoteOn(APTR data, UBYTE chn, UBYTE note, UBYTE vel, LONG reltime);
void Lin_NoteOff(APTR data, UBYTE chn, UBYTE note, LONG reltime);
void Lin_ReleaseNotes(APTR data, LONG reltime);
void Lin_FreeNoteData(APTR data);
void Lin_ExitFireworks(APTR data);

#endif /* LINEAR_PROTOS_H */
