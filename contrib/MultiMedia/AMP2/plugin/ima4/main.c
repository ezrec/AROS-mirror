/*
 *
 * main.c
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <plugin.h>

#include "adpcm.h"

#define downScale 0 /* FIXME */
#define get16(dptr) ((dptr[0] << 8) | dptr[1]); dptr+=2 /* FIXME */

static void (*DecodeIMA4)(void *src, void *dst, unsigned long size);
static unsigned char *audio = NULL;

static void DecodeIMA4Mono(void *src, void *dst, unsigned long size)
{
  unsigned char *from = src;
  unsigned short *to = dst;
  long todo=(size/0x40*0x22);

  unsigned long blockCnt=0x40;

  unsigned long hiNibble = 0;

  while (todo>0) {
    long valPred;
    long index;
    long step;
    unsigned long n;
    unsigned char firstNibble;

    valPred=get16(from);
    index=valPred & 0x7f;
    if (index>88) index=88;
    valPred &= 0xff80;
    if (valPred & 0x8000) valPred-=0x10000;

    step=stepSizeTable[index];
    todo-=2;
    if (todo<=0) return;

    firstNibble=1;
    for(n=0; n<blockCnt; n++) {
      unsigned long loNibble;

      if (firstNibble) {
        loNibble=*from++;
        hiNibble=(loNibble>>4) & 0x0f;
        loNibble &= 0x0f;
        todo--;
        if (todo<0) return;
        if (downScale)
          n++;
        else
          firstNibble=0;
      } else {
        loNibble=hiNibble;
        firstNibble=1;
      }
      calcDVI(valPred,loNibble,index,step);
      *to++=(unsigned short)valPred;
    }
  }
}

static void DecodeIMA4Stereo(void *src, void *dst, unsigned long size)
{
  unsigned char *from = src;
  unsigned short *to = dst;
  long todo=((size>>1)/0x40*0x22);
  unsigned long decoded=0;

  unsigned long blockCnt=0x40;
  unsigned char *fromLeft=from;
  unsigned char *fromRight=from+0x22;

  unsigned long hiNibbleL = 0;
  unsigned long hiNibbleR = 0;

  while (todo>0) {
    long lValPred, rValPred;
    long lIndex, rIndex;
    long lStep, rStep;
    unsigned long n;
    unsigned char firstNibble;

    lValPred=get16(fromLeft);
    lIndex=lValPred & 0x7f;
    if (lIndex>88) lIndex=88;
    lValPred &= 0xff80;
    if (lValPred & 0x8000) lValPred-=0x10000;
    lStep=stepSizeTable[lIndex];

    rValPred=get16(fromRight);
    rIndex=rValPred & 0x7f;
    if (rIndex>88) rIndex=88;
    rValPred &= 0xff80;
    if (rValPred & 0x8000) rValPred-=0x10000;
    rStep=stepSizeTable[rIndex];

    todo-=2;
    if (todo<=0) return;

    firstNibble=1;
    for(n=0; n<blockCnt; n++) {
      unsigned long loNibbleL;
      unsigned long loNibbleR;

      if (firstNibble) {
        loNibbleL=*fromLeft++;
        hiNibbleL=(loNibbleL>>4) & 0x0f;
        loNibbleL &= 0x0f;
        loNibbleR=*fromRight++;
        hiNibbleR=(loNibbleR>>4) & 0x0f;
        loNibbleR &= 0x0f;
        todo--;
        if (todo<0) return;
        if (downScale)
          n++;
        else
          firstNibble=0;
      } else {
        loNibbleL=hiNibbleL;
        loNibbleR=hiNibbleR;
        firstNibble=1;
      }
      calcDVI(lValPred,loNibbleL,lIndex,lStep);
      *to++=(unsigned short)lValPred;
      calcDVI(rValPred,loNibbleR,rIndex,rStep);
      *to++=(unsigned short)rValPred;
      decoded++;
    }
    fromLeft+=0x22;
    fromRight+=0x22;
  }
}

static int ima4_error = 0;

static double current_time = 0.0;
static int bytes_per_second = 0;
static int bufsize = 0;

int decode_ima4(unsigned char *buf, unsigned long length, double sync_time)
{
  if (ima4_error != 0) {
    return PLUGIN_FAIL;
  }

  if (length > 0) {
    /* Make sure the buffer is big enough */
    if ((length * 2) > bufsize) {
      bufsize = (length * 2);
      free(audio);
      audio = malloc(bufsize);
      if (audio == NULL) {
        ima4_error = 1;
        return PLUGIN_FAIL;
      }
    }

    DecodeIMA4(buf, audio, length);

    if (sync_time > 0.0) {
      current_time = sync_time;
    }

    amp->audio_refresh(audio, length * 2, current_time);

    current_time += (double)(length * 2) / (double)bytes_per_second;
  }

  return PLUGIN_OK;
}

int init_ima4(unsigned long type, unsigned long fourcc)
{
  int bits, channels;

  bits = amp->bits;
  channels = amp->channels;
  bufsize = amp->rate; /* 1 second buffer */

  bufsize *= 2; /* Always 16bit */

  if (channels == 2) {
    bufsize *= 2;
    DecodeIMA4 = DecodeIMA4Stereo;
  } else {
    bufsize *= 1;
    DecodeIMA4 = DecodeIMA4Mono;
  }

  ima4_error = 0;

  audio = malloc(bufsize);
  if (audio == NULL) {
    ima4_error = 1;
    return PLUGIN_FAIL;
  }

  current_time = 0.0;
  bytes_per_second = bufsize;

  if (amp->audio_init(amp->rate, amp->bits, amp->channels) != PLUGIN_OK) {
    ima4_error = 1;
    return PLUGIN_FAIL;
  }

  return PLUGIN_OK;
}

int exit_ima4(void)
{
  if (audio != NULL) {
    free(audio);
    audio = NULL;
  }

  return PLUGIN_OK;
}

VERSION("IMA4 v1.00 (020216)");

IDENT_INIT
IDENT_ADD((TYPE_AUDIO | SUBTYPE_NONE | 0x0002), FOURCC_ima4, init_ima4, decode_ima4, exit_ima4)
IDENT_EXIT

