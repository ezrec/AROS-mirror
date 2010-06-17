#ifndef PROTO_SOUND_H
#define PROTO_SOUND_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/classes/datatypes/sound/sound.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/sound_protos.h>

#if !defined(SoundBase) && !defined(__NOLIBBASE__) && !defined(__SOUND_NOLIBBASE__)
 #ifdef __SOUND_STDLIBBASE__
  extern struct Library *SoundBase;
 #else
  extern struct Library *SoundBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(SOUND_NOLIBDEFINES)
#   include <defines/sound.h>
#endif

#endif /* PROTO_SOUND_H */
