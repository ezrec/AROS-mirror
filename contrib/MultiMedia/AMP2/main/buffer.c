/*
 *
 * buffer.c
 *
 */

#include <stdio.h>
#include <stdlib.h>

#if !defined(__AROS__)
#include <powerpc/powerpc_protos.h>
#else

#include "aros-inc.h"

#endif

#include "buffer.h"
#include "../common/ampio.h"

/* Buffer size */
#define BUFFERSIZE (512*1024)

/* Buffers */
#define BUFFERS 256

/* 10000 us = 10 ms when reading */
#define READ_WAITTIME 10000

/* 10000 us = 10 ms when writing */
#define WRITE_WAITTIME 10000

/* Buffers */
static struct {
  struct {
    unsigned char *buffer;
    int length;
    int position;
    int file_pos;
    double pts;
  } datas[BUFFERS];

  struct SignalSemaphorePPC *semaphore;

  /* Variables protected by the semaphore -> */
  int write_position;
  int read_position;
  int buffersize;
  int endoffile;
  int play_pos;
  /* <- Variables protected by the semaphore */

  int readbuffer;
  int writebuffer;
  int max_write_position;
  unsigned char *data;
} buffers[BUFFER_TOTAL];

int amp_fopen(int type)
{
  int i;

  buffers[type].data = (unsigned char *)malloc(BUFFERSIZE);
  if (buffers[type].data == NULL) {
    return BUF_FALSE;
  }
  for (i=0; i<BUFFERS; i++) {
    buffers[type].datas[i].buffer = NULL; /* Warning */
    buffers[type].datas[i].length = 0;
    buffers[type].datas[i].position = 0;
    buffers[type].datas[i].pts = -1.0;
  }

  buffers[type].semaphore = (struct SignalSemaphorePPC *)malloc(sizeof(struct SignalSemaphorePPC));
  if (buffers[type].semaphore == NULL) {
    free(buffers[type].data);
    return BUF_FALSE;
  }

  memset(buffers[type].semaphore, 0, sizeof(struct SignalSemaphorePPC));
#if defined(__AROS__)
  InitSemaphorePPC(buffers[type].semaphore);
#else
  if (InitSemaphorePPC(buffers[type].semaphore) != SSPPC_SUCCESS) {
    free(buffers[type].semaphore);
    free(buffers[type].data);
    return BUF_FALSE;
  }
#endif

  buffers[type].write_position = 0;
  buffers[type].read_position = BUFFERSIZE;
  buffers[type].buffersize = 0;
  buffers[type].endoffile = BUF_FALSE;
  buffers[type].readbuffer = 0;
  buffers[type].writebuffer = 0;
  buffers[type].max_write_position = BUFFERSIZE;

  return BUF_TRUE;
}

int amp_fread(int type, unsigned char **buffer, double *pts)
{
  int length = BUF_EOF;

  ObtainSemaphorePPC(buffers[type].semaphore);

  if ((buffers[type].buffersize == 0) && (buffers[type].endoffile == BUF_FALSE)) {
    ReleaseSemaphorePPC(buffers[type].semaphore);
    WaitTime(0, READ_WAITTIME);
    return BUF_FAIL;
  }

  if (buffers[type].buffersize > 0) {
    buffers[type].play_pos = buffers[type].datas[buffers[type].readbuffer].file_pos;

    *buffer = buffers[type].datas[buffers[type].readbuffer].buffer;
    length = buffers[type].datas[buffers[type].readbuffer].length;
    buffers[type].read_position = buffers[type].datas[buffers[type].readbuffer].position;
    *pts = buffers[type].datas[buffers[type].readbuffer].pts;

    buffers[type].readbuffer++;
    if (buffers[type].readbuffer >= BUFFERS) {
      buffers[type].readbuffer = 0;
    }

    buffers[type].buffersize--;
  }

  ReleaseSemaphorePPC(buffers[type].semaphore);

  return length;
}

int amp_fwrite(int type, unsigned char *buffer, int length, double pts, int file_pos)
{
  int max_position, free_size;

  /* Reject zero and negative writes */
  if (length <= 0) {
    return BUF_TRUE;
  }

  ObtainSemaphorePPC(buffers[type].semaphore);

  /* NUMBER_OF_BUFFERS - 1 required since the buffer wont be copied
   * by fread but instead the actual reader will be using the buffer.
   */

  if (buffers[type].buffersize >= (BUFFERS - 1)) {
    if (buffers[type].read_position == buffers[type].max_write_position) {
      /* This can ONLY happend at init */
      buffers[type].read_position = 0;
    }
    ReleaseSemaphorePPC(buffers[type].semaphore);
    WaitTime(0, WRITE_WAITTIME); /* Wait a little */
    return BUF_FALSE;
  }

retry: /* Warning */

  if (buffers[type].write_position <= buffers[type].read_position) {
    max_position = buffers[type].read_position;
  } else {
    max_position = buffers[type].max_write_position;
  }

  free_size = max_position - buffers[type].write_position;

  if (free_size < length) {
    /* Not enough memory at end of buffer? Wrap! */
    if (max_position == buffers[type].max_write_position) {
      buffers[type].write_position = 0;

      if (buffers[type].read_position == buffers[type].max_write_position) {
        /* This can ONLY happend at init */
        buffers[type].read_position = 0;
      }

      goto retry; /* Warning */
    }

    ReleaseSemaphorePPC(buffers[type].semaphore);
    WaitTime(0, WRITE_WAITTIME); /* Wait a little */
    return BUF_FALSE;
  }

  ReleaseSemaphorePPC(buffers[type].semaphore);

  /* Setup pointer and such */
  buffers[type].datas[buffers[type].writebuffer].buffer = &buffers[type].data[buffers[type].write_position];
  buffers[type].datas[buffers[type].writebuffer].position = buffers[type].write_position;

  /* File position */
  buffers[type].datas[buffers[type].writebuffer].file_pos = file_pos;

  if (buffer == NULL) {
    /* Read data to buffer */
    length = ampio_fread(buffers[type].datas[buffers[type].writebuffer].buffer, length);
  } else {
    /* Copy data to buffer */
    memcpy(buffers[type].datas[buffers[type].writebuffer].buffer, buffer, length);
  }

  /* Increase write position */
  buffers[type].write_position += length;

  /* Copy some information */
  buffers[type].datas[buffers[type].writebuffer].length = length;
  buffers[type].datas[buffers[type].writebuffer].pts = pts;

  buffers[type].writebuffer++;
  if (buffers[type].writebuffer >= BUFFERS) {
    buffers[type].writebuffer = 0;
  }

  ObtainSemaphorePPC(buffers[type].semaphore);

  buffers[type].buffersize++;

  ReleaseSemaphorePPC(buffers[type].semaphore);

  return BUF_TRUE;
}

int amp_ftell(int type)
{
  int file_pos = 0;

  ObtainSemaphorePPC(buffers[type].semaphore);

  file_pos = buffers[type].play_pos;

  ReleaseSemaphorePPC(buffers[type].semaphore);

  return file_pos;
}

void amp_fempty(int type)
{
  ObtainSemaphorePPC(buffers[type].semaphore);
  buffers[type].write_position = 0;
  buffers[type].read_position = BUFFERSIZE;
  buffers[type].buffersize = 0;
  buffers[type].readbuffer = 0;
  buffers[type].writebuffer = 0;
  ReleaseSemaphorePPC(buffers[type].semaphore);
}

void amp_fseek(int type)
{
  ObtainSemaphorePPC(buffers[type].semaphore);

  /* Setup pointer and such */
  buffers[type].datas[0].buffer = NULL;
  buffers[type].datas[0].position = 0;
  buffers[type].datas[0].length = 0;
  buffers[type].datas[0].pts = 0.0;

  /* Setup buffer variables */
  buffers[type].write_position = 0;
  buffers[type].read_position = BUFFERSIZE;
  buffers[type].buffersize = 1;
  buffers[type].readbuffer = 0;
  buffers[type].writebuffer = 1;

  /* Sync audio and video buffers */
  while (buffers[type].read_position != 0) {
    ReleaseSemaphorePPC(buffers[type].semaphore);
    WaitTime(0, WRITE_WAITTIME);
    ObtainSemaphorePPC(buffers[type].semaphore);
  }

  /* This will tell fwrite that the buffer is empty */
  buffers[type].read_position = BUFFERSIZE;
  ReleaseSemaphorePPC(buffers[type].semaphore);
}

void amp_feof(int type)
{
  ObtainSemaphorePPC(buffers[type].semaphore);
  buffers[type].endoffile = BUF_TRUE;
  ReleaseSemaphorePPC(buffers[type].semaphore);
}

void amp_fclose(int type)
{
  FreeSemaphorePPC(buffers[type].semaphore);
  free(buffers[type].semaphore);
  free(buffers[type].data);
}
