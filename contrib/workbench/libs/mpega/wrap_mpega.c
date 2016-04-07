#include "wrap_mpega.h"

#include <proto/dos.h>
#include <aros/asmcall.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "audio.h"
#include "stream.h"
#include "frame.h"
#include "synth.h"
#include "resample.h"
#include "filter.h"
#include "timer.h"
#include "tag.h"

#define DEFAULT_BUFFER_SIZE 8192

typedef struct
{
    BPTR fh;
    UBYTE *buffer;
    ULONG streamsize;
    ULONG streamskip;
    ULONG sampleskip;

    MPEGA_CTRL *ctrl;
    MPEGA_ACCESS *access;

    struct mad_stream stream;
    struct mad_frame frame;
    struct mad_synth synth;

    struct tag tag;
    mad_timer_t timer;

    struct audio_dither left_dither;
    struct audio_dither right_dither;

    mad_fixed_t gain;
    mad_fixed_t replaygain;

    struct resample_state resample[2];
    mad_fixed_t resampled[2][1152];
} DecHandle;

#define CALLHOOK(hook, object, message, base)          \
    AROS_UFC4(IPTR, ((struct Hook *) hook)->h_Entry, \
        AROS_UFCA(struct Hook *, hook, A0),          \
    AROS_UFCA(APTR, object, A2),                 \
    AROS_UFCA(APTR, message, A1),                 \
    AROS_UFCA(APTR, base, A4))    


static __inline void SetStreamValues(MPEGA_STREAM *mpega_stream, struct mad_header *header)
{
    switch (header->mode)
    {
        case MAD_MODE_SINGLE_CHANNEL:
            mpega_stream->mode = MPEGA_MODE_MONO;
            break;

        case MAD_MODE_DUAL_CHANNEL:
            mpega_stream->mode = MPEGA_MODE_DUAL;
            break;

        case MAD_MODE_JOINT_STEREO:
            mpega_stream->mode = MPEGA_MODE_J_STEREO;
            break;

        case MAD_MODE_STEREO:
            mpega_stream->mode = MPEGA_MODE_STEREO;
            break;

        default:
            mpega_stream->mode = -1;
            break;
    }

    mpega_stream->norm = ((header->flags & MAD_FLAG_LSF_EXT) ? 2 : 1);
    mpega_stream->layer = header->layer;
    mpega_stream->bitrate = header->bitrate / 1000;
    mpega_stream->frequency = header->samplerate;
    mpega_stream->channels = ((header->mode == MAD_MODE_SINGLE_CHANNEL) ? 1 : 2);

    mpega_stream->private_bit = ((header->private_bits & MAD_PRIVATE_HEADER) ? 1 : 0);
    mpega_stream->copyright = ((header->flags & MAD_FLAG_COPYRIGHT) ? 1 : 0);
    mpega_stream->original = ((header->flags & MAD_FLAG_ORIGINAL) ? 1 : 0);
}

static __inline void ClearStreamValues(MPEGA_STREAM *mpega_stream)
{
    /* Dummy values */
    mpega_stream->mode = MPEGA_MODE_J_STEREO;
    mpega_stream->norm = 1;
    mpega_stream->layer = 3;
    mpega_stream->bitrate = 128;
    mpega_stream->frequency = 44100;
    mpega_stream->channels = 2;

    mpega_stream->private_bit = 0;
    mpega_stream->copyright = 0;
    mpega_stream->original = 0;

    mpega_stream->ms_duration = 1000;

    mpega_stream->dec_channels = 2;
    mpega_stream->dec_quality = 2;
    mpega_stream->dec_frequency = 44100;
}

static __inline void SetStreamOptions(MPEGA_STREAM *mpega_stream, MPEGA_CTRL *ctrl, DecHandle *dechandle)
{
    MPEGA_LAYER *layer = ((mpega_stream->layer == 3) ? (&ctrl->layer_3) : (&ctrl->layer_1_2));
    MPEGA_OUTPUT *output;
    LONG freq;

    if (layer->force_mono || mpega_stream->channels == 1)
    {
        output = &layer->mono;
        mpega_stream->dec_channels = 1;
    } else {
        output = &layer->stereo;
        mpega_stream->dec_channels = 2;
    }

    if (output->quality == 0)
    {
        switch (output->freq_div)
        {
            case 4:
                if ((freq = mpega_stream->frequency / 4) > 8000)
                {
                    mad_stream_options(&dechandle->stream, MAD_OPTION_QUARTSAMPLERATE);
                    mpega_stream->dec_frequency = freq;
                    break;
                }
                /* Fall through */

            case 2:
                if ((freq = mpega_stream->frequency / 2) > 8000)
                {
                    mad_stream_options(&dechandle->stream, MAD_OPTION_HALFSAMPLERATE);
                    mpega_stream->dec_frequency = freq;
                    break;
                }
                /* Fall through */

            case 0:
                if ((freq = mpega_stream->frequency) > output->freq_max)
                {
                    LONG freqdiv = freq / output->freq_max;

                    switch (freqdiv)
                    {
                        case 6:
                        case 5:
                        case 4:
                            freq /= 4;
                            mad_stream_options(&dechandle->stream, MAD_OPTION_QUARTSAMPLERATE);

                            resample_init(&dechandle->resample[0], freq, output->freq_max);
                            if (mpega_stream->dec_channels == 2)
                                resample_init(&dechandle->resample[1], freq, output->freq_max);

                            mpega_stream->dec_frequency = output->freq_max;
                            break;

                        case 3:
                        case 2:
                            freq /= 2;
                            mad_stream_options(&dechandle->stream, MAD_OPTION_HALFSAMPLERATE);

                            resample_init(&dechandle->resample[0], freq, output->freq_max);
                            if (mpega_stream->dec_channels == 2)
                                resample_init(&dechandle->resample[1], freq, output->freq_max);

                            mpega_stream->dec_frequency = output->freq_max;
                            break;

                        default:
                            mad_stream_options(&dechandle->stream, 0);

                            resample_init(&dechandle->resample[0], mpega_stream->frequency, freq);
                            if (mpega_stream->dec_channels == 2)
                                resample_init(&dechandle->resample[1], mpega_stream->frequency, freq);

                            mpega_stream->dec_frequency = freq;
                            break;
                    }

                    break;
                }
                /* Fall through */

            default:
                mad_stream_options(&dechandle->stream, 0);
                mpega_stream->dec_frequency = mpega_stream->frequency;
                break;
        }
    } else {
        switch (output->freq_div)
        {
            case 4:
                if ((freq = mpega_stream->frequency / 4) > 8000)
                {
                    resample_init(&dechandle->resample[0], mpega_stream->frequency, freq);
                    if (mpega_stream->dec_channels == 2)
                        resample_init(&dechandle->resample[1], mpega_stream->frequency, freq);

                    mpega_stream->dec_frequency = freq;
                    break;
                }
                /* Fall through */

            case 2:
                if ((freq = mpega_stream->frequency / 2) > 8000)
                {
                    resample_init(&dechandle->resample[0], mpega_stream->frequency, freq);
                    if (mpega_stream->dec_channels == 2)
                        resample_init(&dechandle->resample[1], mpega_stream->frequency, freq);

                    mpega_stream->dec_frequency = freq;
                    break;
                }
                /* Fall through */

            case 0:
                if ((freq = mpega_stream->frequency) > output->freq_max)
                {
                    resample_init(&dechandle->resample[0], freq, output->freq_max);
                    if (mpega_stream->dec_channels == 2)
                        resample_init(&dechandle->resample[1], freq, output->freq_max);

                    mpega_stream->dec_frequency = output->freq_max;
                    break;
                }
                /* Fall through */

            default:
                mpega_stream->dec_frequency = mpega_stream->frequency;
                break;
        }
    }

    if ((mpega_stream->dec_quality = output->quality) < 2)
    {
        dechandle->stream.options |= MAD_OPTION_IGNORECRC;
        dechandle->gain = MAD_F_ONE;
    }
}

static __inline ULONG CheckID3(UBYTE *buffer)
{
    ULONG size = 0;

    if (buffer[0]=='I' && buffer[1]=='D' && buffer[2]=='3' && buffer[3]<0xFF && buffer[4]<0xFF && buffer[6]<0x80 && buffer[7]<0x80 && buffer[8]<0x80 && buffer[9]<0x80)
    {
        size = 10 + (((buffer[6] & 0x7F) << 21) | ((buffer[7] & 0x7F) << 14) | ((buffer[8] & 0x7F) << 7) | buffer[9] & 0x7F);

        if (buffer[5] & 0x10)
            size += 10;
    }

    return size;
}

static __inline LONG ReadFunc(DecHandle *dechandle, UBYTE *buffer, LONG len, APTR a4base)
{
    MPEGA_CTRL *ctrl = dechandle->ctrl;
    LONG bytes;

    if (ctrl->bs_access == NULL)
    {
        bytes = Read(dechandle->fh, buffer, len);
    } else {
        MPEGA_ACCESS *access = dechandle->access;

        access->func = MPEGA_BSFUNC_READ;
        access->data.read.buffer = buffer;
        access->data.read.num_bytes = len;

        bytes = CALLHOOK(ctrl->bs_access, (APTR)dechandle->fh, access, a4base);
    }

    return bytes;
}

static LONG InitDecoder(MPEGA_STREAM *mpega_stream, MPEGA_CTRL *ctrl, APTR a4base)
{
    DecHandle *dechandle = (DecHandle *) mpega_stream->handle;
    ULONG skip;
    LONG bytes;

    ClearStreamValues(mpega_stream);

    mad_stream_init(&dechandle->stream);
    mad_frame_init(&dechandle->frame);
    mad_synth_init(&dechandle->synth);

    tag_init(&dechandle->tag);
    resample_init(&dechandle->resample[0], 44100, 44100);
    resample_init(&dechandle->resample[1], 44100, 44100);
    mad_timer_reset(&dechandle->timer);
    dechandle->gain = dechandle->replaygain = MAD_F_ONE;

    bytes = ReadFunc(dechandle, dechandle->buffer, ctrl->stream_buffer_size, a4base);

    if (bytes <= 0)
    {
        if (ctrl->check_mpeg) return 0;

        /* Force MAD_ERROR_BUFLEN on first decode */
        bzero(dechandle->buffer, MAD_BUFFER_GUARD);
        mad_stream_buffer(&dechandle->stream, dechandle->buffer, 0);

        return 1;
    }

    dechandle->streamskip = skip = CheckID3(dechandle->buffer);
    if (dechandle->streamsize == 0) dechandle->streamsize = ((bytes > skip) ? (bytes) : (skip));

    /* Ensure that we have something to decode */
    if (skip > (bytes - 2000))
    {
        LONG len = 0, size = bytes;

        if (skip < size)
        {
            bytes = len = size - skip;
            bcopy(dechandle->buffer + (size - len), dechandle->buffer, len);
            skip = size;
        }

        while (skip >= size)
        {
            skip -= size;
            size = ReadFunc(dechandle, dechandle->buffer + len, ctrl->stream_buffer_size - len, a4base);

            if (size <= 0) return 0;
        }

        if (skip)
        {
            bytes = len = size - skip;
            bcopy(dechandle->buffer + (size - len), dechandle->buffer, len);
            size = ReadFunc(dechandle, dechandle->buffer + len, ctrl->stream_buffer_size - len, a4base);
        }

        if (size > 0) bytes += size;
    }

    bzero(dechandle->buffer + bytes, MAD_BUFFER_GUARD);
    mad_stream_buffer(&dechandle->stream, dechandle->buffer, bytes);

    dechandle->stream.sync = 0;
    mad_stream_options(&dechandle->stream, MAD_OPTION_IGNORECRC);

    if (mad_frame_decode(&dechandle->frame, &dechandle->stream) == -1)
    {
        while (MAD_RECOVERABLE(dechandle->stream.error))
        {
            dechandle->stream.error = 0;
            if (mad_frame_decode(&dechandle->frame, &dechandle->stream) == 0) break;
        }

        if (dechandle->stream.error)
        {
            mad_stream_options(&dechandle->stream, 0);
            if (ctrl->check_mpeg || dechandle->streamsize <= (bytes + skip)) return 0;

            return 1;
        }
    }

    mad_stream_options(&dechandle->stream, 0);
    SetStreamValues(mpega_stream, &dechandle->frame.header);

    if (tag_parse(&dechandle->tag, &dechandle->stream) == 0)
    {
        skip = dechandle->stream.next_frame - dechandle->buffer;
        dechandle->streamskip += skip;

        if (dechandle->tag.flags & TAG_LAME)
        {
            struct tag_rgain *rgain = &dechandle->tag.lame.replay_gain[0];

            /*** TODO: Skip Encoder Delay samples when decoding the first frames ***/
            dechandle->sampleskip = dechandle->tag.lame.start_delay;

            if (dechandle->streamsize <= (bytes+skip) && dechandle->tag.lame.music_length > (bytes+skip))
                dechandle->streamsize = dechandle->tag.lame.music_length;

            if (dechandle->tag.lame.replay_gain[1].name == TAG_RGAIN_NAME_AUDIOPHILE && dechandle->tag.lame.replay_gain[1].originator != TAG_RGAIN_ORIGINATOR_UNSPECIFIED)
                rgain = &dechandle->tag.lame.replay_gain[1];

            if ((rgain->name == TAG_RGAIN_NAME_RADIO || rgain->name == TAG_RGAIN_NAME_AUDIOPHILE) && rgain->originator != TAG_RGAIN_ORIGINATOR_UNSPECIFIED)
            {
                double db = (double)(rgain->adjustment + 60) / 10;    /* 6.0 dB pregain */

                if (db > 18.0 || db < -175.0) db = (db > 18.0) ? 18.0 : -175.0;
                dechandle->gain = dechandle->replaygain = mad_f_tofixed(exp(db/20*M_LN10));
            }
        }

        if (dechandle->tag.flags & TAG_VBR && dechandle->tag.xing.flags & TAG_XING_FRAMES)
        {
            ULONG seconds;
            mad_timer_t total = dechandle->frame.header.duration;

            mad_timer_multiply(&total, dechandle->tag.xing.frames);
            mpega_stream->ms_duration = mad_timer_count(total, MAD_UNITS_MILLISECONDS);
            seconds = ((mpega_stream->ms_duration > 1000) ? (mpega_stream->ms_duration / 1000) : (1));

            if (dechandle->tag.xing.flags & TAG_XING_BYTES)
            {
                mpega_stream->bitrate = ((dechandle->tag.xing.bytes / 125) / seconds);
            } else {
                mpega_stream->bitrate = (((dechandle->streamsize - dechandle->streamskip) / 125) / seconds);
            }
        }
        else mpega_stream->ms_duration = ((dechandle->streamsize - dechandle->streamskip) * 8) / ((mpega_stream->bitrate) ? (mpega_stream->bitrate) : (8));
    } else {
        skip = dechandle->stream.this_frame - dechandle->buffer;
        dechandle->streamskip += skip;

        if (dechandle->streamsize)
        {
#if 0
            mad_timer_t total = dechandle->frame.header.duration;

            mad_timer_multiply(&total, ((dechandle->streamsize - dechandle->streamskip) / (dechandle->stream.next_frame - dechandle->stream.this_frame)));
            mpega_stream->ms_duration = mad_timer_count(total, MAD_UNITS_MILLISECONDS);
#else
            /* This will overflow on files over 500MB, but is more accurate than the above */
            mpega_stream->ms_duration = ((dechandle->streamsize - dechandle->streamskip) * 8) / ((mpega_stream->bitrate) ? (mpega_stream->bitrate) : (8));
#endif
        }

        mad_stream_buffer(&dechandle->stream, dechandle->buffer + skip, bytes - skip);
        mad_frame_mute(&dechandle->frame);
    }

    SetStreamOptions(mpega_stream, ctrl, dechandle);

    return 1;
}

static void FinishDecoder(DecHandle *dechandle)
{
    resample_finish(&dechandle->resample[0]);
    resample_finish(&dechandle->resample[1]);
    tag_finish(&dechandle->tag);
    mad_synth_finish(&dechandle->synth);
    mad_frame_finish(&dechandle->frame);
    mad_stream_finish(&dechandle->stream);
}

static __inline LONG ValidateFrequency(LONG freq)
{
    if (freq <= 0) return 48000;
    if (freq <= 8000) return 8000;
    if (freq <= 11025) return 11025;
    if (freq <= 12000) return 12000;
    if (freq <= 16000) return 16000;
    if (freq <= 22050) return 22050;
    if (freq <= 24000) return 24000;
    if (freq <= 32000) return 32000;
    if (freq <= 44100) return 44100;

    return 48000;
}

static __inline void ValidateCtrl(MPEGA_CTRL *ctrl)
{
    if (ctrl->stream_buffer_size <= 0)
        ctrl->stream_buffer_size = DEFAULT_BUFFER_SIZE;

    if (ctrl->layer_1_2.mono.freq_div < 0)
        ctrl->layer_1_2.mono.freq_div = 0;

    if (ctrl->layer_1_2.mono.freq_div > 2)
        ctrl->layer_1_2.mono.freq_div = 4;

    if (ctrl->layer_1_2.mono.quality < 0)
        ctrl->layer_1_2.mono.quality = 0;

    if (ctrl->layer_1_2.mono.quality > 2)
        ctrl->layer_1_2.mono.quality = 2;

    if (ctrl->layer_1_2.stereo.freq_div < 0)
        ctrl->layer_1_2.stereo.freq_div = 0;

    if (ctrl->layer_1_2.stereo.freq_div > 2)
        ctrl->layer_1_2.stereo.freq_div = 4;

    if (ctrl->layer_1_2.stereo.quality < 0)
        ctrl->layer_1_2.stereo.quality = 0;

    if (ctrl->layer_1_2.stereo.quality > 2)
        ctrl->layer_1_2.stereo.quality = 2;

    ctrl->layer_1_2.mono.freq_max = ValidateFrequency(ctrl->layer_1_2.mono.freq_max);
    ctrl->layer_1_2.stereo.freq_max = ValidateFrequency(ctrl->layer_1_2.stereo.freq_max);

    if (ctrl->layer_3.mono.freq_div < 0)
        ctrl->layer_3.mono.freq_div = 0;

    if (ctrl->layer_3.mono.freq_div > 2)
        ctrl->layer_3.mono.freq_div = 4;

    if (ctrl->layer_3.mono.quality < 0)
        ctrl->layer_3.mono.quality = 0;

    if (ctrl->layer_3.mono.quality > 2)
        ctrl->layer_3.mono.quality = 2;

    if (ctrl->layer_3.stereo.freq_div < 0)
        ctrl->layer_3.stereo.freq_div = 0;

    if (ctrl->layer_3.stereo.freq_div > 2)
        ctrl->layer_3.stereo.freq_div = 4;

    if (ctrl->layer_3.stereo.quality < 0)
        ctrl->layer_3.stereo.quality = 0;

    if (ctrl->layer_3.stereo.quality > 2)
        ctrl->layer_3.stereo.quality = 2;

    ctrl->layer_3.mono.freq_max = ValidateFrequency(ctrl->layer_3.mono.freq_max);
    ctrl->layer_3.stereo.freq_max = ValidateFrequency(ctrl->layer_3.stereo.freq_max);
}

static __inline LONG OutputPCM(MPEGA_STREAM *mpega_stream, DecHandle *dechandle, WORD *pcm[MPEGA_MAX_CHANNELS])
{
    WORD *outleft = pcm[0];
    WORD *outright = pcm[1];
    mad_fixed_t const *left = dechandle->synth.pcm.samples[0];
    mad_fixed_t const *right = dechandle->synth.pcm.samples[1];
    ULONG len = dechandle->synth.pcm.length;
    LONG nsamples, sample0, sample1;

    nsamples = len;

    if (mpega_stream->dec_channels == 1 || !outright)
    {
        if (dechandle->resample[0].ratio != MAD_F_ONE)
        {
            nsamples = resample_block(&dechandle->resample[0], len, left, dechandle->resampled[0]);
            left = dechandle->resampled[0];
        }

        if (mpega_stream->dec_quality < 2)
        {
            while (len--)
            {
                sample0 = audio_linear_round(16, *left++);

                *outleft++ = sample0 & 0x0000FFFF;
            }
        } else {
            while (len--)
            {
                sample0 = audio_linear_dither(16, *left++, &dechandle->left_dither);

                *outleft++ = sample0 & 0x0000FFFF;
            }
        }
    } else {    /* Stereo */
        if (dechandle->resample[0].ratio != MAD_F_ONE)
        {
            nsamples = resample_block(&dechandle->resample[0], len, left, dechandle->resampled[0]);
            resample_block(&dechandle->resample[1], len, right, dechandle->resampled[1]);
            left = dechandle->resampled[0];
            right = dechandle->resampled[1];
        }

        if (mpega_stream->dec_quality < 2)
        {
            while (len--)
            {
                sample0 = audio_linear_round(16, *left++);
                sample1 = audio_linear_round(16, *right++);

                *outleft++ = sample0 & 0x0000FFFF;
                *outright++ = sample1 & 0x0000FFFF;
            }
        } else {
            while (len--)
            {
                sample0 = audio_linear_dither(16, *left++, &dechandle->left_dither);
                sample1 = audio_linear_dither(16, *right++, &dechandle->right_dither);

                *outleft++ = sample0 & 0x0000FFFF;
                *outright++ = sample1 & 0x0000FFFF;
            }
        }
    }

    return nsamples;
}


MPEGA_STREAM *WRAP_MPEGA_open(char *stream_name, MPEGA_CTRL *ctrl, APTR a4base)
{
    if (ctrl)
    {
        MPEGA_STREAM *mpega_stream;

        if ((mpega_stream = malloc(sizeof(MPEGA_STREAM))))
        {
            DecHandle *dechandle;

            if ((dechandle = calloc(1, sizeof(DecHandle))))
            {
                mpega_stream->handle = dechandle;

                dechandle->fh = NULL;
                dechandle->access = NULL;

                if ((dechandle->ctrl = malloc(sizeof(MPEGA_CTRL))))
                {
                    bcopy(ctrl, dechandle->ctrl, sizeof(MPEGA_CTRL));
                    ValidateCtrl(dechandle->ctrl);

                    if (ctrl->bs_access == NULL)
                    {
                        if (stream_name)
                        {
                            if ((dechandle->fh = Open(stream_name, MODE_OLDFILE)))
                            {
                                struct FileInfoBlock *fib;

                                dechandle->streamsize = 0;

                                if ((fib = malloc(sizeof(struct FileInfoBlock))))
                                {
                                    if (ExamineFH(dechandle->fh, fib))
                                    {
                                        dechandle->streamsize = fib->fib_Size;
                                    }

                                    free(fib);
                                }

                                if ((dechandle->buffer = malloc(dechandle->ctrl->stream_buffer_size + MAD_BUFFER_GUARD)))
                                {
                                    if (InitDecoder(mpega_stream, dechandle->ctrl, a4base) != 0)
                                    {
                                        return mpega_stream;
                                    }

                                    free(dechandle->buffer);
                                }

                                Close(dechandle->fh);
                            }
                        }
                    } else {
                        MPEGA_ACCESS *access;

                        if ((access = malloc(sizeof(MPEGA_ACCESS))))
                        {
                            dechandle->access = access;

                            access->func = MPEGA_BSFUNC_OPEN;
                            access->data.open.stream_name = stream_name;
                            access->data.open.buffer_size = dechandle->ctrl->stream_buffer_size;
                            access->data.open.stream_size = 0;

                            if ((dechandle->fh = (BPTR)CALLHOOK(ctrl->bs_access, NULL, access, a4base)))
                            {
                                dechandle->streamsize = access->data.open.stream_size;
                                if (access->data.open.buffer_size > 0) dechandle->ctrl->stream_buffer_size = access->data.open.buffer_size;

                                if ((dechandle->buffer = malloc(dechandle->ctrl->stream_buffer_size + MAD_BUFFER_GUARD)))
                                {
                                    if (InitDecoder(mpega_stream, dechandle->ctrl, a4base) != 0)
                                    {
                                        return mpega_stream;
                                    }

                                    free(dechandle->buffer);
                                }

                                access->func = MPEGA_BSFUNC_CLOSE;
                                CALLHOOK(ctrl->bs_access, (APTR)dechandle->fh, access, a4base);
                            }

                            free(access);
                        }
                    }

                    free(dechandle->ctrl);
                }

                free(dechandle);
            }

            free(mpega_stream);
        }
    }

    return NULL;
}

void WRAP_MPEGA_close(MPEGA_STREAM *mpega_stream, APTR a4base)
{
    if (mpega_stream)
    {
        DecHandle *dechandle = (DecHandle *) mpega_stream->handle;

        if (dechandle)
        {
            MPEGA_CTRL *ctrl = dechandle->ctrl;

            FinishDecoder(dechandle);

            if (ctrl->bs_access == NULL)
            {
                if (dechandle->fh) Close(dechandle->fh);
            } else {
                MPEGA_ACCESS *access = dechandle->access;

                access->func = MPEGA_BSFUNC_CLOSE;

                CALLHOOK(ctrl->bs_access, (APTR)dechandle->fh, access, a4base);
            }

            if (dechandle->ctrl) free(dechandle->ctrl);
            if (dechandle->buffer) free(dechandle->buffer);
            if (dechandle->access) free(dechandle->access);

            free(dechandle);
        }

        free(mpega_stream);
    }
}

LONG WRAP_MPEGA_decode_frame(MPEGA_STREAM *mpega_stream, WORD *pcm[MPEGA_MAX_CHANNELS], APTR a4base)
{
    if (mpega_stream && pcm && pcm[0])
    {
        DecHandle *dechandle = (DecHandle *) mpega_stream->handle;
        MPEGA_CTRL *ctrl = dechandle->ctrl;

        if (mad_frame_decode(&dechandle->frame, &dechandle->stream) == -1)
        {
            LONG error = dechandle->stream.error;

            if (error == MAD_ERROR_BUFLEN)
            {
                LONG bytes = ctrl->stream_buffer_size, len = 0;

                if (dechandle->stream.next_frame)
                {
                    bcopy(dechandle->stream.next_frame, dechandle->buffer, len = &dechandle->buffer[bytes] - dechandle->stream.next_frame);
                }

                bytes = ReadFunc(dechandle, dechandle->buffer + len, bytes - len, a4base);

                if (bytes == 0) return MPEGA_ERR_EOF;
                if (bytes < 0) return MPEGA_ERR_BADVALUE;

                bzero(dechandle->buffer + bytes + len, MAD_BUFFER_GUARD);
                mad_stream_buffer(&dechandle->stream, dechandle->buffer, bytes + len);

                if (mad_frame_decode(&dechandle->frame, &dechandle->stream) == -1) error = dechandle->stream.error;
                else error = 0;
            }

            if (error)
            {
                switch (error)
                {
                    case MAD_ERROR_BUFPTR:
                        return MPEGA_ERR_BADVALUE;
                        break;

                    case MAD_ERROR_NOMEM:
                        return MPEGA_ERR_MEM;
                        break;

                    case MAD_ERROR_LOSTSYNC:
                        return MPEGA_ERR_NO_SYNC;
                        break;

                    case MAD_ERROR_BADBIGVALUES:
                    case MAD_ERROR_BADBLOCKTYPE:
                    case MAD_ERROR_BADSCFSI:
                    case MAD_ERROR_BADHUFFDATA:
                        if (mpega_stream->dec_quality > 0) return MPEGA_ERR_BADFRAME;
                        break;

                    case MAD_ERROR_BADLAYER:
                    case MAD_ERROR_BADBITRATE:
                    case MAD_ERROR_BADSAMPLERATE:
                    case MAD_ERROR_BADEMPHASIS:
                    case MAD_ERROR_BADCRC:
                    case MAD_ERROR_BADBITALLOC:
                    case MAD_ERROR_BADSCALEFACTOR:
                    case MAD_ERROR_BADMODE:
                    case MAD_ERROR_BADFRAMELEN:
                    case MAD_ERROR_BADDATAPTR:
                    case MAD_ERROR_BADPART3LEN:
                    case MAD_ERROR_BADHUFFTABLE:
                    case MAD_ERROR_BADSTEREO:
                        return MPEGA_ERR_BADFRAME;
                        break;

                    default:
                        return -error;
                        break;
                }
            }
        }

        mad_timer_add(&dechandle->timer, dechandle->frame.header.duration);

        SetStreamValues(mpega_stream, &dechandle->frame.header);
        SetStreamOptions(mpega_stream, ctrl, dechandle);

        if (dechandle->gain != MAD_F_ONE)
            gain_filter(dechandle->gain, &dechandle->frame);

        mad_synth_frame(&dechandle->synth, &dechandle->frame);

        return OutputPCM(mpega_stream, dechandle, pcm);
    }

    return MPEGA_ERR_BADVALUE;
}

LONG WRAP_MPEGA_seek(MPEGA_STREAM *mpega_stream, ULONG ms_time_position, APTR a4base)
{
    if (mpega_stream)
    {
        DecHandle *dechandle = (DecHandle *) mpega_stream->handle;
        MPEGA_CTRL *ctrl = dechandle->ctrl;
        LONG pos, error;

        if (dechandle->tag.flags & TAG_VBR && dechandle->tag.xing.flags & TAG_XING_TOC)
        {
            int step, i = 99;

            if (dechandle->tag.xing.flags & TAG_XING_BYTES)
            {
                step = dechandle->tag.xing.bytes / 256;
            } else {
                step = (dechandle->streamsize - dechandle->streamskip) / 256;
            }

            if (ms_time_position < mpega_stream->ms_duration)
            {
                i = ((mpega_stream->ms_duration > 100) ? (ms_time_position / (mpega_stream->ms_duration / 100)) : (0));
                if (i > 99) i = 99;
            }

            pos = (step * dechandle->tag.xing.toc[i]) + dechandle->streamskip;
        } else {
            pos = ((((ULONG)mpega_stream->bitrate * 125UL) * (ms_time_position / 1000)) + dechandle->streamskip);
        }

        if (ctrl->bs_access == NULL)
        {
            error = Seek(dechandle->fh, pos, OFFSET_BEGINNING);

            if (error > 0) error = 0;
        } else {
            MPEGA_ACCESS *access = dechandle->access;

            access->func = MPEGA_BSFUNC_SEEK;
            access->data.seek.abs_byte_seek_pos = pos;

            error = CALLHOOK(ctrl->bs_access, (APTR)dechandle->fh, access, a4base);
        }

        if (error == 0)
        {
            LONG bytes;

            bytes = ReadFunc(dechandle, dechandle->buffer, ctrl->stream_buffer_size, a4base);

            if (bytes == 0) return MPEGA_ERR_EOF;
            if (bytes < 0) return MPEGA_ERR_BADVALUE;

            bzero(dechandle->buffer + bytes, MAD_BUFFER_GUARD);
            mad_stream_buffer(&dechandle->stream, dechandle->buffer, bytes);

            mad_frame_mute(&dechandle->frame);
            mad_synth_mute(&dechandle->synth);
            dechandle->stream.sync = 0;

            /* If we seek somewhere other than the beginning we need to flush out junk audio */
            if (pos > dechandle->streamskip)
            {
                mad_frame_decode(&dechandle->frame, &dechandle->stream);
                mad_frame_decode(&dechandle->frame, &dechandle->stream);
                mad_synth_frame(&dechandle->synth, &dechandle->frame);
            }

//            mad_timer_set(&dechandle->timer, 0, pos - dechandle->streamskip, (ULONG)mpega_stream->bitrate * 125);
            mad_timer_set(&dechandle->timer, 0, ms_time_position, 1000);

            return 0;
        }
    }

    return MPEGA_ERR_BADVALUE;
}

LONG WRAP_MPEGA_time(MPEGA_STREAM *mpega_stream, ULONG *ms_time_position)
{
    if (mpega_stream && ms_time_position)
    {
        DecHandle *dechandle = (DecHandle *) mpega_stream->handle;

        *ms_time_position = mad_timer_count(dechandle->timer, MAD_UNITS_MILLISECONDS);
        return 0;
    }

    return MPEGA_ERR_BADVALUE;
}

LONG WRAP_MPEGA_find_sync(UBYTE *buffer, LONG buffer_size)
{
    if (buffer && buffer_size > 75)
    {
        LONG i = 0;

        while (i < buffer_size - 1)
        {
            if ((buffer[i] == 0xFF) && ((buffer[i+1] & 0xE0) == 0xE0)) return i;
            ++i;
        }

        return MPEGA_ERR_NO_SYNC;
    }

    return MPEGA_ERR_BADVALUE;
}

LONG WRAP_MPEGA_scale(MPEGA_STREAM *mpega_stream, LONG scale_percent)
{
    if (mpega_stream)
    {
        DecHandle *dechandle = (DecHandle *) mpega_stream->handle;

        if (scale_percent <= 0) dechandle->gain = ((mad_fixed_t)0x00000001L);
        else
        {
            mad_fixed_t scaled;

            if (scale_percent > 800) scale_percent = 800;
            scaled = ((dechandle->replaygain/100)*scale_percent)+1;

            if (scaled > MAD_F_MAX || scaled < ((mad_fixed_t)0x00000001L)) dechandle->gain = MAD_F_MAX;
            else dechandle->gain = scaled;
        }

        return 0;
    }

    return MPEGA_ERR_BADVALUE;
}

