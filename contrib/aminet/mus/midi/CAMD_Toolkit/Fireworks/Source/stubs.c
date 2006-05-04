
#include "fireworks.h"
#include "fireworks_protos.h"


/*-----------------------*/
/* Standard Exit routine */
/*-----------------------*/

void __stdargs _XCEXIT(LONG lcode)
{
	Message("Task wants to exit, return code %ld\nHolding task!", NULL, lcode);
	Wait(0);
}


/*-------------------*/
/* CAM Library stubs */
/*-------------------*/
#if defined(NO_INLINE_STDARG)
struct MidiNode *CreateMidi(Tag tag, ...)
{
	return CreateMidiA((struct TagItem *)&tag );
}

BOOL SetMidiAttrs(struct MidiNode *mi, Tag tag, ...)
{
	return SetMidiAttrsA(mi, (struct TagItem *)&tag );
}

struct MidiLink *AddMidiLink(struct MidiNode *mi, LONG type, Tag tag, ...)
{
	return AddMidiLinkA(mi, type, (struct TagItem *)&tag );
}

BOOL SetMidiLinkAttrs(struct MidiLink *mi, Tag tag, ...)
{
	return SetMidiLinkAttrsA(mi, (struct TagItem *)&tag );
}

APTR GetVisualInfo(struct Screen *screen, Tag tag, ...){
  return GetVisualInfoA(screen,(struct TagItem *)&tag);
}

struct Menu *CreateMenus(struct NewMenu *newmenu,Tag tag,...){
  return CreateMenusA(newmenu,(struct TagItem *)&tag);
}

BOOL LayoutMenus(struct Menu *menu,APTR vi,Tag tag,...){
  return LayoutMenusA(menu,vi,(struct TagItem *)&tag);
}

ULONG BestModeID(Tag tag,...){
  return BestModeIDA((struct TagItem *)&tag);
}
#endif

