#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <exec/exec.h>
#include <dos/dos.h>
#include <graphics/gfxbase.h>
#include <devices/audio.h>

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

extern struct ExecBase *SysBase;

#define	BeginIO(ioRequest)	_BeginIO(ioRequest)

__inline void
_BeginIO(struct IORequest *ioRequest)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode	=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart	=	NULL;
//	MyCaos.M68kSize		=	0;
	MyCaos.PPCCacheMode	=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart		=	NULL;
//	MyCaos.PPCSize		=	0;
	MyCaos.a1		=(ULONG) ioRequest;
	MyCaos.caos_Un.Offset	=	(-30);
	MyCaos.a6		=(ULONG) ioRequest->io_Device;
	PPCCallOS(&MyCaos);
}

/**********************************************************************/
#define MAXSFXVOICES    16   /* max number of Sound Effects with server */
#define MAXNUMCHANNELS   4   /* max number of Amiga sound channels */
#define CHIP_CACHE_SIZE 12   /* number of waveforms allowed in chipmem */

struct chip_cache_info {
  int id;
  ULONG age;
  char *chip_data;
  int len;
};

struct channel_info {
  struct MsgPort *audio_mp;
  struct IOAudio *audio_io;
  BOOL sound_in_progress;
};

/**********************************************************************/
// The actual lengths of all sound effects.
static int lengths[NUMSFX];

static struct MsgPort *audio_mp = NULL;
static struct IOAudio *audio_io = NULL;
static BOOL audio_is_open = FALSE;

static struct channel_info channel_info[MAXNUMCHANNELS] = {
  {NULL, NULL, FALSE},
  {NULL, NULL, FALSE},
  {NULL, NULL, FALSE},
  {NULL, NULL, FALSE},
};

/* cache up to CHIP_CACHE_SIZE sound effects in chipmem, rest in fastmem */
static struct chip_cache_info chip_cache_info[CHIP_CACHE_SIZE] = {
  {-1, 0, NULL, -1},
  {-1, 0, NULL, -1},
  {-1, 0, NULL, -1},
  {-1, 0, NULL, -1},
  {-1, 0, NULL, -1},
  {-1, 0, NULL, -1},
  {-1, 0, NULL, -1},
  {-1, 0, NULL, -1},
  {-1, 0, NULL, -1},
  {-1, 0, NULL, -1},
  {-1, 0, NULL, -1},
  {-1, 0, NULL, -1}
};

static ULONG age = 1;
static UWORD period_table[256];

static int changepitch;
/* sampling freq (Hz) for each pitch step when changepitch is TRUE
   calculated from (2^((i-128)/64))*11025
   I'm not sure if this is the right formula */
static UWORD freqs[256] = {
  2756, 2786, 2817, 2847, 2878, 2910, 2941, 2973,
  3006, 3038, 3072, 3105, 3139, 3173, 3208, 3242,
  3278, 3313, 3350, 3386, 3423, 3460, 3498, 3536,
  3574, 3613, 3653, 3692, 3733, 3773, 3814, 3856,
  3898, 3940, 3983, 4027, 4071, 4115, 4160, 4205,
  4251, 4297, 4344, 4391, 4439, 4487, 4536, 4586,
  4635, 4686, 4737, 4789, 4841, 4893, 4947, 5001,
  5055, 5110, 5166, 5222, 5279, 5336, 5394, 5453,
  5513, 5573, 5633, 5695, 5757, 5819, 5883, 5947,
  6011, 6077, 6143, 6210, 6278, 6346, 6415, 6485,
  6556, 6627, 6699, 6772, 6846, 6920, 6996, 7072,
  7149, 7227, 7305, 7385, 7465, 7547, 7629, 7712,
  7796, 7881, 7967, 8053, 8141, 8230, 8319, 8410,
  8501, 8594, 8688, 8782, 8878, 8975, 9072, 9171,
  9271, 9372, 9474, 9577, 9681, 9787, 9893, 10001,
  10110, 10220, 10331, 10444, 10558, 10673, 10789, 10906,
  11025, 11145, 11266, 11389, 11513, 11638, 11765, 11893,
  12023, 12154, 12286, 12420, 12555, 12692, 12830, 12970,
  13111, 13254, 13398, 13544, 13691, 13841, 13991, 14144,
  14298, 14453, 14611, 14770, 14931, 15093, 15258, 15424,
  15592, 15761, 15933, 16107, 16282, 16459, 16639, 16820,
  17003, 17188, 17375, 17564, 17756, 17949, 18144, 18342,
  18542, 18744, 18948, 19154, 19363, 19574, 19787, 20002,
  20220, 20440, 20663, 20888, 21115, 21345, 21578, 21812,
  22050, 22290, 22533, 22778, 23026, 23277, 23530, 23787,
  24046, 24308, 24572, 24840, 25110, 25384, 25660, 25940,
  26222, 26508, 26796, 27088, 27383, 27681, 27983, 28287,
  28595, 28907, 29221, 29540, 29861, 30187, 30515, 30848,
  31183, 31523, 31866, 32213, 32564, 32919, 33277, 33639,
  34006, 34376, 34750, 35129, 35511, 35898, 36289, 36684,
  37084, 37487, 37896, 38308, 38725, 39147, 39573, 40004,
  40440, 40880, 41325, 41775, 42230, 42690, 43155, 43625
};

/**********************************************************************/

static struct Library *DoomSndBase = NULL;

static __inline void Sfx_SetVol(int vol)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHNO;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHNO;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.d0			=	vol;
	MyCaos.caos_Un.Offset		=       (((int)DoomSndBase)-30);
	PPCCallM68k(&MyCaos);
}

static __inline void Sfx_Start(char *wave, int cnum, int step, int vol, int sep, int length)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.a0			= (ULONG) wave;
	MyCaos.d0			=	cnum;
	MyCaos.d1			=	step;
	MyCaos.d2			=	vol;
	MyCaos.d3			=	sep;
	MyCaos.d4			=	length;
	MyCaos.caos_Un.Offset		=       (((int)DoomSndBase)-36);
	PPCCallM68k(&MyCaos);
}

static __inline void Sfx_Update(int cnum, int step, int vol, int sep)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHNO;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHNO;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.d0			=	cnum;
	MyCaos.d1			=	step;
	MyCaos.d2			=	vol;
	MyCaos.d3			=	sep;
	MyCaos.caos_Un.Offset		=       (((int)DoomSndBase)-42);
	PPCCallM68k(&MyCaos);
}

static __inline void Sfx_Stop(int cnum)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHNO;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHNO;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.d0			=	cnum;
	MyCaos.caos_Un.Offset		=       (((int)DoomSndBase)-48);
	PPCCallM68k(&MyCaos);
}

static __inline int Sfx_Done(int cnum)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHNO;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHNO;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.d0			=	cnum;
	MyCaos.caos_Un.Offset		=       (((int)DoomSndBase)-54);
	return((int)PPCCallM68k(&MyCaos));
}

static __inline void Mus_SetVol(int vol)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHNO;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHNO;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.d0			=	vol;
	MyCaos.caos_Un.Offset		=       (((int)DoomSndBase)-60);
	PPCCallM68k(&MyCaos);
}

static __inline int Mus_Register(void *musdata)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.a0			= (ULONG) musdata;
	MyCaos.caos_Un.Offset		=       (((int)DoomSndBase)-66);
	return((int)PPCCallM68k(&MyCaos));
}

static __inline void Mus_Unregister(int handle)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHNO;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHNO;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.d0			=	handle;
	MyCaos.caos_Un.Offset		=       (((int)DoomSndBase)-72);
	PPCCallM68k(&MyCaos);
}

static __inline void Mus_Play(int handle, int looping)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHNO;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHNO;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.d0			=	handle;
	MyCaos.d1			=	looping;
	MyCaos.caos_Un.Offset		=       (((int)DoomSndBase)-78);
	PPCCallM68k(&MyCaos);
}

static __inline void Mus_Stop(int handle)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHNO;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHNO;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.d0			=	handle;
	MyCaos.caos_Un.Offset		=       (((int)DoomSndBase)-84);
	PPCCallM68k(&MyCaos);
}

static __inline void Mus_Pause(int handle)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHNO;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHNO;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.d0			=	handle;
	MyCaos.caos_Un.Offset		=       (((int)DoomSndBase)-90);
	PPCCallM68k(&MyCaos);
}

static __inline void Mus_Resume(int handle)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHNO;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHNO;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.d0			=	handle;
	MyCaos.caos_Un.Offset		=       (((int)DoomSndBase)-96);
	PPCCallM68k(&MyCaos);
}

static __inline int MUS_Done(int handle)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHNO;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHNO;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.d0			=	handle;
	MyCaos.caos_Un.Offset		=       (((int)DoomSndBase)-102);
	return((int)PPCCallM68k(&MyCaos));
}


/**********************************************************************/
//
// This function loads the sound data for sfxname from the WAD lump,
// and returns a ptr to the data in fastmem and its length in len.
//
static void *getsfx (char *sfxname, int *len)
{
  unsigned char*      sfx;
  unsigned char*      paddedsfx;
  int                 i;
  int                 size;
  int                 paddedsize;
  char                name[20];
  int                 sfxlump;

  // Get the sound data from the WAD, allocate lump
  //  in zone memory.
  sprintf(name, "ds%s", sfxname);

  // Now, there is a severe problem with the
  //  sound handling, in it is not (yet/anymore)
  //  gamemode aware. That means, sounds from
  //  DOOM II will be requested even with DOOM
  //  shareware.
  // The sound list is wired into sounds.c,
  //  which sets the external variable.
  // I do not do runtime patches to that
  //  variable. Instead, we will use a
  //  default sound for replacement.
  if (W_CheckNumForName(name) == -1)
    sfxlump = W_GetNumForName("dspistol");
  else
    sfxlump = W_GetNumForName (name);

  size = W_LumpLength (sfxlump);

  // Debug.
  // fprintf( stderr, "." );
  // fprintf( stderr, " -loading  %s (lump %d, %d bytes)\n",
  //         sfxname, sfxlump, size );
  //fflush( stderr );

  sfx = (unsigned char*)W_CacheLumpNum (sfxlump, PU_STATIC);

  // Allocate from zone memory.
  paddedsfx = (unsigned char*)Z_Malloc (size, PU_STATIC, 0);
  // ddt: (unsigned char *) realloc(sfx, paddedsize+8);
  // This should interfere with zone memory handling,
  //  which does not kick in in the soundserver.

  // Now copy and pad.
  for (i = 0; i < size; i++)
    paddedsfx[i] = sfx[i] ^ 0x80;
  /* memcpy (paddedsfx, sfx, size); */

  // Remove the cached lump.
  Z_Free( sfx );

  // Preserve padded length.
  *len = size;

  // Return allocated padded data.
  return (void *) (paddedsfx /* + 8 */);
}

/**********************************************************************/
// Init at program start...
void I_InitSound (void)
{
  int i;
  struct channel_info *c;
  UBYTE chans[1];
  ULONG clock_constant;   /* see Amiga Hardware Manual page 141 */

  if (M_CheckParm("-nosfx"))
    return;

  // Secure and configure sound device first.
  fprintf( stderr, "I_InitSound: ");

  if ((DoomSndBase = OpenLibrary ("doomsound.library",37)) != NULL) {
    Sfx_SetVol(64);
    Mus_SetVol(64);
    numChannels = 16;
  } else {
    fprintf (stderr, " Cannot open doomsound.library, music not available.\n" );
    fprintf( stderr, "I_InitSound: ");
  }

  if (DoomSndBase == NULL) {
    numChannels = 4;

    if ((audio_mp = CreatePort (NULL, 0)) == NULL ||
        (audio_io = (struct IOAudio *)AllocMem(sizeof(struct IOAudio),
                                             MEMF_PUBLIC | MEMF_CLEAR)) == NULL)
      I_Error ("CreatePort() or AllocMem() failed");

    chans[0] = (1 << numChannels) - 1; /* numchannels Amiga audio channels */
    audio_io->ioa_Request.io_Message.mn_ReplyPort = audio_mp;
    audio_io->ioa_Request.io_Message.mn_Node.ln_Pri = 127;
    audio_io->ioa_AllocKey = 0;
    audio_io->ioa_Data = chans;
    audio_io->ioa_Length = sizeof(chans);

    if (OpenDevice (AUDIONAME, 0, (struct IORequest *)audio_io, 0) != 0)
      I_Error ("OpenDevice(\"audio.device\") failed");
    audio_is_open = TRUE;

    for (i = 0; i < numChannels; i++) {
      c = &channel_info[i];
      if ((c->audio_mp = CreatePort (NULL, 0)) == NULL ||
          (c->audio_io = (struct IOAudio *)AllocMem(sizeof(struct IOAudio),
                                             MEMF_PUBLIC | MEMF_CLEAR)) == NULL)
        I_Error ("CreatePort() or AllocMem() failed");
      *c->audio_io = *audio_io;
      c->audio_io->ioa_Request.io_Message.mn_ReplyPort = c->audio_mp;
      c->audio_io->ioa_Request.io_Unit = (struct Unit *)(1 << i);
    }
  }

  if ((GfxBase->DisplayFlags & REALLY_PAL) == 0)
    clock_constant = 3579545;   /* NTSC */
  else
    clock_constant = 3546895;   /* PAL */
  changepitch = M_CheckParm ("-changepitch");
  for (i = 0; i < 256; i++) {
    if (changepitch)
      period_table[i] = ((clock_constant << 1) + freqs[i]) /
                        (((ULONG)freqs[i]) << 1);
    else
      period_table[i] = ((clock_constant << 1) + 11025) / ((11025) << 1);
  }

  fprintf (stderr, " configured audio device\n" );


  // Initialize external data (all sounds) at start, keep static.
  fprintf (stderr, "I_InitSound: ");

  for (i = 1; i < NUMSFX; i++) {
    // Alias? Example is the chaingun sound linked to pistol.
    if (!S_sfx[i].link) {
      // Load data from WAD file.
      S_sfx[i].data = getsfx (S_sfx[i].name, &lengths[i]);
    } else {
      // Previously loaded already?
      S_sfx[i].data = S_sfx[i].link->data;
      lengths[i] = lengths[(S_sfx[i].link - S_sfx)/sizeof(sfxinfo_t)];
    }
  }

  fprintf (stderr, " pre-cached all sound data\n");

  // Finished initialization.
  fprintf (stderr, "I_InitSound: sound module ready\n");
}

/**********************************************************************/
// ... update sound buffer and audio device at runtime...
void I_UpdateSound (void)
{
  /* fprintf (stderr, "I_UpdateSound()\n"); */
}

/**********************************************************************/
// ... update sound buffer and audio device at runtime...
void I_SubmitSound (void)
{
  /* fprintf (stderr, "I_SubmitSound()\n"); */
  // Write it to DSP device.
  // write(audio_fd, mixbuffer, SAMPLECOUNT*BUFMUL);
}

/**********************************************************************/
// ... shut down and relase at program termination.
void I_ShutdownSound (void)
{
  int i;

  fprintf (stderr, "I_ShutdownSound()\n");

  if (DoomSndBase != NULL) {
    CloseLibrary (DoomSndBase);
    DoomSndBase = NULL;
  }

  if (audio_is_open) {
    for (i = 0; i < numChannels; i++)
      I_StopSound (i);
    audio_io->ioa_Request.io_Unit = (struct Unit *)
                     ((1 << numChannels) - 1);  /* free numChannels channels */
    CloseDevice ((struct IORequest *)audio_io);
    audio_is_open = FALSE;
  }
  for (i = 0; i < CHIP_CACHE_SIZE; i++) {
    if (chip_cache_info[i].chip_data != NULL) {
      FreeMem (chip_cache_info[i].chip_data, chip_cache_info[i].len);
      chip_cache_info[i].chip_data = NULL;
    }
  }
  for (i = 0; i < MAXNUMCHANNELS; i++) {
    if (channel_info[i].audio_io != NULL) {
      FreeMem (channel_info[i].audio_io, sizeof(struct IOAudio));
      channel_info[i].audio_io = NULL;
    }
    if (channel_info[i].audio_mp != NULL) {
      DeletePort (channel_info[i].audio_mp);
      channel_info[i].audio_mp = NULL;
    }
  }
  if (audio_io != NULL) {
    FreeMem (audio_io, sizeof(struct IOAudio));
    audio_io = NULL;
  }
  if (audio_mp != NULL) {
    DeletePort (audio_mp);
    audio_mp = NULL;
  }
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
  int i, mini;
  ULONG minage;
  struct chip_cache_info *c;

  if (age == 0xfffffffe)
    I_Error ("Age overflow in cache_chip_data()");
  minage = 0xffffffff;
  mini = 0;
  for (i = 0; i < CHIP_CACHE_SIZE; i++) {
    c = &chip_cache_info[i];
    if (c->id == id) {
      c->age = age++;
      return i;
    }
    if (c->age < minage) {
      minage = c->age;
      mini = i;
    }
  }
  c = &chip_cache_info[mini];
  if (c->chip_data != NULL) {
    FreeMem (c->chip_data, c->len);
    c->chip_data = NULL;
  }
  c->id = id;
  c->age = age++;
  if ((c->chip_data = AllocMem (lengths[id], MEMF_CHIP)) == NULL)
    I_Error ("Out of CHIP memory allocating %d bytes", lengths[id]);
//  memcpy (c->chip_data, S_sfx[id].data, lengths[id]);
  CopyMem (S_sfx[id].data, c->chip_data, lengths[id]);
  c->len = lengths[id];
  return mini;
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
  struct channel_info *c;

/*  fprintf (stderr, "I_StartSound(%d,%d,%d,%d,%d,%d)\n", id, cnum, vol, sep,
              pitch, priority); */

  if (DoomSndBase != NULL) {
    I_StopSound(cnum);
    Sfx_Start (S_sfx[id].data, cnum, changepitch ? freqs[pitch] : 11025,
               vol<<3, sep, lengths[id]);
    return cnum;
  }

  if (!audio_is_open)
    return 1;
  I_StopSound (cnum);
  c = &channel_info[cnum];
  c->audio_io->ioa_Request.io_Command = CMD_WRITE;
  c->audio_io->ioa_Request.io_Flags = ADIOF_PERVOL;
  c->audio_io->ioa_Data = &chip_cache_info[cache_chip_data (id)].chip_data[8];
  c->audio_io->ioa_Length = lengths[id] - 8;
  c->audio_io->ioa_Period = period_table[pitch];
  c->audio_io->ioa_Volume = vol << 2;
  c->audio_io->ioa_Cycles = 1;
  BeginIO ((struct IORequest *)c->audio_io);
  c->sound_in_progress = TRUE;
  return cnum;
}

/**********************************************************************/
// Stops a sound channel.
void I_StopSound(int handle)
{
  /* fprintf (stderr, "I_StopSound(%d)\n", handle); */

  if (DoomSndBase != NULL) {
    Sfx_Stop(handle);
    return;
  }

  if (!audio_is_open)
    return;
  if (channel_info[handle].sound_in_progress) {
    AbortIO ((struct IORequest *)channel_info[handle].audio_io);
    WaitPort (channel_info[handle].audio_mp);
    GetMsg (channel_info[handle].audio_mp);
    channel_info[handle].sound_in_progress = FALSE;
  }
}

/**********************************************************************/
// Called by S_*() functions
//  to see if a channel is still playing.
// Returns 0 if no longer playing, 1 if playing.
int I_SoundIsPlaying(int handle)
{
  /* fprintf (stderr, "I_SoundIsPlaying(%d)\n", handle); */

  if (DoomSndBase != NULL) {
    return Sfx_Done(handle) ? 1 : 0;
  }

  if (!audio_is_open)
    return 0;
  if (channel_info[handle].sound_in_progress) {
    if (CheckIO ((struct IORequest *)channel_info[handle].audio_io)) {
      WaitPort (channel_info[handle].audio_mp);  /* clears signal & returns immediately */
      GetMsg (channel_info[handle].audio_mp);
      channel_info[handle].sound_in_progress = FALSE;
      return 0;
    } else {
      return 1;
    }
  }
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
/*
  fprintf (stderr, "I_UpdateSoundParams(%d,%d,%d,%d)\n", handle, vol,
           sep, pitch);
*/

  if (DoomSndBase != NULL) {
    Sfx_Update(handle, changepitch ? freqs[pitch] : 11025, vol<<3, sep);
    return;
  }

  if (!audio_is_open)
    return;
  if (channel_info[handle].sound_in_progress) {
    audio_io->ioa_Request.io_Command = ADCMD_PERVOL;
    audio_io->ioa_Request.io_Flags = ADIOF_PERVOL;
    audio_io->ioa_Request.io_Unit = (struct Unit *)(1 << handle);
    audio_io->ioa_Period = period_table[pitch];
    audio_io->ioa_Volume = vol << 2;
    BeginIO ((struct IORequest *)audio_io);
    WaitPort (audio_mp);
    GetMsg (audio_mp);
  }
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
//  fprintf (stderr, "I_InitMusic()\n");

  if (M_CheckParm("-music") && (DoomSndBase != NULL))
    music_okay = 1;
}

/**********************************************************************/
void I_ShutdownMusic(void)
{
//  fprintf (stderr, "I_ShutdownMusic()\n");
}

/**********************************************************************/
// Volume.
void I_SetMusicVolume(int volume)
{
//  fprintf (stderr, "I_SetMusicVolume(%d)\n", volume);
  snd_MusicVolume = volume;

  if (music_okay) {
    Mus_SetVol((volume<<2)+4);
  }
}

/**********************************************************************/
// PAUSE game handling.
void I_PauseSong(int handle)
{
//  fprintf (stderr, "I_PauseSong(%d)\n", handle);

  if (music_okay) {
    Mus_Pause(handle);
  }
}

/**********************************************************************/
void I_ResumeSong(int handle)
{
//  fprintf (stderr, "I_ResumeSong(%d)\n", handle);

  if (music_okay) {
    Mus_Resume(handle);
  }
}

/**********************************************************************/
// Registers a song handle to song data.
int I_RegisterSong(void *data)
{
//  fprintf (stderr, "I_RegisterSong(%08x)\n", data);

  if (music_okay) {
    return Mus_Register(data);
  } else
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
//  fprintf (stderr, "I_PlaySong(%d,%d)\n", handle, looping);

  if (music_okay) {
    Mus_Play(handle, looping);
  }

  musicdies = gametic + TICRATE*30;
}

/**********************************************************************/
// Stops a song over 3 seconds.
void I_StopSong(int handle)
{
//  fprintf (stderr, "I_StopSong(%d)\n", handle);

  if (music_okay) {
    Mus_Stop(handle);
  }
  musicdies = 0;
}

/**********************************************************************/
// See above (register), then think backwards
void I_UnRegisterSong(int handle)
{
//  fprintf (stderr, "I_UnRegisterSong(%d)\n", handle);

  if (music_okay) {
    Mus_Unregister(handle);
  }
  handle = 0;
}

/**********************************************************************/


/**********************************************************************/
void _STDaudio_cleanup (void)
{
  I_ShutdownSound ();
  I_ShutdownMusic ();
}

/**********************************************************************/
