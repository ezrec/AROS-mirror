
/* The fireworks modes */

#include "fwmodes.h"

#include "linear_protos.h"
#include "parabolic_protos.h"

struct FWDefinition FWDefinitions[NUM_FWMODES] =
{
  {
    &Lin_InitFireworks,			/* The Laser Mode */
    &Lin_RethinkWindow,
    &Lin_TimePassed,
    &Lin_IsIdle,
    &Lin_DrawFireworks,
    &Lin_NoteOn,
    &Lin_NoteOff,
    &Lin_ReleaseNotes,
    &Lin_FreeNoteData,
    &Lin_ExitFireworks
  },{
    &Par_InitFireworks,			/* The Pixel Mode */
    &Par_RethinkWindow,
    &Par_TimePassed,
    &Par_IsIdle,
    &Par_DrawFireworks,
    &Par_NoteOn,
    &Par_NoteOff,
    &Par_ReleaseNotes,
    &Par_FreeNoteData,
    &Par_ExitFireworks,
  },{	
    &Par_InitFireworks,			/* The Fountain Mode */
    &Par_RethinkWindow,
    &Par_TimePassed,
    &Par_IsIdle,
    &Par_DrawFireworks2,
    &Par_NoteOn,
    &Par_NoteOff,
    &Par_ReleaseNotes,
    &Par_FreeNoteData,
    &Par_ExitFireworks,
  }
};
