#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <exec/exec.h>
#include <dos/dos.h>
#include <graphics/gfxbase.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>

/* #include <math.h> */

#include "z_zone.h"

#include "i_system.h"
#include "i_sound.h"
#include "m_argv.h"
#include "m_misc.h"
#include "w_wad.h"

#include "doomdef.h"


// Any value of numChannels set
// by the defaults code in M_misc is now clobbered by I_InitSound().
// number of channels available for sound effects

extern int numChannels;


/**********************************************************************/
//
// This function loads the sound data for sfxname from the WAD lump,
// and returns a ptr to the data in fastmem and its length in len.
//
static void *getsfx (char *sfxname, int *len)
{
    return NULL;
}

/**********************************************************************/
// Init at program start...
void I_InitSound (void)
{
    return;
}

/**********************************************************************/
// ... update sound buffer and audio device at runtime...
void I_UpdateSound (void)
{
}

/**********************************************************************/
// ... update sound buffer and audio device at runtime...
void I_SubmitSound (void)
{
}

/**********************************************************************/
// ... shut down and relase at program termination.
void I_ShutdownSound (void)
{
}

/**********************************************************************/
/**********************************************************************/
//
//  SFX I/O
//

/**********************************************************************/
// Initialize number of channels
void I_SetChannels (void)
{
}

/**********************************************************************/
// Get raw data lump index for sound descriptor.
int I_GetSfxLumpNum (sfxinfo_t *sfx)
{
  char namebuf[9];

  /* fprintf (stderr, "I_GetSfxLumpNum()\n"); */
  sprintf(namebuf, "ds%s", sfx->name);
  return W_GetNumForName(namebuf);
}

/**********************************************************************/
// Find chip cache entry used longest ago and re-use it.
static int cache_chip_data (int id)
{
  return 0;
}

/**********************************************************************/
// Starts a sound in a particular sound channel.
int I_StartSound (
  int id,
  int cnum,
  int vol,
  int sep,
  int pitch,
  int priority )
{
  return 1;
}

/**********************************************************************/
// Stops a sound channel.
void I_StopSound(int handle)
{
}

/**********************************************************************/
// Called by S_*() functions
//  to see if a channel is still playing.
// Returns 0 if no longer playing, 1 if playing.
int I_SoundIsPlaying(int handle)
{
  return 0;
}

/**********************************************************************/
// Updates the volume, separation,
//  and pitch of a sound channel.
void
I_UpdateSoundParams
( int		handle,
  int		vol,
  int		sep,
  int		pitch )
{
}


/**********************************************************************/
/**********************************************************************/
//
// MUSIC API.
//

static int looping=0;
static int musicdies=-1;

static int music_okay = 0;

/**********************************************************************/
//
//  MUSIC I/O
//
void I_InitMusic(void)
{
}

/**********************************************************************/
void I_ShutdownMusic(void)
{
}

/**********************************************************************/
// Volume.
void I_SetMusicVolume(int volume)
{
}

/**********************************************************************/
// PAUSE game handling.
void I_PauseSong(int handle)
{
}

/**********************************************************************/
void I_ResumeSong(int handle)
{
}

/**********************************************************************/
// Registers a song handle to song data.
int I_RegisterSong(void *data)
{
    return 2;
}

/**********************************************************************/
// Called by anything that wishes to start music.
//  plays a song, and when the song is done,
//  starts playing it again in an endless loop.
// Horrible thing to do, considering.
void
I_PlaySong
( int		handle,
  int		looping )
{
}

/**********************************************************************/
// Stops a song over 3 seconds.
void I_StopSong(int handle)
{
}

/**********************************************************************/
// See above (register), then think backwards
void I_UnRegisterSong(int handle)
{
}

/**********************************************************************/


/**********************************************************************/
void _STDaudio_cleanup (void)
{
}

/**********************************************************************/
