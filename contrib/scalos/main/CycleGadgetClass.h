// CycleGadgetClass.h
// $Date$
// $Revision$

#ifndef CYCLEGADGETCLASS_H
#define CYCLEGADGETCLASS_H


/* Tags for Cycle Gadget class */

#define	SCACYCLE_TagBase	(TAG_USER + 0x91b00)

#define	SCACYCLE_Active		(SCACYCLE_TagBase + 1)	// [ISG] (ULONG) index of active label
#define	SCACYCLE_Labels		(SCACYCLE_TagBase + 2)	// [I..] (STRPTR []) array of label strings, end with NULL
#define SCACYCLE_FrameImage	(SCACYCLE_TagBase + 3)	// [I..] (Object *) Image to use as frame
#define SCACYCLE_TextFont	(SCACYCLE_TagBase + 4)	// [ISG] (struct TextFont *) font to use for gadget label strings
#define SCACYCLE_PopupTextFont	(SCACYCLE_TagBase + 5)	// [ISG] (struct TextFont *) font to use for popup strings
#define SCACYCLE_PopupTTFont	(SCACYCLE_TagBase + 6)	// [ISG] (struct TTFontFamily *) TT font to use for popup strings

#endif /* CYCLEGADGETCLASS_H */

