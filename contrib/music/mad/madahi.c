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
 * This is based on:
 * minimad.c (Simple MAD decoder) v1.13   (C) 2000-2001 Robert Leslie
 * madlld.c (MAD low-level demonstration/decoder) v1.0p1   (C) 2001, 2002 Bertrand Petit
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <aros/debug.h>

#include <dos/dos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <devices/ahi.h>
#include <proto/ahi.h>

#define SRCBUFSIZE 60000
#define DESTBUFSIZE 8900

#include "mad.h"

static struct MsgPort *AHImp;
static struct AHIRequest *AHIios[2];
static struct AHIRequest *AHIio, *link;
static APTR               AHIiocopy = NULL;
static BYTE               AHIDevice = -1;
static short *destbuffer;

/* private message buffer */
struct buffer {
    BPTR infh;
    void *srcbuffer;
    short *destbuffer1;
    short *destbuffer2;
    int framecount;
};

static void cleanup(struct buffer *buffer);
static enum mad_flow input(void *data, struct mad_stream *stream);
static inline signed int scale(mad_fixed_t sample);
static enum mad_flow output(void *data, struct mad_header const *header, struct mad_pcm *pcm);
static enum mad_flow error(void *data, struct mad_stream *stream, struct mad_frame *frame);
static const char *MadErrorString(const struct mad_stream *Stream);
static void PrintFrameInfo(struct mad_header *Header);

int main (int argc, char *argv[])
{
    struct buffer buffer;
    struct mad_decoder decoder;

    buffer.infh = NULL;
    buffer.srcbuffer = NULL;
    buffer.destbuffer1 = NULL;
    buffer.destbuffer2 = NULL;
    buffer.framecount = 0;
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

    destbuffer = AllocVec(DESTBUFSIZE * 2, MEMF_ANY);
    if (!destbuffer)
    {
	printf ("allocvec dest error\n");
	cleanup(&buffer);
	return -1;
    }
    buffer.destbuffer1 = destbuffer;
    buffer.destbuffer2 = (short *)((UBYTE *)buffer.destbuffer1 + DESTBUFSIZE);
    
    buffer.infh = Open(argv[1], MODE_OLDFILE);
    if (buffer.infh == 0)
    {
	printf ("error %ld opening file\n", IoErr());
	cleanup(&buffer);
	return -1;
    }
    
    AHImp = CreateMsgPort();
    if (!AHImp) 
    {
	printf("Could not create msgport\n");
	cleanup(&buffer);
	return -1;
    }

    if (!(AHIio = (struct AHIRequest *)CreateIORequest(AHImp, sizeof(struct AHIRequest))))
    {
	printf("Could not create AHI request\n");
    	cleanup(&buffer);
	return -1;
    }

    AHIio->ahir_Version = 4;
    AHIDevice = (BYTE)OpenDevice(AHINAME, 0, (struct IORequest *)AHIio, 0);
    if (AHIDevice)
    {
    	printf("Unable to open ahi.device\n");
	cleanup(&buffer);
	return -1;
    }
    
    AHIiocopy = AllocMem(sizeof(struct AHIRequest), MEMF_ANY);
    if (!AHIiocopy)
    {
    	printf("Failed to make copy of AHI Request\n");
    	cleanup(&buffer);
	return -1;
    }
    CopyMem(AHIio, AHIiocopy, sizeof(struct AHIRequest));
    AHIios[0] = AHIio;
    AHIios[1] = AHIiocopy;
    
    /* configure input, output, and error functions */
    mad_decoder_init(&decoder, &buffer,
		     input, 0 /* header */, 0 /* filter */, output,
		     error, 0 /* message */);

    mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);

    mad_decoder_finish(&decoder);

    printf("%d frames decoded.\n", buffer.framecount);
    cleanup(&buffer);
    return 0;
}

static void cleanup(struct buffer *buffer)
{
    if (AHIios[0])
    {
    	if (!CheckIO((struct IORequest *)AHIios[0])) AbortIO((struct IORequest *)AHIios[0]);
	WaitIO((struct IORequest *)AHIios[0]);
    }

    if (AHIios[1])
    {
    	if (!CheckIO((struct IORequest *)AHIios[1])) AbortIO((struct IORequest *)AHIios[1]);
	WaitIO((struct IORequest *)AHIios[1]);
    }
    
    if (!AHIDevice)
    	CloseDevice((struct IORequest *)AHIio);
	
    DeleteIORequest((struct IORequest *)AHIio);
    if (AHIiocopy) FreeMem(AHIiocopy, sizeof(struct AHIRequest));
    DeleteMsgPort(AHImp);
    
    if(buffer->infh) Close(buffer->infh);
    if(buffer->srcbuffer) FreeVec(buffer->srcbuffer);
    if(destbuffer) FreeVec(destbuffer);
}

/* input function: called when more input is needed; refill stream buffer */
static enum mad_flow input(void *data, struct mad_stream *stream)
{
    struct buffer *buffer = data;
    int readlength, remainingbytes;
    void * srcstart;

    //printf("1 buffer %x bufend %x thispos %d next %x buflen %d\n", stream->buffer, stream->bufend, stream->this_frame - stream->buffer, stream->next_frame, stream->bufend - stream->buffer);
    if( stream->next_frame != NULL )
    {
	remainingbytes = stream->bufend - stream->next_frame;
	memcpy( (void*)stream->buffer, stream->this_frame, remainingbytes );
    }
    else remainingbytes = 0;

    srcstart = buffer->srcbuffer + remainingbytes;
    readlength = Read(buffer->infh, srcstart, SRCBUFSIZE - remainingbytes);
    printf("read %d remainingbytes %d srcstart %x\n", readlength, remainingbytes, (int)srcstart);
    if( readlength<0 )
    {
	printf("error reading file\n");
	return MAD_FLOW_STOP;
    }
    if( readlength==0 )
	return MAD_FLOW_STOP;
    mad_stream_buffer(stream, buffer->srcbuffer, readlength + remainingbytes);
    //printf("2 buffer %x bufend %x thispos %d next %x buflen %d\n", stream->buffer, stream->bufend, stream->this_frame - stream->buffer, stream->next_frame, stream->bufend - stream->buffer);

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
    int length;
    
    /* pcm->samplerate contains the sampling frequency */
    nchannels = pcm->channels;
    nsamples  = pcm->length;
    left_ch   = pcm->samples[0];
    right_ch  = pcm->samples[1];
    //printf ("nchannels %d nsamples %d left_ch %x right_ch %x\n",nchannels,nsamples,left_ch,right_ch);

    if(buffer->framecount==0)
    {
	PrintFrameInfo((struct mad_header *)header);
    }
    buffer->framecount++;

    length = nsamples*2;
    if( nchannels == 2 )
	length *= 2;
    if( length > DESTBUFSIZE )
    {
	printf("buffer overflow\n");
	return MAD_FLOW_STOP;
    }
    
    ptr = buffer->destbuffer1;
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

    ptr = buffer->destbuffer1;
    AHIios[0]->ahir_Std.io_Message.mn_Node.ln_Pri = 0;
    AHIios[0]->ahir_Std.io_Command  = CMD_WRITE;
    AHIios[0]->ahir_Std.io_Data     = ptr;
    AHIios[0]->ahir_Std.io_Length   = length;
    AHIios[0]->ahir_Std.io_Offset   = 0;
    AHIios[0]->ahir_Frequency       = pcm->samplerate;
    AHIios[0]->ahir_Type            = AHIST_S16S;
    AHIios[0]->ahir_Volume          = 0x10000;          // Full volume
    AHIios[0]->ahir_Position        = 0x8000;           // Centered
    AHIios[0]->ahir_Link            = link;
    SendIO((struct IORequest *)AHIios[0]);
    if (link)
    {
    	ULONG signals = Wait(SIGBREAKF_CTRL_C | (1L << AHImp->mp_SigBit));
	
	if (signals & SIGBREAKF_CTRL_C)
	{
	    return MAD_FLOW_STOP;
	}
	
	if (WaitIO((struct IORequest *)link))
	{
	    printf("WaitIO!?\n");
	}
	
    }
    link = AHIios[0];
    
    /* swap destbuffer1, destbuffer2 */
    {
    	short *tmp = buffer->destbuffer1;
	buffer->destbuffer1 = buffer->destbuffer2;
	buffer->destbuffer2 = tmp;
    }
    
    /* swap AHI requests */
    
    {
    	struct AHIRequest *tmp = AHIios[0];
	AHIios[0] = AHIios[1];
	AHIios[1] = tmp;
    }
    
    return MAD_FLOW_CONTINUE;
}

/* error function: called to handle a decoding error */
static enum mad_flow error(void *data, struct mad_stream *stream, struct mad_frame *frame)
{
    //struct buffer *buffer = data;

    printf("%srecoverable decoding error 0x%04x at byte 0x%08x (%s)\n", MAD_RECOVERABLE(stream->error) ? "" : "un",
	   stream->error, stream->this_frame - stream->buffer, MadErrorString(stream));

    if( MAD_RECOVERABLE(stream->error) )
	return MAD_FLOW_CONTINUE;
    else
	return MAD_FLOW_STOP;
}

#if (MAD_VERSION_MAJOR>=1) || \
    ((MAD_VERSION_MAJOR==0) && \
     (((MAD_VERSION_MINOR==14) && \
       (MAD_VERSION_PATCH>=2)) || \
      (MAD_VERSION_MINOR>14)))
#define MadErrorString(x) mad_stream_errorstr(x)
#else
static const char *MadErrorString(const struct mad_stream *Stream)
{
	switch(Stream->error)
	{
		/* Generic unrecoverable errors. */
		case MAD_ERROR_BUFLEN:
			return("input buffer too small (or EOF)");
		case MAD_ERROR_BUFPTR:
			return("invalid (null) buffer pointer");
		case MAD_ERROR_NOMEM:
			return("not enough memory");

		/* Frame header related unrecoverable errors. */
		case MAD_ERROR_LOSTSYNC:
			return("lost synchronization");
		case MAD_ERROR_BADLAYER:
			return("reserved header layer value");
		case MAD_ERROR_BADBITRATE:
			return("forbidden bitrate value");
		case MAD_ERROR_BADSAMPLERATE:
			return("reserved sample frequency value");
		case MAD_ERROR_BADEMPHASIS:
			return("reserved emphasis value");

		/* Recoverable errors */
		case MAD_ERROR_BADCRC:
			return("CRC check failed");
		case MAD_ERROR_BADBITALLOC:
			return("forbidden bit allocation value");
		case MAD_ERROR_BADSCALEFACTOR:
			return("bad scalefactor index");
		case MAD_ERROR_BADFRAMELEN:
			return("bad frame length");
		case MAD_ERROR_BADBIGVALUES:
			return("bad big_values count");
		case MAD_ERROR_BADBLOCKTYPE:
			return("reserved block_type");
		case MAD_ERROR_BADSCFSI:
			return("bad scalefactor selection info");
		case MAD_ERROR_BADDATAPTR:
			return("bad main_data_begin pointer");
		case MAD_ERROR_BADPART3LEN:
			return("bad audio data length");
		case MAD_ERROR_BADHUFFTABLE:
			return("bad Huffman table select");
		case MAD_ERROR_BADHUFFDATA:
			return("Huffman data overrun");
		case MAD_ERROR_BADSTEREO:
			return("incompatible block_type for JS");

		/* Unknown error. This swich may be out of sync with libmad's
		 * defined error codes.
		 */
		default:
			return("Unknown error code");
	}
}
#endif

static void PrintFrameInfo(struct mad_header *Header)
{
	const char	*Layer,
				*Mode,
				*Emphasis;

	/* Convert the layer number to it's printed representation. */
	switch(Header->layer)
	{
		case MAD_LAYER_I:
			Layer="I";
			break;
		case MAD_LAYER_II:
			Layer="II";
			break;
		case MAD_LAYER_III:
			Layer="III";
			break;
		default:
			Layer="(unexpected layer value)";
			break;
	}

	/* Convert the audio mode to it's printed representation. */
	switch(Header->mode)
	{
		case MAD_MODE_SINGLE_CHANNEL:
			Mode="single channel";
			break;
		case MAD_MODE_DUAL_CHANNEL:
			Mode="dual channel";
			break;
		case MAD_MODE_JOINT_STEREO:
			Mode="joint (MS/intensity) stereo";
			break;
		case MAD_MODE_STEREO:
			Mode="normal LR stereo";
			break;
		default:
			Mode="(unexpected mode value)";
			break;
	}

	/* Convert the emphasis to it's printed representation. */
	switch(Header->emphasis)
	{
		case MAD_EMPHASIS_NONE:
			Emphasis="no";
			break;
		case MAD_EMPHASIS_50_15_US:
			Emphasis="50/15 us";
			break;
		case MAD_EMPHASIS_CCITT_J_17:
			Emphasis="CCITT J.17";
			break;
		default:
			Emphasis="(unexpected emphasis value)";
			break;
	}

	printf("%lu kb/s audio mpeg layer %s stream %s crc, "
			"%s with %s emphasis at %d Hz sample rate\n",
			Header->bitrate,Layer,
			Header->flags&MAD_FLAG_PROTECTION?"with":"without",
			Mode,Emphasis,Header->samplerate);
}
