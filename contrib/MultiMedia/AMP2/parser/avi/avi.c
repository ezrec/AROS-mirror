/*
 *
 * avi.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../common/core.h"
#include "../../common/ampio.h"
#include "../../main/main.h"
#include "../../main/buffer.h"

typedef struct {
  unsigned long id;
  /* Video */
  int stream;
  int width;
  int height;
  int depth;
  double framerate;
} video_info;

typedef struct {
  unsigned long id;
  /* Audio */
  int stream;
  int rate;
  int bits;
  int channels;
} audio_info;

static video_info vi;
static audio_info ai;

static unsigned char avi_pal[256 * 4];
static unsigned long avi_palette[256];

typedef struct {
  double timestamp;
  long position;
  long length;
  int type;
  int is_keyframe;
} avi_node;

static avi_node *avi_index;

#define AVI_VIDEO BUFFER_VIDEO
#define AVI_AUDIO BUFFER_AUDIO

/***************************************/

#define MKTAG(a, b, c, d) FOURCC(d, c, b, a)

#undef DEBUG
//#define DEBUG

typedef struct AVIIndex {
    unsigned char tag[4];
    unsigned long flags, pos, len;
} AVIIndex;

typedef signed long long INT64;
typedef INT64 offset_t;
typedef unsigned long UINT32;

typedef struct {
    INT64 movi_start;
    INT64 movi_end;
//    unsigned long movi_start, movi_end;
    offset_t movi_list;
    AVIIndex *first, *last;

    int index_entries;
    unsigned char *index;
} AVIContext;

#ifdef DEBUG
void print_tag(const char *str, unsigned int tag, int size)
{
    printf("%s: tag=%c%c%c%c size=0x%x\n",
           str, tag & 0xff,
           (tag >> 8) & 0xff,
           (tag >> 16) & 0xff,
           (tag >> 24) & 0xff,
           size);
}
#endif

static AVIContext *avi = NULL; /* FIXME */

/* FIXME: place in context struct */
static long dwScale = 0, dwRate = 0;
static long dwScale_audio = 0, dwRate_audio = 0;
static long dwSampleSize = 0;

static int avi_read_header(void) //AVFormatContext *s, AVFormatParameters *ap)
{
//    ByteIOContext *pb = &s->pb;
    UINT32 tag, tag1;
    int codec_type, stream_index, size, frame_period, bit_rate;
    int bps;
//    AVStream *st;

//long dwScale = 0, dwRate = 0;
//long dwScale_audio = 0, dwRate_audio = 0;
//long dwSampleSize = 0;

/**/

int colors = 0;

unsigned long nb_streams = 0;

#define CODEC_TYPE_VIDEO 0
#define CODEC_TYPE_AUDIO 1

/**/

    avi = malloc(sizeof(AVIContext));
    if (!avi)
        return -1;
    memset(avi, 0, sizeof(AVIContext));
//    s->priv_data = avi;

    /* check RIFF header */
    tag = get_le32();

    if (tag != MKTAG('R', 'I', 'F', 'F'))
        return -1;
    get_le32(); /* file size */
    tag = get_le32();
    if (tag != MKTAG('A', 'V', 'I', ' '))
        return -1;
    
    /* first list tag */
    stream_index = -1;
    codec_type = -1;
    frame_period = 0;
    for(;;) {
        if (ampio_feof()) break;
/*
        if (ampio_feof())
            goto fail;
*/
        tag = get_le32();
        size = get_le32();
#ifdef DEBUG
        print_tag("tag", tag, size);
#endif

        switch(tag) {
        case MKTAG('L', 'I', 'S', 'T'):
            /* ignored, except when start of video packets */
            tag1 = get_le32();
            size -= 4;
#ifdef DEBUG
            print_tag("list", tag1, size);
#endif
            switch (tag1) {
            case MKTAG('m', 'o', 'v', 'i'):
                avi->movi_start = ampio_ftell();
                avi->movi_end = avi->movi_start + size;
#ifdef DEBUG
                printf("movi start=%Lx, movi end=%Lx\n", avi->movi_start, avi->movi_end);
#endif
                if (avi->movi_end >= ampio_length()) {
                  goto end_of_header;
                }
                ampio_fseek(avi->movi_end, SEEK_SET);
                //goto end_of_header;
                break;
            case MKTAG('h', 'd', 'r', 'l'):
            case MKTAG('s', 't', 'r', 'l'):
                break;
            default:
                ampio_fseek(size, SEEK_CUR);
                break;
            }
            break;
        case MKTAG('a', 'v', 'i', 'h'):
            /* avi header */
            frame_period = get_le32();
            bit_rate = get_le32() * 8;
            ampio_fskip(4 * 4);
//            s->nb_streams = get_le32();
            nb_streams = get_le32();
#if 0
            for(i=0;i<s->nb_streams;i++) {
                AVStream *st;
                st = malloc(sizeof(AVStream));
                if (!st)
                    goto fail;
                memset(st, 0, sizeof(AVStream));
                s->streams[i] = st;
            }
#endif
            ampio_fskip(size - 7 * 4);
            break;
        case MKTAG('s', 't', 'r', 'h'):
            /* stream header */
            stream_index++;
            tag1 = get_le32();
            switch(tag1) {
            case MKTAG('v', 'i', 'd', 's'):
                codec_type = CODEC_TYPE_VIDEO;
                get_le32(); /* codec tag */
                get_le32(); /* flags */
                get_le16(); /* priority */
                get_le16(); /* language */
                get_le32(); /* XXX: initial frame ? */
                dwScale = get_le32(); /* scale */
                dwRate = get_le32(); /* rate */
                ampio_fskip(size - 7 * 4);
                break;
            case MKTAG('a', 'u', 'd', 's'):
                codec_type = CODEC_TYPE_AUDIO;
                get_le32(); /* ??? */
                get_le32(); /* ??? */
                get_le32(); /* ??? */
                get_le32(); /* ??? */
                dwScale_audio = get_le32(); /* scale */
                dwRate_audio = get_le32(); /* rate */
                get_le32(); /* ??? */
                get_le32(); /* ??? */
                get_le32(); /* ??? */
                get_le32(); /* ??? */
                dwSampleSize = get_le32(); /* scale */
                ampio_fskip(size - 12 * 4);
                break;
            default:
                goto fail;
            }
            break;
/* MARO */

        case MKTAG('i', 'd', 'x', '1'):
        case MKTAG('i', 'd', 'd', 'x'):
            size += (size & 1);
            avi->index = malloc(size);
            memset(avi->index, 0, size);
            ampio_fread(avi->index, size);
#ifdef DEBUG
            printf("index: %d (%d) -> %d (%d)\n", size, (size & ~0x0f), size/16, (size & ~0x0f)/16);
#endif
            avi->index_entries = (size & ~0x0f)/16;
            avi->first = (AVIIndex *)avi->index;
            avi->last = (AVIIndex *)(avi->index + (size & ~0x0f));
            break;

/* MARO */

        case MKTAG('s', 't', 'r', 'f'):
            /* stream header */
//            if (stream_index >= s->nb_streams) {
            if (stream_index >= nb_streams) {
                ampio_fskip(size);
            } else {
//                st = s->streams[stream_index];
                switch(codec_type) {
                case CODEC_TYPE_VIDEO:
                    get_le32(); /* size */
//                    st->codec.width = get_le32();
//                    st->codec.height = get_le32();
//                    if (frame_period)
//                        st->codec.frame_rate = (INT64_C(1000000) * FRAME_RATE_BASE) / frame_period;
//                    else
//                        st->codec.frame_rate = 25 * FRAME_RATE_BASE;

                    vi.width = get_le32();
                    vi.height = get_le32();
                    if (frame_period)
                        vi.framerate = (double)1000000 / (double)frame_period;
                    else
                        vi.framerate = (double)25;
                    vi.stream = stream_index;

                    get_le16(); /* panes */
                    vi.depth = get_le16(); /* depth */
                    tag1 = get_be32(); // AmiDog get_le32();
                    vi.id = tag1; /* FIXME */
#ifdef DEBUG
                    print_tag("video", tag1, 0);
#endif
//                    st->codec.codec_type = CODEC_TYPE_VIDEO;
//                    st->codec.codec_tag = tag1;
//                    st->codec.codec_id = codec_get_id(codec_bmp_tags, tag1);
                    size -= (5 * 4);
                    get_be32();
                    get_be32();
                    get_be32();
                    colors = get_le32();
//                    printf("colors: %d\n", colors);
                    get_be32();
                    size -= (5 * 4);
                    if (vi.depth <= 8) {
//                      printf("reading...\n");
                      ampio_fread(avi_pal, (colors * 4));
                      size -= (colors * 4);
                    }
                    if (size > 0)
                      ampio_fskip(size);
                    break;
                case CODEC_TYPE_AUDIO:
                    tag1 = get_le16();
                    ai.id = tag1; /* FIXME */
//                    st->codec.codec_type = CODEC_TYPE_AUDIO;
//                    st->codec.codec_tag = tag1;
#ifdef DEBUG
                    printf("audio: 0x%lx\n", tag1);
#endif
//                    st->codec.channels = get_le16();
//                    st->codec.sample_rate = get_le32();
//                    st->codec.bit_rate = get_le32() * 8;

                    ai.channels = get_le16();
                    ai.rate = get_le32();
                    get_le32();
                    ai.stream = stream_index;

                    get_le16(); /* block align */
                    bps = get_le16();
//                    st->codec.codec_id = wav_codec_get_id(tag1, bps);
                    ai.bits = bps; /* bits per sample */
                    ampio_fskip(size - 4 * 4);
                    break;
                default:
                    ampio_fskip(size);
                    break;
                }
            }
            break;
        default:
            /* skip tag */
            size += (size & 1);
            ampio_fskip(size);
            break;
        }
    }
end_of_header:
    /* check stream number */
    if (stream_index != nb_streams - 1) {
    fail:
#if 0
        for(i=0;i<s->nb_streams;i++) {
            if (s->streams[i])
                free(s->streams[i]);
        }
#endif
        return -1;
    }

    return 0;
}

static int decoders_running = 0;

static int do_audio = 0, do_video = 0;

#include "../../refresh/refresh.h" /* FIXME: REMOVE! */

static int avi_read_packet(void)
{
  int id, size, stream, type, buf;

find_next:
  if (ampio_feof()) return -1;

  id = get_le32();
  size = get_le32();

  if (id == MKTAG('L', 'I', 'S', 'T')) {
    get_le32();
    goto find_next;
  }

  id = SWAP32(id); /* FIXME */

  stream = (id >> 16) - 0x3030;
  type = id & 0x0000ffff;

  buf = -1;
  if ((type == 0x6462 /* db */ ) || (type == 0x6463 /* dc */ ) || (type == 0x7762 /* wb */)) {
    if ((stream == vi.stream) && (do_video)) {
      buf = 0; // Video
    } else if ((stream == ai.stream) && (do_audio)) {
      buf = 1; // Audio
    }
  }

  if (buf >= 0) {
    while (amp_fwrite(buf, NULL, size, 0.0, ampio_ftell()) == 0) {
      if (decoders_running == 0) {
        decoders_running = 1;

        plugin_init();

        if (do_video) {
          if (plugin_video_init(SUBTYPE_NONE, vi.id, vi.width, vi.height, vi.depth, vi.framerate) != CORE_OK) {
            do_video = 0;
          }
        }

        if (do_audio) {
          if (plugin_audio_init(SUBTYPE_NONE, ai.id, ai.rate, ai.bits, ai.channels) != CORE_OK) {
            do_audio = 0;
          }
        }

        if ((do_video == 0) && (do_audio == 0)) {
          return 1;
        }

        if ((do_video) && (vi.depth == 8)) {
          int i;
          for (i=0; i<256; i++) {
            int r, g, b;
            r = avi_pal[i * 4 + 2];
            g = avi_pal[i * 4 + 1];
            b = avi_pal[i * 4 + 0];

            avi_palette[i] = (r << 16) | (g << 8) | b;
          }
          refresh_palette(avi_palette, 256);
        }

        plugin_start();
      }
    }
    if (size & 1) get_byte(); // Is this right ???
  } else {
    if (size & 1) size++;
    ampio_fskip(size);
  }

  return 0;
}

static int avi_read_close(void)
{
    free(avi);
    return 0;
}

int avi_parser(char *filename, int video, int audio)
{
  int i = 0;

  if (ampio_fopen(filename) != 1) {
    return 0;
  }

  do_video = video;
  do_audio = audio;

  memset(&vi, 0, sizeof(vi));
  memset(&ai, 0, sizeof(ai));
  vi.stream = -1;
  ai.stream = -1;

  if (avi_read_header() < 0) {
    printf("Failed to read header\n");
    return 0;
  }

  if (vi.stream == -1) {
    do_video = 0;
  }

  if (ai.stream == -1) {
    do_audio = 0;
  }

  if ((do_video == 0) && (do_audio == 0)) {
    printf("Neither video nor audio to play\n");
    return 0;
  }

/*
printf("vi: %08lx, %d %d %d %d %.2f\n", vi.id, vi.stream, vi.width, vi.height, vi.depth, vi.framerate);
printf("ai: %08lx, %d %d %d %d\n", ai.id, ai.stream, ai.rate, ai.bits, ai.channels);
printf("streams: %d, %d\n", vi.stream, ai.stream);
*/

  if (avi->index_entries == 0) {
    printf("video has no index, seeking is disabled\n");

    ampio_fseek(avi->movi_start, SEEK_SET);

    while (avi_read_packet() == 0) {
#if 0
      volatile unsigned char *lmb = (unsigned char *)0xbfe001;
      if ((*lmb & 0x40) == 0) { /* LMB */
        video_fempty();
        audio_fempty();
        break;
      }
#else
      /* FIXME: The "every 4th write" hack is no good */
      if ((decoders_running == 1) && ((i & 3) == 0)) {
        int check, set, pos;
        long long length;

        if (do_video) {
          pos = video_ftell();
        } else {
          pos = audio_ftell();
        }

        length = (long long)ampio_length();
        set = (int)(((long long)pos * (long long)65535) / length);
        check = core_input(set);

        if (check == INPUT_QUIT) {
          video_fempty();
          audio_fempty();
          break;
        }
      }
      i++;
#endif
    }
  } else {
    long tot = 0, apos = 0, vpos = 0, offset = 0, entries = 0, has_keyframes = 0;
    char video_tag[4], audio_tag[4], data[8];
    AVIIndex *idx;

#ifdef DEBUG
    printf("entries: %d\n", avi->index_entries);
#endif

    video_tag[0] = vi.stream/10 + '0';
    video_tag[1] = vi.stream%10 + '0';
    video_tag[2] = 'd';
    video_tag[3] = 'b';

    audio_tag[0] = ai.stream/10 + '0';
    audio_tag[1] = ai.stream%10 + '0';
    audio_tag[2] = 'w';
    audio_tag[3] = 'b';

    {
      unsigned long *src, *dst;
      src = (unsigned long *)(avi->index);
      dst = (unsigned long *)(avi->index);

      for (i=0; i<avi->index_entries; i++) {
        dst++;
        src++;
        *dst = SWAP32(*src);
        dst++;
        src++;
        *dst = SWAP32(*src);
        dst++;
        src++;
        *dst = SWAP32(*src);
        dst++;
        src++;
      }
    }

    idx = (AVIIndex *)avi->index; /* FIXME: BAD */

    for (i=0; i<avi->index_entries; i++) {
      idx = (AVIIndex *)(avi->index + (16 * i));

      if (strncasecmp(idx->tag, video_tag, 3) == 0) {
        break;
      }
    }

    ampio_fseek(idx->pos, SEEK_SET);
    ampio_fread(data, 8);

    if ((strncasecmp(data, idx->tag, 4) == 0) && (SWAP32(*(unsigned long *)&data[4]) == idx->len)) {
      printf("start of file\n");
      offset = 8;
    } else {
      ampio_fseek(idx->pos + avi->movi_start - 4, SEEK_SET);
      ampio_fread(data, 8);

      if ((strncasecmp(data, idx->tag, 4) == 0) && (SWAP32(*(unsigned long *)&data[4]) == idx->len)) {
        printf("start of movi list\n");
        offset = avi->movi_start + 4;
      }
    }

    avi_index = malloc(avi->index_entries * sizeof(avi_node));
    memset(avi_index, 0, avi->index_entries * sizeof(avi_node));

    for (i=0; i<avi->index_entries; i++) {
      double pts;
      unsigned char tmp[5] = {0, 0, 0, 0, 0};

      idx = (AVIIndex *)(avi->index + (16 * i));

      tmp[0] = idx->tag[0];
      tmp[1] = idx->tag[1];
      tmp[2] = idx->tag[2];
      tmp[3] = idx->tag[3];

//      printf("%d : %s %08lx %ld %ld\n", i, tmp, idx->flags, idx->pos + offset, idx->len);

      if ((video) && (strncasecmp(idx->tag, video_tag, 3) == 0)) {

        pts = (double)vpos * (double)dwScale / (double)dwRate;

        #define AVIIF_KEYFRAME 0x00000010

        avi_index[entries].timestamp = pts;
        avi_index[entries].type = AVI_VIDEO;
        avi_index[entries].length = idx->len;
        avi_index[entries].position = idx->pos + offset;
        avi_index[entries].is_keyframe = ((idx->flags & AVIIF_KEYFRAME) > 0); /* FIXME */
        entries++;

        if ((idx->flags & AVIIF_KEYFRAME) > 0) {
          has_keyframes = 1;
        }

        vpos++;
      } else if ((audio) && (strncasecmp(idx->tag, audio_tag, 4) == 0)) {

        if (dwSampleSize == 0) {
          pts = (double)apos * (double)dwScale_audio / (double)dwRate_audio;
        } else {
          pts = (double)tot / (double)dwSampleSize * (double)dwScale_audio / (double)dwRate_audio;
        }

        avi_index[entries].timestamp = pts;
        avi_index[entries].type = AVI_AUDIO;
        avi_index[entries].length = idx->len;
        avi_index[entries].position = idx->pos + offset;
        avi_index[entries].is_keyframe = 0; /* FIXME */
        entries++;

        apos++;
        tot += idx->len;
      }
    }

#if 0
{
  int i;
  for (i=0; i<entries; i++) {
    printf("%s %ld -> %ld %ld %f\n", (avi_index[i].type == AVI_VIDEO) ? "VIDEO" : "AUDIO",
                              (long)avi_index[i].position, (long)avi_index[i].position + avi_index[i].length,
                              avi_index[i].length, avi_index[i].timestamp);
  }
}
#endif

    if (has_keyframes) {
      printf("video has keyframes\n");
    } else {
      printf("video has NO keyframes\n");
    }

    /* Play the movie */
    for (i=0; i<entries; i++) {
#if 0
      volatile unsigned char *lmb = (unsigned char *)0xbfe001;
      if ((*lmb & 0x40) == 0) { /* LMB */
        video_fempty();
        audio_fempty();
        break;
      }
#else
      /* FIXME: The "every 4th write" hack is no good */
      if ((decoders_running == 1) && ((i & 3) == 0)) {
        int check, set, pos;

        if (do_video) {
          pos = video_ftell();
        } else {
          pos = audio_ftell();
        }

        set = (pos * 65536) / entries;
        check = core_input(set);

        if ((check >= 0) && (has_keyframes)) {
          i = (entries * check) / 100; /* MAX 100 % */

          /* Search to next keyframe */
          while (avi_index[i].is_keyframe == 0) {
            i++;
            if (i >= entries) {
              break;
            }
          }

          if (audio) {
            audio_fseek();
          }

          if (video) {
            video_fseek();
          }
        } else if (check == INPUT_QUIT) {
          video_fempty();
          audio_fempty();
          break;
        }
      }
#endif

      if ((i >= entries) || (i < 0)) {
        break;
      }

      ampio_fseek((int)avi_index[i].position, SEEK_SET);

      while (amp_fwrite(avi_index[i].type, NULL, avi_index[i].length, avi_index[i].timestamp, i) == BUF_FALSE) {
        if (decoders_running == 0) {
          decoders_running = 1;

          plugin_init();

          if (video) {
            if (plugin_video_init(SUBTYPE_NONE, vi.id, vi.width, vi.height, vi.depth, vi.framerate) != CORE_OK) {
              video = 0;
            }
          }

          if (audio) {
            if (plugin_audio_init(SUBTYPE_NONE, ai.id, ai.rate, ai.bits, ai.channels) != CORE_OK) {
              audio = 0;
            }
          }

          if ((audio == 0) && (video == 0)) {
            goto exit; /* FIXME */
          }

          plugin_start();
        }
      }
    }
  }

  avi_read_close();

exit:

  if (decoders_running) {
    plugin_exit();
  }

  ampio_fclose();

  return 0;
}
