/*
 *
 * mpg123.h
 *
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/signal.h>
#include <unistd.h>
#include <math.h>

#define real float

#define FALSE 0
#define TRUE  1

#define SBLIMIT     32
#define SCALE_BLOCK 12
#define SSLIMIT     18

#define MPG_MD_STEREO       0
#define MPG_MD_JOINT_STEREO 1
#define MPG_MD_DUAL_CHANNEL 2
#define MPG_MD_MONO         3

struct al_table 
{
  short bits;
  short d;
};

struct frame {
  struct al_table *alloc;
  int (*synth)(real *,int,unsigned char *,int *);
  int (*synth_mono)(real *,unsigned char *,int *);
  int stereo;
  int jsbound;
  int single;
  int II_sblimit;
  int down_sample_sblimit;
  int lsf;
  int mpeg25;
  int down_sample;
  int header_change;
  int lay;
  int (*do_layer)(struct frame *fr);
  int error_protection;
  int bitrate_index;
  int sampling_frequency;
  int padding;
  int extension;
  int mode;
  int mode_ext;
  int copyright;
  int original;
  int emphasis;
  int framesize; /* computed framesize */
};

extern int head_read(unsigned long *newhead);
extern int head_shift(unsigned long *head);
extern int skip_bytes(int len);
extern int read_frame_body(unsigned char *buf, int size);

extern void audio_flush(void);
extern void set_pointer(long);

extern unsigned char *pcm_sample;
extern int pcm_point;
extern int audiobufsize;

struct bitstream_info {
  int bitindex;
  unsigned char *wordpointer;
};

extern struct bitstream_info bsi;

struct gr_info_s {
  int scfsi;
  unsigned part2_3_length;
  unsigned big_values;
  unsigned scalefac_compress;
  unsigned block_type;
  unsigned mixed_block_flag;
  unsigned table_select[3];
  unsigned subblock_gain[3];
  unsigned maxband[3];
  unsigned maxbandl;
  unsigned maxb;
  unsigned region1start;
  unsigned region2start;
  unsigned preflag;
  unsigned scalefac_scale;
  unsigned count1table_select;
  real *full_gain[3];
  real *pow2gain;
};

struct III_sideinfo
{
  unsigned main_data_begin;
  unsigned private_bits;
  struct {
    struct gr_info_s gr[2];
  } ch[2];
};

extern void read_frame_init(void);
extern int read_frame(struct frame *fr);
extern int do_layer3(struct frame *fr);
extern int do_layer2(struct frame *fr);
extern int do_layer1(struct frame *fr);

extern int synth_1to1 (real *,int,unsigned char *,int *);
extern int synth_1to1_8bit (real *,int,unsigned char *,int *);
extern int synth_1to1_mono (real *,unsigned char *,int *);
extern int synth_1to1_8bit_mono (real *,unsigned char *,int *);

extern int synth_2to1 (real *,int,unsigned char *,int *);
extern int synth_2to1_8bit (real *,int,unsigned char *,int *);
extern int synth_2to1_mono (real *,unsigned char *,int *);
extern int synth_2to1_8bit_mono (real *,unsigned char *,int *);

extern int synth_4to1 (real *,int,unsigned char *,int *);
extern int synth_4to1_8bit (real *,int,unsigned char *,int *);
extern int synth_4to1_mono (real *,unsigned char *,int *);
extern int synth_4to1_8bit_mono (real *,unsigned char *,int *);

extern void init_layer3(int);
extern void init_layer2(void);
extern void make_decode_tables(long scale);
extern void dct64(real *,real *,real *);

extern long freqs[9];
extern real muls[27][64];
extern real decwin[512+32];
extern real *pnts[5];
