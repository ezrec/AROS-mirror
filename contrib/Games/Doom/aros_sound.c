#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <exec/exec.h>
#include <dos/dos.h>
#include <graphics/gfxbase.h>
#include <devices/ahi.h>

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
#include "m_swap.h"

#include "doomdef.h"

// Any value of numChannels set
// by the defaults code in M_misc is now clobbered by I_InitSound().
// number of channels available for sound effects

extern int numChannels;

/**********************************************************************/

// NUM_VOICES = SFX_VOICES + MUS_VOICES
#define NUM_VOICES 64
#define SFX_VOICES 32
#define MUS_VOICES 32

struct Channel {
  float pitch;
  float pan;
  float vol;
  float ltvol;
  float rtvol;
  int instrument;
  int map[128];
};

struct midiHdr {
  ULONG rate;
  ULONG loop;                          // 16.16 fixed pt
  ULONG length;                        // 16.16 fixed pt
  UWORD base;                          // note base of sample
  BYTE sample[8];                      // actual length varies
};

struct Voice {
  BYTE *wave;
  float index;
  float step;
  ULONG loop;
  ULONG length;
  float ltvol;
  float rtvol;
  UWORD base;
  UWORD flags;
  int chan;
};

static int voice_avail[MUS_VOICES];
static struct Channel mus_channel[16];
static struct Voice audVoice[NUM_VOICES];
static struct Voice midiVoice[256];

// The actual lengths of all sound effects.
static int lengths[NUMSFX];

static struct Task *sound_task = NULL;
static struct MsgPort *audio_mp = NULL;
static struct AHIRequest *audio_io1 = NULL;
static struct AHIRequest *audio_io2 = NULL;

static WORD *buffer1 = NULL;
static WORD *buffer2 = NULL;

static ULONG NUM_SAMPLES = 1260;       // 1260 = 35Hz, 630 = 70Hz, 315 = 140Hz
static ULONG BUFFER_SIZE = (1260 * 2 * 2);        // stereo 16-bit samples
static ULONG BEATS_PER_PASS = 4;       // 4 = 35Hz, 2 = 70Hz, 1 = 140Hz

static int sound_status = 0;

static BOOL audio_is_open = FALSE;

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

static int note_table[128] = {
65536/64,69433/64,73562/64,77936/64,82570/64,87480/64,92682/64,98193/64,104032/64,110218/64,116772/64,123715/64,
65536/32,69433/32,73562/32,77936/32,82570/32,87480/32,92682/32,98193/32,104032/32,110218/32,116772/32,123715/32,
65536/16,69433/16,73562/16,77936/16,82570/16,87480/16,92682/16,98193/16,104032/16,110218/16,116772/16,123715/16,
65536/8,69433/8,73562/8,77936/8,82570/8,87480/8,92682/8,98193/8,104032/8,110218/8,116772/8,123715/8,
65536/4,69433/4,73562/4,77936/4,82570/4,87480/4,92682/4,98193/4,104032/4,110218/4,116772/4,123715/4,
65536/2,69433/2,73562/2,77936/2,82570/2,87480/2,92682/2,98193/2,104032/2,110218/2,116772/2,123715/2,
65536,69433,73562,77936,82570,87480,92682,98193,104032,110218,116772,123715,
65536*2,69433*2,73562*2,77936*2,82570*2,87480*2,92682*2,98193*2,104032*2,110218*2,116772*2,123715*2,
65536*4,69433*4,73562*4,77936*4,82570*4,87480*4,92682*4,98193*4,104032*4,110218*4,116772*4,123715*4,
65536*8,69433*8,73562*8,77936*8,82570*8,87480*8,92682*8,98193*8,104032*8,110218*8,116772*8,123715*8,
65536*16,69433*16,73562*16,77936*16,82570*16,87480*16,92682*16,98193*16
};

static float pitch_table[256];

static float master_vol =  64.0f;

/**********************************************************************/

ULONG sound_handler(char *args);

void Sfx_Start(char *wave, int cnum, int step, int vol, int sep, int length);
void Sfx_Update(int cnum, int step, int vol, int sep);
void Sfx_Stop(int cnum);
int Sfx_Done(int cnum);

void Mus_SetVol(int vol);
int Mus_Register(void *musdata);
void Mus_Unregister(int handle);
void Mus_Play(int handle, int looping);
void Mus_Stop(int handle);
void Mus_Pause(int handle);
void Mus_Resume(int handle);

/**********************************************************************/
//
// This function loads the sound data for sfxname from the WAD lump,
// and returns a ptr to the data in fastmem and its length in len.
//
static void *getsfx (char *sfxname, int *len)
{
  unsigned char *sfx;
  unsigned char *cnvsfx;
  int i;
  int size;
  char name[32];
  int sfxlump;

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
  cnvsfx = (unsigned char*)Z_Malloc (size, PU_STATIC, 0);

  // Now copy and convert offset to signed.
  for (i = 0; i < size; i++)
    cnvsfx[i] = sfx[i] ^ 0x80;

  // Remove the cached lump.
  Z_Free( sfx );

  // return length.
  *len = size;

  // Return allocated converted data.
  return (void *)cnvsfx;
}

/**********************************************************************/
// Init at program start...
void I_InitSound (void)
{
  int i;

  if (M_CheckParm("-nosfx"))
    return;

  if (M_CheckParm("-music")) {
    if (M_CheckParm("-70Hz")) {
      NUM_SAMPLES = 630;
      BUFFER_SIZE = NUM_SAMPLES * 2 * 2;
      BEATS_PER_PASS = 2;
      fprintf (stderr, "I_InitSound: Playing music at 70Hz.\n");
      fflush( stderr );
    } else if (M_CheckParm("-140Hz")) {
      NUM_SAMPLES = 315;
      BUFFER_SIZE = NUM_SAMPLES * 2 * 2;
      BEATS_PER_PASS = 1;
      fprintf (stderr, "I_InitSound: Playing music at 140Hz.\n");
      fflush( stderr );
    }
  }

  sound_status = 0;

  // create sound handling task
  sound_task = (struct Task *)CreateNewProcTags( NP_Output, Output(),
                                                 NP_Input, Input(),
					                             NP_Name, (IPTR)"doom_sound_daemon",
					                             NP_CloseOutput, FALSE,
                                                 NP_CloseInput, FALSE,
					                             NP_StackSize, 20000,
					                             NP_Entry, (IPTR)sound_handler,
                                                 NP_Priority, 40,
					                             TAG_DONE);
  if (!sound_task) {
    fprintf (stderr, "I_InitSound: Cannot create sound daemon, no sfx or music.\n");
    fflush( stderr );
    return;
  }
  while (!sound_status)
    Delay(10);

  switch (sound_status) {
    case 1:
      fprintf (stderr, "I_InitSound: Sound daemon running.\n");
      fflush( stderr );
      break;
    case -1:
      fprintf (stderr, "I_InitSound: Cannot create audio msgport, no sfx or music.\n");
      fflush( stderr );
      return;
    case -2:
      fprintf (stderr, "I_InitSound: Cannot create audio request, no sfx or music.\n");
      fflush( stderr );
      return;
    case -3:
      fprintf (stderr, "I_InitSound: Cannot open AHI device, no sfx or music.\n");
      fflush( stderr );
      return;
    case -4:
      fprintf (stderr, "I_InitSound: Cannot create double-buffer request, no sfx or music.\n");
      fflush( stderr );
      return;
    case -5:
      fprintf (stderr, "I_InitSound: Cannot allocate mixing buffers, no sfx or music.\n");
      fflush( stderr );
      return;
    case 2:
      fprintf (stderr, "I_InitSound: Cannot open MIDI instruments file, no music.\n");
      fflush( stderr );
      break;                           // non-fatal error
    case 3:
      fprintf (stderr, "I_InitSound: Cannot alloc memory for MIDI instruments, no music.\n");
      fflush( stderr );
      break;                           // non-fatal error
    default:
      fprintf (stderr, "I_InitSound: Error while initializing sound daemon, no sfx or music.\n");
      fflush( stderr );
      return;
  }

  fprintf (stderr, "I_InitSound: AHI audio initialized.\n" );
  fflush( stderr );

  changepitch = M_CheckParm ("-changepitch");

  // Initialize external data (all sounds) at start, keep static.
  for (i = 1; i < NUMSFX; i++) {
    // Alias? Example is the chaingun sound linked to pistol.
    if (!S_sfx[i].link) {
      // Load data from WAD file.
      S_sfx[i].data = getsfx(S_sfx[i].name, &lengths[i]);
    } else {
      // Previously loaded already?
      S_sfx[i].data = S_sfx[i].link->data;
      lengths[i] = lengths[(S_sfx[i].link - S_sfx)/sizeof(sfxinfo_t)];
    }
  }
  fprintf (stderr, "I_InitSound: Pre-cached all sound data.\n");
  fflush( stderr );

  // fill in pitch wheel table
  for (i=0; i<128; i++)
    pitch_table[i] = 1.0f + (-3678.0f * (float)(128-i) / 64.0f) / 65536.0f;
  for (i=0; i<128; i++)
    pitch_table[i+128] = 1.0f + (3897.0f * (float)i / 64.0f) / 65536.0f;

  // Finished initialization.
  fprintf (stderr, "I_InitSound: Sound module ready.\n");
  fflush( stderr );
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
//  fprintf (stderr, "I_ShutdownSound:");
//  fflush( stderr );

  if (sound_status > 0) {
    sound_status = 0xDEADBEEF;         // DIE, DAEMON! DIE!!
    while (sound_status)
      Delay(10);
  }

//  fprintf (stderr, " Sound module closed.\n");
//  fflush( stderr );
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

//  fprintf (stderr, "I_GetSfxLumpNum()\n");

  sprintf(namebuf, "ds%s", sfx->name);
  return W_GetNumForName(namebuf);
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
//  fprintf (stderr, "I_StartSound(%d,%d,%d,%d,%d,%d)\n", id, cnum, vol, sep, pitch, priority);

  if (audio_is_open) {
    I_StopSound(cnum);
    Sfx_Start (S_sfx[id].data, cnum, changepitch ? freqs[pitch] : 11025,
               vol, sep, lengths[id]);
  }
  return cnum;
}

/**********************************************************************/
// Stops a sound channel.
void I_StopSound(int handle)
{
//  fprintf (stderr, "I_StopSound(%d)\n", handle);

  if (audio_is_open)
    Sfx_Stop(handle);
}

/**********************************************************************/
// Called by S_*() functions
//  to see if a channel is still playing.
// Returns 0 if no longer playing, 1 if playing.
int I_SoundIsPlaying(int handle)
{
//  fprintf (stderr, "I_SoundIsPlaying(%d)\n", handle);

  if (audio_is_open)
    return Sfx_Done(handle) ? 1 : 0;

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
//  fprintf (stderr, "I_UpdateSoundParams(%d,%d,%d,%d)\n", handle, vol, sep, pitch);

  if (audio_is_open)
    Sfx_Update(handle, changepitch ? freqs[pitch] : 11025, vol, sep);
}

/**********************************************************************/
/**********************************************************************/
//
// MUSIC API.
//

static int musicdies=-1;
static int music_okay = 0;

static APTR midi_instruments = NULL;

static UWORD score_len, score_start, inst_cnt;
static void *score_data;
static UBYTE *score_ptr;

static int mus_delay = 0;
static int mus_playing = 0;
static int mus_looping = 0;
static float mus_volume = 1.0f;

/**********************************************************************/
//
//  MUSIC I/O
//
void I_InitMusic(void)
{
  fprintf (stderr, "I_InitMusic:");
  fflush( stderr );

  if (M_CheckParm("-music") && (music_okay == 1)) {
    fprintf (stderr, " Music okay.\n");
    fflush( stderr );

    if (M_CheckParm("-70Hz")) {
      NUM_SAMPLES = 630;
      BUFFER_SIZE = NUM_SAMPLES * 2 * 2;
      BEATS_PER_PASS = 2;
    } else if (M_CheckParm("-140Hz")) {
      NUM_SAMPLES = 315;
      BUFFER_SIZE = NUM_SAMPLES * 2 * 2;
      BEATS_PER_PASS = 1;
    }

    return;
  }

  music_okay = 0;
  fprintf (stderr, " No music.\n");
  fflush( stderr );
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

  if (music_okay)
    Mus_SetVol(volume);
}

/**********************************************************************/
// PAUSE game handling.
void I_PauseSong(int handle)
{
//  fprintf (stderr, "I_PauseSong(%d)\n", handle);

  if (music_okay)
    Mus_Pause(handle);
}

/**********************************************************************/
void I_ResumeSong(int handle)
{
//  fprintf (stderr, "I_ResumeSong(%d)\n", handle);

  if (music_okay)
    Mus_Resume(handle);
}

/**********************************************************************/
// Registers a song handle to song data.
int I_RegisterSong(void *data)
{
//  fprintf (stderr, "I_RegisterSong(%08x)\n", data);

  if (music_okay)
    return Mus_Register(data);

  return 0;
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

  if (music_okay)
    Mus_Play(handle, looping);

  musicdies = gametic + TICRATE*30;
}

/**********************************************************************/
// Stops a song over 3 seconds.
void I_StopSong(int handle)
{
//  fprintf (stderr, "I_StopSong(%d)\n", handle);

  if (music_okay)
    Mus_Stop(handle);

  musicdies = 0;
}

/**********************************************************************/
// See above (register), then think backwards
void I_UnRegisterSong(int handle)
{
//  fprintf (stderr, "I_UnRegisterSong(%d)\n", handle);

  if (music_okay)
    Mus_Unregister(handle);
}

/**********************************************************************/

void _STDaudio_cleanup (void)
{
  I_ShutdownSound ();
  I_ShutdownMusic ();
}

/**********************************************************************/

void Sfx_Start(char *wave, int cnum, int step, int volume, int seperation, int length)
{
  float vol = (float)volume;
  float sep = (float)seperation;

  vol = (volume > 15) ? 127.0f : vol * 8.0f + 7.0f;

  Forbid();
  audVoice[cnum].wave = wave + 8;
  audVoice[cnum].index = 0.0f;
  audVoice[cnum].step = (float)step / 44100.0f;
  audVoice[cnum].loop = 0;
  audVoice[cnum].length = length - 8;
  audVoice[cnum].ltvol = (vol - (vol * sep * sep) / 65536.0f) / 127.0f;
  sep -= 256.0f;
  audVoice[cnum].rtvol = (vol - (vol * sep * sep) / 65536.0f) / 127.0f;
  audVoice[cnum].flags = 0x81;
  Permit();
}

void Sfx_Update(int cnum, int step, int volume, int seperation)
{
  float vol = (float)volume;
  float sep = (float)seperation;

  vol = (volume > 15) ? 127.0f : vol * 8.0f + 7.0f;

  Forbid();
  audVoice[cnum].step = (float)step / 44100.0f;
  audVoice[cnum].ltvol = (vol - (vol * sep * sep) / 65536.0f) / 127.0f;
  sep -= 256.0f;
  audVoice[cnum].rtvol = (vol - (vol * sep * sep) / 65536.0f) / 127.0f;
  Permit();
}

void Sfx_Stop(int cnum)
{
  Forbid();
  audVoice[cnum].flags &= 0xFE;
  Permit();
}

int Sfx_Done(int cnum)
{
  int done;

  Forbid();
  done = (audVoice[cnum].flags & 0x01) ? (int)audVoice[cnum].index + 1 : 0;
  Permit();
  return done;
}

/**********************************************************************/

void Mus_SetVol(int vol)
{
  mus_volume = (vol > 15) ? 1.0f : ((float)vol * 8.0f + 7.0f) / 127.0f;
}

int Mus_Register(void *musdata)
{
  ULONG *lptr = (ULONG *)musdata;
  UWORD *wptr = (UWORD *)musdata;
  //UBYTE *bptr = (UBYTE *)musdata;
  //UWORD ix, iy;

  Mus_Unregister(1);
  // music won't start playing until mus_playing set at this point

  if(lptr[0] != SWAPLONG(0x1a53554d))  // 0x4d55531a
    return 0;                          // "MUS",26 always starts a vaild MUS file
  score_len = SWAPSHORT(wptr[2]);      // score length
  if(!score_len)
    return 0;                          // illegal score length
  score_start = SWAPSHORT(wptr[3]);    // score start
  if(score_start < 18)
    return 0;                          // illegal score start offset

  inst_cnt = SWAPSHORT(wptr[6]);       // instrument count
  if(!inst_cnt)
    return 0;                          // illegal instrument count

  // okay, MUS data seems to check out

  score_data = musdata;
  return 1;
}

void Mus_Unregister(int handle)
{
  Mus_Stop(handle);
  // music won't start playing until mus_playing set at this point

  score_data = 0;
  score_len = 0;
  score_start = 0;
  inst_cnt = 0;
}

void Mus_Play(int handle, int looping)
{
  if (!handle)
    return;

  mus_looping = looping;
  mus_playing = 2;                     // 2 = play from start
}

void Mus_Stop(int handle)
{
  int ix;

  if(mus_playing) {
    mus_playing = -1;                  // stop playing
    while (mus_playing)
      Delay(10);
  }

  // disable instruments in score (just disable them all)
  for (ix=SFX_VOICES; ix<NUM_VOICES; ix++)
    audVoice[ix].flags = 0x00;        // disable voice

  mus_looping = 0;
  mus_delay = 0;
}

void Mus_Pause(int handle)
{
  mus_playing = 0;                     // 0 = not playing
}

void Mus_Resume(int handle)
{
  mus_playing = 1;                     // 1 = play from current position
}

/**********************************************************************/

void fill_buffer(WORD *buffer)
{
  float index;
  float step;
  ULONG loop;
  ULONG length;
  float ltvol;
  float rtvol;
  BYTE *wvbuff;
  WORD *smpbuff = buffer;
  float sample;
  int ix;
  int iy;

  // process music if playing
  if (mus_playing) {
    if (mus_playing < 0)
      mus_playing = 0;                 // music now off
    else {
      if (mus_playing > 1) {
        mus_playing = 1;
        // start music from beginning
        score_ptr = (UBYTE *)((ULONG)score_data + (ULONG)score_start);
      }
      mus_delay -= BEATS_PER_PASS;     // 1=140Hz, 2=70Hz, 4=35Hz
      if (mus_delay <= 0) {
        UBYTE event;
        UBYTE note;
        UBYTE time;
        UBYTE ctrl;
        UBYTE value;
        int next_time;
        int channel;
        int voice;
        int inst;
        float volume;
        float pan;

nextEvent:        // next event
        do {
          event = *score_ptr++;
          switch((event >> 4) & 7) {
            case 0:                    // Release
              channel = (int)(event & 15);
              note = *score_ptr++;
              note &= 0x7f;
              voice = mus_channel[channel].map[(ULONG)note] - 1;
              if (voice >= 0) {
                mus_channel[channel].map[(ULONG)note] = 0;       // clear mapping
                voice_avail[voice] = 0;                        // voice available
                audVoice[voice + SFX_VOICES].flags |= 0x02;    // voice releasing
              }
              break;
            case 1:                    // Play note
              channel = (int)(event & 15);
              note = *score_ptr++;
              volume = -1.0f;
              if (note & 0x80) {       // set volume as well
                note &= 0x7f;
                volume = (float)*score_ptr++;
              }
              for (voice=0; voice<MUS_VOICES; voice++) {
                if (!voice_avail[voice])
                  break;               // found free voice
              }
              if (voice < MUS_VOICES) {
                voice_avail[voice] = 1;        // in use
                mus_channel[channel].map[(ULONG)note] = voice + 1;
                if (volume >= 0.0f) {
                  mus_channel[channel].vol = volume;
                  pan = mus_channel[channel].pan;
                  mus_channel[channel].ltvol = (volume - (volume * pan * pan) / 65536.0f) / 127.0f;
                  pan -= 256.0f;
                  mus_channel[channel].rtvol = (volume - (volume * pan * pan) / 65536.0f) / 127.0f;
                }
                audVoice[voice + SFX_VOICES].ltvol = mus_channel[channel].ltvol;
                audVoice[voice + SFX_VOICES].rtvol = mus_channel[channel].rtvol;
                if (channel != 15) {
                  inst = mus_channel[channel].instrument;
                  audVoice[voice + SFX_VOICES].chan = channel; // back link for pitch wheel
                  audVoice[voice + SFX_VOICES].wave = midiVoice[inst].wave;
                  audVoice[voice + SFX_VOICES].index = 0.0f;
                  audVoice[voice + SFX_VOICES].step = (float)note_table[(72 - midiVoice[inst].base + (ULONG)note) & 0x7f] / 262144.0f;
                  audVoice[voice + SFX_VOICES].loop = midiVoice[inst].loop >> 16;
                  audVoice[voice + SFX_VOICES].length = midiVoice[inst].length >> 16;
                  audVoice[voice + SFX_VOICES].flags = 0x01; // enable voice
                } else {
                  // percussion channel - note is percussion instrument
                  inst = (ULONG)note + 100;
                  audVoice[voice + SFX_VOICES].chan = channel; // back link for pitch wheel
                  audVoice[voice + SFX_VOICES].wave = midiVoice[inst].wave;
                  audVoice[voice + SFX_VOICES].index = 0.0f;
                  audVoice[voice + SFX_VOICES].step = 11025.0f / 44100.0f;
                  audVoice[voice + SFX_VOICES].loop = midiVoice[inst].loop >> 16;
                  audVoice[voice + SFX_VOICES].length = midiVoice[inst].length >> 16;
                  audVoice[voice + SFX_VOICES].flags = 0x01; // enable voice
                }
              }
              break;
            case 2:                    // Pitch
              channel = (int)(event & 15);
              mus_channel[channel].pitch = pitch_table[(ULONG)*score_ptr++ & 0xff];
              break;
            case 3:                    // Tempo
              score_ptr++;             // skip value - not supported
              break;
            case 4:                    // Change control
              channel = (int)(event & 15);
              ctrl = *score_ptr++;
              value = *score_ptr++;
              switch(ctrl) {
                case 0:
                  // set channel instrument
                  mus_channel[channel].instrument = (ULONG)value;
                  mus_channel[channel].pitch = 1.0f;
                  break;
                case 3:
                  // set channel volume
                  mus_channel[channel].vol = volume = (float)value;
                  pan = mus_channel[channel].pan;
                  mus_channel[channel].ltvol = (volume - (volume * pan * pan) / 65536.0f) / 127.0f;
                  pan -= 256.0f;
                  mus_channel[channel].rtvol = (volume - (volume * pan * pan) / 65536.0f) / 127.0f;
                  break;
                case 4:
                  // set channel pan
                  mus_channel[channel].pan = pan = (float)value;
                  volume = mus_channel[channel].vol;
                  mus_channel[channel].ltvol = (volume - (volume * pan * pan) / 65536.0f) / 127.0f;
                  pan -= 256.0f;
                  mus_channel[channel].rtvol = (volume - (volume * pan * pan) / 65536.0f) / 127.0f;
                  break;
              }
              break;
            case 6:                    // End score
              if (mus_looping)
                score_ptr = (UBYTE *)((ULONG)score_data + (ULONG)score_start);
              else {
                for (voice=0; voice<MUS_VOICES; voice++) {
                  audVoice[voice + SFX_VOICES].flags = 0;
                  voice_avail[voice] = 0;
                }
                mus_delay = 0;
                mus_playing = 0;
                goto mix;
              }
              break;
          }
        } while (!(event & 0x80));     // not last event

        // now get the time until the next event(s)
        next_time = 0;
        time = *score_ptr++;
        while (time & 0x80) {
          // while msb set, accumulate 7 bits
          next_time |= (ULONG)(time & 0x7f);
          next_time <<= 7;
          time = *score_ptr++;
        }
        next_time |= (ULONG)time;
        mus_delay += next_time;
        if (mus_delay <= 0)
          goto nextEvent;
      }
    }
  }

mix:
  // clear buffer
  memset((void *)buffer, 0, BUFFER_SIZE);

  // mix enabled voices
  for (ix=0; ix<NUM_VOICES; ix++) {
    if (audVoice[ix].flags & 0x01) {
      // process enabled voice
      if (!(audVoice[ix].flags & 0x80) && !(mus_playing))
        continue;                      // skip instrument if music not playing

      index = audVoice[ix].index;
      step = audVoice[ix].step;
      loop = audVoice[ix].loop;
      length = audVoice[ix].length;
      ltvol = audVoice[ix].ltvol;
      rtvol = audVoice[ix].rtvol;
      wvbuff = audVoice[ix].wave;

      if (!(audVoice[ix].flags & 0x80)) {
        // special handling for instrument
        if (audVoice[ix].flags & 0x02) {
          // releasing
          ltvol *= 0.90f;
          rtvol *= 0.90f;
          audVoice[ix].ltvol = ltvol;
          audVoice[ix].rtvol = rtvol;
          if (ltvol <= 0.02f && rtvol <= 0.02f) {
            audVoice[ix].flags = 0;      // disable voice
            continue;                    // next voice
          }
        }
        step *= mus_channel[audVoice[ix].chan & 15].pitch;
        ltvol *= mus_volume;
        rtvol *= mus_volume;
      }

      for (iy=0; iy<(NUM_SAMPLES<<1); iy+=2) {
        if ((ULONG)index >= length) {
          if (!(audVoice[ix].flags & 0x80)) {
            // check if instrument has loop
            if (loop) {
              index -= (float)length;
              index += (float)loop;
            } else {
              audVoice[ix].flags = 0;  // disable voice
              break;                   // exit sample loop
            }
          } else {
            audVoice[ix].flags = 0;  // disable voice
            break;                   // exit sample loop
          }
        }
        sample = (wvbuff) ? (float)wvbuff[(int)index] : 0.0f;  // for safety
        smpbuff[iy] += (WORD)(sample * ltvol * master_vol);
        smpbuff[iy + 1] += (WORD)(sample * rtvol * master_vol);
        index += step;
      }
      audVoice[ix].index = index;
    }
  }
}

/**********************************************************************/
// Need swap routines because the MIDI file was made on the Amiga and
//   is in big endian format. :)

#ifdef __BIG_ENDIAN__

#define WSWAP(x) x
#define LSWAP(x) x

#else

UWORD WSWAP(UWORD x)
{
    return (UWORD)((x>>8) | (x<<8));
}

ULONG LSWAP(ULONG x)
{
    return
	(x>>24)
	| ((x>>8) & 0xff00)
	| ((x<<8) & 0xff0000)
	| (x<<24);
}

#endif

/**********************************************************************/

ULONG sound_handler(char *args)
{
  FILE *hnd;
  int size;
  int i;
  ULONG *miptr;
  struct midiHdr *mhdr;
  int current = 0;
  ULONG playing = 0;

  // try to initialize AHI
  if(!(audio_mp = CreateMsgPort())) {
    sound_status = -1;
    return 0;
  }

  if(!(audio_io1 = (struct AHIRequest *)CreateIORequest(audio_mp, sizeof(struct AHIRequest)))) {
    DeleteMsgPort(audio_mp);
    sound_status = -2;
    return 0;
  }
  audio_io1->ahir_Version = 4;

  if(OpenDevice(AHINAME, 0, (struct IORequest *)audio_io1, 0)) {
    DeleteIORequest((struct IORequest *)audio_io1);
    DeleteMsgPort(audio_mp);
    sound_status = -3;
    return 0;
  }

  // init double-buffer iorequest
  if(!(audio_io2 = AllocVec(sizeof(struct AHIRequest), MEMF_PUBLIC))) {
    CloseDevice((struct IORequest *)audio_io1);
    DeleteIORequest((struct IORequest *)audio_io1);
    DeleteMsgPort(audio_mp);
    sound_status = -4;
    return 0;
  }
  memcpy(audio_io2, audio_io1, sizeof(struct AHIRequest));

  // Allocate mixing buffers
  buffer1 = (WORD *)AllocVec(BUFFER_SIZE, MEMF_PUBLIC+MEMF_CLEAR);
  buffer2 = (WORD *)AllocVec(BUFFER_SIZE, MEMF_PUBLIC+MEMF_CLEAR);

  if(buffer1 == NULL || buffer2 == NULL) {
    CloseDevice((struct IORequest *)audio_io1);
    DeleteIORequest((struct IORequest *)audio_io1);
    DeleteMsgPort(audio_mp);
    FreeVec((APTR)audio_io2);
    audio_io2 = NULL;
    if(buffer1) {
      FreeVec((APTR)buffer1);
      buffer1 = NULL;
    }
    if(buffer2) {
      FreeVec((APTR)buffer2);
      buffer2 = NULL;
    }
    sound_status = -5;
    return 0;
  }

  numChannels = SFX_VOICES;
  audio_is_open = TRUE;

  music_okay = 0;

  // try to set up MIDI instruments
  hnd = fopen("PROGDIR:MIDI_Instruments","rb");
  if (hnd) {
    fseek(hnd, 0, SEEK_END);
    size = ftell(hnd);
    fseek(hnd, 0, SEEK_SET);

    midi_instruments = AllocVec(size, MEMF_PUBLIC+MEMF_CLEAR);
    if (midi_instruments) {
      fread((void *)midi_instruments, 1, size, hnd);
      fclose(hnd);
      // set midiVoice[] from file
      miptr = (ULONG *)midi_instruments;
      for(i=0; i<256; i++) {
        if(i>181 || miptr[i]==0 || LSWAP(miptr[i]) >= size) {
          midiVoice[i].wave = 0;
          midiVoice[i].index = 0.0f;
          midiVoice[i].step = 11025.0f / 44100.0f;
          midiVoice[i].loop = 0;
          midiVoice[i].length = 2000 << 16;
          midiVoice[i].ltvol = 0.0f;
          midiVoice[i].rtvol = 0.0f;
          midiVoice[i].base = 60;
          midiVoice[i].flags = 0x00;
        } else {
          mhdr = (struct midiHdr *)(midi_instruments + LSWAP(miptr[i])); 
          midiVoice[i].wave = &mhdr->sample[0];
          midiVoice[i].index = 0.0f;
          midiVoice[i].step = 11025.0f / 44100.0f;
          midiVoice[i].loop = LSWAP(mhdr->loop);
          midiVoice[i].length = LSWAP(mhdr->length);
          midiVoice[i].ltvol = 0.0f;
          midiVoice[i].rtvol = 0.0f;
          midiVoice[i].base = WSWAP(mhdr->base);
          midiVoice[i].flags = 0x00;
        }
      }
      music_okay = 1;
      sound_status = 1;
    } else {
      fclose(hnd);
      sound_status = 3;                // couldn't allocate memory
    }
  } else {
    sound_status = 2;                  // couldn't open MIDI file
  }

  // send first (empty) buffer, then enter main loop
  audio_io1->ahir_Std.io_Message.mn_Node.ln_Pri = 60;
  audio_io1->ahir_Std.io_Data                   = buffer1;
  audio_io1->ahir_Std.io_Length                 = BUFFER_SIZE;
  audio_io1->ahir_Std.io_Offset                 = 0;
  audio_io1->ahir_Std.io_Command                = CMD_WRITE;
  audio_io1->ahir_Frequency                     = 44100;
  audio_io1->ahir_Volume                        = 0x10000;
  audio_io1->ahir_Type                          = AHIST_S16S;
  audio_io1->ahir_Position                      = 0x8000;
  audio_io1->ahir_Link                          = NULL;
  SendIO((struct IORequest *)audio_io1);
  playing++;
  current = 1;

  // main loop
  while (sound_status != 0xDEADBEEF) {
    if (current) {
      if(playing>1)
        WaitIO((struct IORequest *)audio_io2);
      fill_buffer(buffer2);
      audio_io2->ahir_Std.io_Message.mn_Node.ln_Pri = 60;
      audio_io2->ahir_Std.io_Data                   = buffer2;
      audio_io2->ahir_Std.io_Length                 = BUFFER_SIZE;
      audio_io2->ahir_Std.io_Offset                 = 0;
      audio_io2->ahir_Std.io_Command                = CMD_WRITE;
      audio_io2->ahir_Frequency                     = 44100;
      audio_io2->ahir_Volume                        = 0x10000;
      audio_io2->ahir_Type                          = AHIST_S16S;
      audio_io2->ahir_Position                      = 0x8000;
      audio_io2->ahir_Link                          = audio_io1;
      SendIO((struct IORequest *)audio_io2);
      playing++;
      current = 0;
    } else {
      WaitIO((struct IORequest *)audio_io1);
      fill_buffer(buffer1);
      audio_io1->ahir_Std.io_Message.mn_Node.ln_Pri = 60;
      audio_io1->ahir_Std.io_Data                   = buffer1;
      audio_io1->ahir_Std.io_Length                 = BUFFER_SIZE;
      audio_io1->ahir_Std.io_Offset                 = 0;
      audio_io1->ahir_Std.io_Command                = CMD_WRITE;
      audio_io1->ahir_Frequency                     = 44100;
      audio_io1->ahir_Volume                        = 0x10000;
      audio_io1->ahir_Type                          = AHIST_S16S;
      audio_io1->ahir_Position                      = 0x8000;
      audio_io1->ahir_Link                          = audio_io2;
      SendIO((struct IORequest *)audio_io1);
      playing++;
      current = 1;
    }
  }

  // cleanup and shut down daemon
  if(playing>1) {
    AbortIO((struct IORequest *)audio_io2);
    WaitIO((struct IORequest *)audio_io2);
  }

  AbortIO((struct IORequest *)audio_io1);
  WaitIO((struct IORequest *)audio_io1);

  // double abort to be sure to break double-buffering
  if(playing>1) {
    AbortIO((struct IORequest *)audio_io2);
    WaitIO((struct IORequest *)audio_io2);
  }

  CloseDevice((struct IORequest *)audio_io1);
  DeleteIORequest((struct IORequest *)audio_io1);
  DeleteMsgPort(audio_mp);
  FreeVec((APTR)audio_io2);
  audio_io2 = NULL;
  FreeVec((APTR)buffer1);
  buffer1 = NULL;
  FreeVec((APTR)buffer2);
  buffer2 = NULL;

  if(midi_instruments) {
    FreeVec(midi_instruments);
    midi_instruments = NULL;
  }

  audio_is_open = FALSE;
  sound_status = 0;
  return 0;
}

/**********************************************************************/
