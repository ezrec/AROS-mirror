/*
 *
 * mpg123.c
 *
 */

#include <stdlib.h>
#include <sys/types.h>

#include <plugin.h>

#include "mpg123.h"
#include "getbits.h"

#define HEADER_SIZE 4
#define MAXFRAMESIZE 1792

int tabsel_123[2][3][16] = {
  { {0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,},
    {0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,},
    {0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,} },

  { {0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,},
    {0, 8,16,24,32,40,48, 56, 64, 80, 96,112,128,144,160,},
    {0, 8,16,24,32,40,48, 56, 64, 80, 96,112,128,144,160,} }
};

long freqs[9] = { 44100, 48000, 32000, 22050, 24000, 16000 , 11025 , 12000 , 8000 };

struct bitstream_info bsi;

#define AUDIO_BUFFER 32768
int audiobufsize = AUDIO_BUFFER;

static int audioplaysize = 0;
static int mpega_error = 0;

static int fsizeold = 0, ssize = 0;
static unsigned char bsspace[2][MAXFRAMESIZE+512];
static unsigned char *bsbuf = bsspace[1], *bsbufold;
static int bsnum = 0;

unsigned char *pcm_sample = NULL;
int pcm_point = 0;

/* Sync stuff */
static double real_current_time = 0.0;
static double current_time = -1.0;
static double new_current_time = -1.0;
static double audio_sync_time = 0.0;

static int previous_pcm_point = 0;
static unsigned long audio_rate = 0;
static int counter = 0;

static struct frame fr;

/* For buffer and such */
static struct {
  unsigned char *inbuf;
  unsigned char *inbuf_ptr;
  unsigned char header[4];
  int is8bit;
  int channels;
} mpeg;

void set_synth_functions(struct frame *fr)
{
  typedef int (*func)(real *,int,unsigned char *,int *);
  typedef int (*func_mono)(real *,unsigned char *,int *);

  static func funcs[2][3] = { 
    { synth_1to1,
      synth_2to1,
      synth_4to1 },
    { synth_1to1_8bit,
      synth_2to1_8bit,
      synth_4to1_8bit } 
  };

  static func_mono funcs_mono[2][3] = {    
    { synth_1to1_mono,
      synth_2to1_mono,
      synth_4to1_mono },
    { synth_1to1_8bit_mono,
      synth_2to1_8bit_mono,
      synth_4to1_8bit_mono }
  };

  fr->synth = funcs[mpeg.is8bit][fr->down_sample];
  fr->synth_mono = funcs_mono[mpeg.is8bit][fr->down_sample];
}

unsigned long init_output(struct frame *fr)
{
  unsigned long rate = 0, bits, channels;
  char *modes[4] = { "stereo", "joint-stereo", "dual-channel", "mono" };
  char *layers[4] = { "Unknown" , "I", "II", "III" };

  sprintf(amp->verbose, "MPEG Audio : MPEG %s layer %s, %d kbit/s, %ld Hz %s",
    fr->mpeg25 ? "2.5" : (fr->lsf ? "2.0" : "1.0"),
    layers[fr->lay],
    tabsel_123[fr->lsf][fr->lay-1][fr->bitrate_index],
    freqs[fr->sampling_frequency], modes[fr->mode]);

  rate = freqs[fr->sampling_frequency];

  if (amp->desired_bits == 8) {
    mpeg.is8bit = 1;
    bits = 8;
  } else {
    mpeg.is8bit = 0;
    bits = 16;
  }

  if (amp->divisor == 2) {
    fr->down_sample = 1; /* reduce the frequency to 1/2, i.e. 22050Hz for 44100 Hz stream */
  } else if (amp->divisor == 4) {
    fr->down_sample = 2; /* reduce the frequency to 1/4, i.e. 11025Hz for 44100 Hz stream */
  } else {
    fr->down_sample = 0;
  }

  fr->down_sample_sblimit = SBLIMIT>>(fr->down_sample);

  rate = freqs[fr->sampling_frequency]>>(fr->down_sample);

  if (amp->desired_channels == 1) {
    fr->single = 3; /* Set mono when required */
    channels = 1;
  } else {
    fr->single = -1;
    channels = 2;
  }

  /* Force mono for mono streams */
  if (fr->stereo == 1) {
    channels = 1;
  }

  mpeg.channels = channels;

  if (amp->audio_init(rate, bits, channels) != PLUGIN_OK) {
    mpega_error = 1;
  }

  audioplaysize = rate / 4; /* Decode 4 times / second */

  /* Force decoding of same number of samples (time units) no matter what */

  if (channels == 2) {
    audioplaysize *= 2;
    audiobufsize *= 2; /* we have the same amount for each channel, so double it! */
  }

  if (bits == 16) {
    audioplaysize *= 2;
    audiobufsize *= 2; /* we are talking samples, not sample depth, so double it! */
  }

  pcm_sample = (unsigned char *)malloc(audiobufsize); /* FIXME: * 2 required ? */

  set_synth_functions(fr);

  init_layer3(fr->down_sample_sblimit);

  return rate;
}

void audio_flush(void)
{
  if (mpega_error != 0) {
    return;
  }

  amp->audio_refresh(pcm_sample, pcm_point, audio_sync_time);
  pcm_point = 0;
}

int header_check(unsigned long header)
{
  /* header */
  if ((header & 0xffe00000) != 0xffe00000) {
    return -1;
  }
  /* layer */
  if (((header >> 17) & 3) == 0) {
    return -1;
  }
  /* bit rate : currently no free format supported */
  if (((header >> 12) & 0xf) == 0xf) {
    return -1;
  }
  if (((header >> 12) & 0xf) == 0x0) {
    return -1;
  }
  /* frequency */
  if (((header >> 10) & 0x3) == 0x3) {
    return -1;
  }

  return 0;
}

static void decode_header(struct frame *fr, unsigned long header)
{
  if (header & (1<<20)) {
    fr->lsf = (header & (1<<19)) ? 0x0 : 0x1;
    fr->mpeg25 = 0;
  } else {
    fr->lsf = 1;
    fr->mpeg25 = 1;
  }

  fr->lay = 4-((header>>17)&3);
  if(fr->mpeg25) {
    fr->sampling_frequency = 6 + ((header>>10)&0x3);
  } else {
    fr->sampling_frequency = ((header>>10)&0x3) + (fr->lsf*3);
  }
  fr->error_protection = ((header>>16)&0x1)^0x1;

  fr->bitrate_index = ((header>>12)&0xf);
  fr->padding       = ((header>>9)&0x1);
  fr->extension     = ((header>>8)&0x1);
  fr->mode          = ((header>>6)&0x3);
  fr->mode_ext      = ((header>>4)&0x3);
  fr->copyright     = ((header>>3)&0x1);
  fr->original      = ((header>>2)&0x1);
  fr->emphasis      = header & 0x3;

  fr->stereo        = (fr->mode == MPG_MD_MONO) ? 1 : 2;

  switch(fr->lay) {
    case 1:
      fr->do_layer   = do_layer1;
      fr->framesize  = (long) tabsel_123[fr->lsf][0][fr->bitrate_index] * 12000;
      fr->framesize /= freqs[fr->sampling_frequency];
      fr->framesize  = ((fr->framesize+fr->padding)<<2)-4;
      break;

    case 2:
      fr->do_layer   = do_layer2;
      fr->framesize  = (long) tabsel_123[fr->lsf][1][fr->bitrate_index] * 144000;
      fr->framesize /= freqs[fr->sampling_frequency];
      fr->framesize += fr->padding - 4;
      break;

    case 3:
      fr->do_layer = do_layer3;
      if (fr->lsf) {
        ssize = (fr->stereo == 1) ? 9 : 17;
      } else {
        ssize = (fr->stereo == 1) ? 17 : 32;
      }
      if (fr->error_protection) {
        ssize += 2;
      }
      fr->framesize  = (long) tabsel_123[fr->lsf][2][fr->bitrate_index] * 144000;
      fr->framesize /= freqs[fr->sampling_frequency]<<(fr->lsf);
      fr->framesize  = fr->framesize + fr->padding - 4;
      break; 
  }
}

void set_pointer(long backstep)
{
  bsi.wordpointer = bsbuf + ssize - backstep;
  if (backstep) {
    memcpy(bsi.wordpointer,bsbufold+fsizeold-backstep,backstep);
  }
  bsi.bitindex = 0;
}

int decode_mpeg(unsigned char *buf, unsigned long length, double sync_time)
{
  unsigned long header;
  int len;

  if (mpega_error != 0) {
    return PLUGIN_FAIL;
  }

  /* Seek ? Get a new header */
  if (length == 0) {
    pcm_point = 0;
    previous_pcm_point = 0;
    counter = 3;
    fsizeold = 0;

    mpeg.inbuf = mpeg.header;
    mpeg.inbuf_ptr = mpeg.inbuf;
    fr.framesize = 0;

    return PLUGIN_OK;
  }

  new_current_time = sync_time;

  while (length > 0) {
    len = mpeg.inbuf_ptr - mpeg.inbuf;

    if (fr.framesize == 0) {
      /* We need a header */
      len = HEADER_SIZE - len;
      if (len > length) {
        len = length;
      }
      memcpy(mpeg.inbuf_ptr, buf, len);
      buf += len;
      mpeg.inbuf_ptr += len;
      length -= len;

      len = mpeg.inbuf_ptr - mpeg.inbuf;
      if (len == HEADER_SIZE) {
        header = *(unsigned long *)mpeg.inbuf; /* FIXME: Big endian only */

        if (header_check(header) < 0) {
          memcpy(mpeg.inbuf, mpeg.inbuf + 1, HEADER_SIZE - 1);
          mpeg.inbuf_ptr--;
        } else {
          decode_header(&fr, header);

          if (audio_rate == 0) {
            audio_rate = init_output(&fr);
          }

          /* flip/init buffer for Layer 3 */
          bsbufold = bsbuf;
          bsbuf = bsspace[bsnum]+512;
          bsnum = (bsnum + 1) & 1;

          /* read main data into memory */
          mpeg.inbuf = bsbuf;
          mpeg.inbuf_ptr = mpeg.inbuf;

          if (new_current_time > 0.0) {
            current_time = new_current_time;
            new_current_time = -1.0;
          }

          bsi.bitindex = 0;
          bsi.wordpointer = bsbuf;
        }
      }
    } else if (len < fr.framesize) {
      /* We need more bytes to complete the frame */
      len = fr.framesize - len;
      if (len > length) {
        len = length;
      }
      memcpy(mpeg.inbuf_ptr, buf, len);
      buf += len;
      mpeg.inbuf_ptr += len;
      length -= len;
    } else {
      /* We have full frame, decode it */
      int diff;

      /* Skip some frames after a seek to reduce noice */
      if (counter > 0) {
        counter--;
        goto get_new_header;
      }

      if (fr.error_protection) {
        getbits(16); /* skip crc */
      }

      /* do the decoding */
      fr.do_layer(&fr);

      /* Calculate time that should have passed during decoding */
      diff = pcm_point - previous_pcm_point;

      if (mpeg.is8bit == 0) {
        diff >>= 1;
      }
      if (mpeg.channels == 2) {
        diff >>= 1;
      }

      real_current_time += (double)diff / (double)audio_rate;

      if (current_time > 0.0) {
        real_current_time = current_time;
        current_time = -1.0;
      }

      /* Time to play the decoded audio ? */
      if (pcm_point > audioplaysize) {
        audio_flush();
        audio_sync_time = real_current_time;
      }

      previous_pcm_point = pcm_point;

      fsizeold = fr.framesize; /* for Layer3 */

      /* Time to get a new header */
get_new_header:
      mpeg.inbuf = mpeg.header;
      mpeg.inbuf_ptr = mpeg.inbuf;
      fr.framesize = 0;
    }
  }

  return PLUGIN_OK;
}

int init_mpeg(unsigned long type, unsigned long fourcc)
{
  memset(&mpeg, 0, sizeof(mpeg));
  memset(&fr, 0, sizeof(fr));

  audio_rate = 0;
  pcm_point = 0;
  previous_pcm_point = 0;
  counter = 0;
  fsizeold = 0;

  /* We need to get the header first */
  mpeg.inbuf = mpeg.header;
  mpeg.inbuf_ptr = mpeg.inbuf;
  fr.framesize = 0;

  make_decode_tables(32768);
  init_layer2(); /* also inits tables shared with layer1 */

  mpega_error = 0;

  return PLUGIN_OK;
}

int exit_mpeg(void)
{
  /* FIXME: Memory leak */

  return PLUGIN_OK;
}

VERSION("MPEGAX v1.00 (020216)");

IDENT_INIT
IDENT_ADD((TYPE_AUDIO | SUBTYPE_MPEGX | FEATURE_DIVISOR_2 | FEATURE_DIVISOR_4), FOURCC_NONE, init_mpeg, decode_mpeg, exit_mpeg)
IDENT_ADD((TYPE_AUDIO | SUBTYPE_NONE  | FEATURE_DIVISOR_2 | FEATURE_DIVISOR_4), FOURCC_MP3, init_mpeg, decode_mpeg, exit_mpeg)
IDENT_EXIT

