/*
 * mad - MPEG audio decoder
 * Copyright (C) 2000-2001 Robert Leslie
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id$
 *
 * This is an example of a command line MPEG audio player, based on minimad.c
 * and libmad. It uses oss.library for audio output.
 * See http://www.underbit.com/products/mad/ for original MAD sources.
 * Have a look at doc/PATENTS.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <dos/dos.h>
#include <proto/oss.h>
#include <proto/exec.h>
#include <proto/dos.h>

#define DSP_DRIVER_NAME "/dev/dsp"
#define SRCBUFSIZE 456780
#define DESTBUFSIZE 8900

#include "mad.h"

static struct Library *OSSBase;

/* private message buffer */
struct buffer {
    BPTR infh;
    void *srcbuffer;
    short *destbuffer;
};

static void cleanup(struct buffer *buffer);
static enum mad_flow input(void *data, struct mad_stream *stream);
static inline signed int scale(mad_fixed_t sample);
static enum mad_flow output(void *data, struct mad_header const *header, struct mad_pcm *pcm);
static enum mad_flow error(void *data, struct mad_stream *stream, struct mad_frame *frame);

int main (int argc, char *argv[])
{
    struct buffer buffer;
    struct mad_decoder decoder;
    int ok, channels, rate;

    OSSBase = NULL;
    buffer.srcbuffer = NULL;
    buffer.destbuffer = NULL;
    channels = 2;
    rate = 44100;
    if (argc != 2)
    {
	printf ("Usage: madoss <filename>\n");
	return -1;
    }

    buffer.srcbuffer = AllocVec(SRCBUFSIZE, MEMF_ANY);
    if (!buffer.srcbuffer)
    {
	printf ("allocvec src error\n");
	cleanup(&buffer);
	return -1;
    }

    buffer.destbuffer = AllocVec(DESTBUFSIZE, MEMF_ANY);
    if (!buffer.destbuffer)
    {
	printf ("allocvec dest error\n");
	cleanup(&buffer);
	return -1;
    }

    buffer.infh = Open(argv[1], MODE_OLDFILE);
    if (buffer.infh == 0)
    {
	printf ("error %ld opening file\n", IoErr());
	cleanup(&buffer);
	return -1;
    }

    OSSBase = OpenLibrary("oss.library", 0);
    if (!OSSBase)
    {
	printf("Could not open oss.library\n");
	cleanup(&buffer);
	return -1;
    }

    ok = OSS_Open(DSP_DRIVER_NAME, FALSE, TRUE, TRUE);
    if( !ok )
    {
	printf ("error opening DSP\n");
	cleanup(&buffer);
	return -1;
    }

    ok = OSS_SetFormat_S16LE();
    if( !ok )
    {
	printf("error setting format\n");
	cleanup(&buffer);
	return -1;
    }

    ok = OSS_SetNumChannels(channels);
    if( !ok )
    {
	printf("error setting channels\n");
	cleanup(&buffer);
	return -1;
    }

    ok = OSS_SetWriteRate(rate, &rate);
    if( !ok )
    {
	printf("error setting write rate\n");
	cleanup(&buffer);
	return -1;
    }

    /* configure input, output, and error functions */
    mad_decoder_init(&decoder, &buffer,
		     input, 0 /* header */, 0 /* filter */, output,
		     error, 0 /* message */);

    mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);

    mad_decoder_finish(&decoder);

    cleanup(&buffer);
    return 0;
}

static void cleanup(struct buffer *buffer)
{
    if(OSSBase)
    {
	OSS_Close();
	CloseLibrary(OSSBase);
    }
    if(buffer->infh) Close(buffer->infh);
    if(buffer->srcbuffer) FreeVec(buffer->srcbuffer);
    if(buffer->destbuffer) FreeVec(buffer->destbuffer);
}

/* input function: called when more input is needed; refill stream buffer */
static enum mad_flow input(void *data, struct mad_stream *stream)
{
    struct buffer *buffer = data;
    int readlength;

    printf("buffer %x bufend %x this %x next %x skiplen %x\n", stream->buffer, stream->bufend, stream->this_frame, stream->next_frame, stream->skiplen);
    readlength = Read(buffer->infh, buffer->srcbuffer, SRCBUFSIZE);
    printf("read %d\n", readlength);
    if ( readlength<0 )
    {
	printf("error reading file\n");
	return MAD_FLOW_STOP;
    }
    if ( readlength==0 )
	return MAD_FLOW_STOP;
    mad_stream_buffer(stream, buffer->srcbuffer, readlength);

    return MAD_FLOW_CONTINUE;
}

/* utility to scale and round samples to 16 bits */
static inline signed int scale(mad_fixed_t sample)
{
    /* round */
    sample += (1L << (MAD_F_FRACBITS - 16));
    
    /* clip */
    if (sample >= MAD_F_ONE)
	sample = MAD_F_ONE - 1;
    else if (sample < -MAD_F_ONE)
	sample = -MAD_F_ONE;
    
    /* quantize */
    return sample >> (MAD_F_FRACBITS + 1 - 16);
}

/* output function: called to process output */
static enum mad_flow output(void *data, struct mad_header const *header, struct mad_pcm *pcm)
{
    struct buffer *buffer = data;
    unsigned int nchannels, nsamples;
    mad_fixed_t const *left_ch, *right_ch;
    signed short *ptr;
    signed int sample;
    int written, length;
    
    /* pcm->samplerate contains the sampling frequency */
    
    nchannels = pcm->channels;
    nsamples  = pcm->length;
    left_ch   = pcm->samples[0];
    right_ch  = pcm->samples[1];
    //printf ("nchannels %d nsamples %d left_ch %x right_ch %x\n",nchannels,nsamples,left_ch,right_ch);
    
    length = nsamples*2;
    if (nchannels == 2)
	length *= 2;
    if( length > DESTBUFSIZE )
    {
	printf("buffer overflow\n");
	return MAD_FLOW_STOP;
    }
    
    ptr = buffer->destbuffer;
    while (nsamples--)
    {
	/* output sample(s) in 16-bit signed little-endian PCM */
	sample = scale(*left_ch++);
	*ptr++ = sample;
	if (nchannels == 2)
	{
	    sample = scale(*right_ch++);
	    *ptr++ = sample;
	}
    }

    ptr = buffer->destbuffer;
    do
    {
	written = OSS_Write(ptr, length);
	//printf("written %d\n", written);
	if( written < 0 )
	{
	    printf ("error writing audio\n");
	    return MAD_FLOW_STOP;
	}

	ptr += written;
	length -= written;
    } while( length > 0 );
    
    return MAD_FLOW_CONTINUE;
}

/* error function: called to handle a decoding error */
static enum mad_flow error(void *data, struct mad_stream *stream, struct mad_frame *frame)
{
    struct buffer *buffer = data;

    printf("decoding error 0x%04x at byte 0x%08x\n",
	   stream->error, /*mad_stream_errorstr(stream),*/ stream->this_frame - stream->buffer);

    if( MAD_RECOVERABLE(stream->error) )
	return MAD_FLOW_CONTINUE;
    else
	return MAD_FLOW_STOP;
}
