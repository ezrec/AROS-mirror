/*
 *
 * rm.c
 *
 */

#include <stdio.h>
#include <stdlib.h>

#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "../../common/core.h"
#include "../../common/ampio.h"
#include "../../main/main.h"
#include "../../main/buffer.h"

#define FOURCC_RMOV FOURCC( 'R', 'M', 'O', 'V') /* AMP specific, RealMOVie */
#define FOURCC_rmov FOURCC( 'r', 'm', 'o', 'v')

typedef struct {
  unsigned long fourcc;
  unsigned long id;
  unsigned long subid;
  /* Video */
  int stream;
  int width;
  int height;
  int depth;
  double framerate;
} video_info;

typedef struct {
  unsigned long fourcc;
  unsigned long id;
  unsigned long subid;
  /* Audio */
  int stream;
  int rate;
  int bits;
  int channels;
} audio_info;

static video_info vi;
static audio_info ai;

typedef struct {
    int nb_packets;
    int packet_total_size;
    int packet_max_size;
    /* codec related output */
    int bit_rate;
    float frame_rate;
    int nb_frames;    /* current frame number */
    int total_frames; /* total number of frames */
    int num;
} StreamInfo;

typedef struct {
    StreamInfo streams[2];
    StreamInfo *audio_stream, *video_stream;
    int data_pos; /* position of the data after the header */
    int nb_packets;
} RMContext;

/***************************************************/

#define PRINTF if (1 == 0) printf

static void get_str(char *buf, int buf_size)
{
    int len, i;
    char *q;

    len = get_be16();
    q = buf;
    for(i=0;i<len;i++) {
        if (i < buf_size - 1) {
            *q++ = get_byte();
        } else {
            get_byte();
        }
    }
    *q = '\0';
}

static void get_str8(char *buf, int buf_size)
{
    int len, i;
    char *q;

    len = get_byte();
    q = buf;
    for(i=0;i<len;i++) {
        if (i < buf_size - 1) {
            *q++ = get_byte();
        } else {
            get_byte();
        }
    }
    *q = '\0';
}

#define MKTAG(a, b, c, d) FOURCC(d, c, b, a)

static RMContext *rm = NULL; /* FIXME */
static int codec_tag = 0xffffffff;

static int rm_read_header(void) //AVFormatContext *s, AVFormatParameters *ap)
{
    #define EIO 1
    #define ENOMEM 2

    unsigned int tag, v, stream;
    int tag_size, size, codec_data_size;
//    INT64 codec_pos;
    int codec_pos;
    unsigned int h263_hack_version;
    char buf[128]; /* FIXME: Too small ! */

//    if (get_le32() != MKTAG('.', 'R', 'M', 'F'))
//        return -EIO;
    get_le32(); /* ".RMF" */
    rm = malloc(sizeof(RMContext));
    if (!rm)
        return -ENOMEM;
//    s->priv_data = rm;

    get_be32(); /* header size */
    get_be16();
    get_be32();
    get_be32(); /* number of headers */
    
    for(;;) {
        if (ampio_feof())
            goto fail;
        tag = get_le32();
        tag_size = get_be32();
        get_be16();
#if 0
        printf("tag=%c%c%c%c (%08x) size=%d\n", 
               (tag) & 0xff,
               (tag >> 8) & 0xff,
               (tag >> 16) & 0xff,
               (tag >> 24) & 0xff,
               tag,
               tag_size);
#endif
        if (tag_size < 10)
            goto fail;

        switch(tag) {
        case MKTAG('P', 'R', 'O', 'P'):
            /* file header */
            get_be32(); /* max bit rate */
            get_be32(); /* avg bit rate */
            get_be32(); /* max packet size */
            get_be32(); /* avg packet size */
            get_be32(); /* nb packets */
            get_be32(); /* duration */
            get_be32(); /* preroll */
            get_be32(); /* index offset */
            get_be32(); /* data offset */
            get_be16(); /* nb streams */
            get_be16(); /* flags */
            break;
        case MKTAG('C', 'O', 'N', 'T'):
//            get_str(s->title, sizeof(s->title));
//            get_str(s->author, sizeof(s->author));
//            get_str(s->copyright, sizeof(s->copyright));
//            get_str(s->comment, sizeof(s->comment));
            get_str(buf, sizeof(buf));
            get_str(buf, sizeof(buf));
            get_str(buf, sizeof(buf));
            get_str(buf, sizeof(buf));
            break;
        case MKTAG('M', 'D', 'P', 'R'):
//            st = av_mallocz(sizeof(AVStream));
//            if (!st)
//                goto fail;
//            s->streams[s->nb_streams++] = st;
//            st->id = get_be16();
            stream = get_be16(); /* FIXME */
            get_be32(); /* max bit rate */
//            st->codec.bit_rate = get_be32(); /* bit rate */
            get_be32(); /* bit rate */
            get_be32(); /* max packet size */
            get_be32(); /* avg packet size */
            get_be32(); /* start time */
            get_be32(); /* preroll */
            get_be32(); /* duration */
            get_str8(buf, sizeof(buf)); /* desc */
            get_str8(buf, sizeof(buf)); /* mimetype */
            codec_data_size = get_be32();
//            codec_pos = url_ftell();
            codec_pos = ampio_ftell();

            v = get_be32();
            if (v == MKTAG(0xfd, 'a', 'r', '.')) {
                //ai.stream = stream;
                /* ra type header */
                get_be32(); /* version */
                get_be32(); /* .ra4 */
                get_be32();
                get_be16();
                get_be32(); /* header size */
                get_be16(); /* add codec info */
                get_be32(); /* coded frame size */
                get_be32(); /* ??? */
                get_be32(); /* ??? */
                get_be32(); /* ??? */
                get_be16(); /* 1 */ 
                get_be16(); /* coded frame size */
                get_be32();
//                st->codec.sample_rate = get_be16();
                ai.rate = get_be16();
                get_be32();
//                st->codec.channels = get_be16();
                ai.channels = get_be16();
                get_str8(buf, sizeof(buf)); /* desc */
                get_str8(buf, sizeof(buf)); /* desc */
//                st->codec.codec_type = CODEC_TYPE_AUDIO;
//                if (!strcmp(buf, "dnet")) {
//                    st->codec.codec_id = CODEC_ID_AC3;
//                } else {
//                    st->codec.codec_id = CODEC_ID_NONE;
//                    nstrcpy(st->codec.codec_name, sizeof(st->codec.codec_name),
//                            buf);
//                }
                if (!strcmp(buf, "dnet")) {
                    ai.stream = stream;
                } else {
                    ai.stream = -1;
                }
            } else {
                if (get_le32() != MKTAG('V', 'I', 'D', 'O')) {
//                    printf("Unsupported video codec\n");
                    goto skip_it;
                }
                vi.stream = stream;
                codec_tag = get_le32();
//                if (st->codec.codec_tag != MKTAG('R', 'V', '1', '0'))
//                    goto fail1;
//                st->codec.width = get_be16();
//                st->codec.height = get_be16();
//                st->codec.frame_rate = get_be16() * FRAME_RATE_BASE;
//                st->codec.codec_type = CODEC_TYPE_VIDEO;
//                get_le32();
                vi.width = get_be16();
                vi.height = get_be16();
                get_be16();

                get_be32();
                get_be16();
//                get_be32();
//                get_be16();

                get_be16();
                vi.subid = get_be32();

                /* modification of h263 codec version (!) */
                h263_hack_version = get_be32();

                vi.id = h263_hack_version;

                switch(h263_hack_version) {
                case 0x10000000:
//                    st->codec.sub_id = 0;
//                    st->codec.codec_id = CODEC_ID_RV10;
//printf("sub_id: 0\n");
vi.fourcc = FOURCC_RV10;
                    break;
                case 0x10003000:
                case 0x10003001:
//                    st->codec.sub_id = 3;
//                    st->codec.codec_id = CODEC_ID_RV10;
//printf("sub_id: 3\n");
vi.fourcc = FOURCC_RV13;
                    break;
                default:
                    /* not handled, maybe... */
//                    st->codec.codec_id = CODEC_ID_NONE;
//                    vi.stream = -1;
vi.fourcc = FOURCC_RMOV;
                    break;
                }
            }
            /* skip codec info */
skip_it:
            size = ampio_ftell() - codec_pos;
            ampio_fskip(codec_data_size - size);
            break;
        case MKTAG('D', 'A', 'T', 'A'):
            goto header_end;
        default:
            /* unknown tag: skip it */
            ampio_fskip(tag_size - 10);
            break;
        }
    }
 header_end:
    rm->nb_packets = get_be32(); /* number of packets */
    get_be32(); /* next data header */
    return 0;

 fail:
    return -EIO;
}

static int decoders_running = 0;

static unsigned char *bad_buf = NULL;

static int do_audio = 0, do_video = 0;
static unsigned char *read_buf = NULL, *asf_packet = NULL, *true_read_buf = NULL;
static unsigned int asf_seq, dp_len;

static void my_fwrite(int buf, unsigned char *src, int len, double do_time, int flags, int extra, int file_pos)
{
  src -= 8; /* this works beacuse read_buf = true_read_buf + 8 */

  *((unsigned int *)(src + 0)) = ((flags & 0x2) ? 0x10 : 0);
  *((unsigned int *)(src + 4)) = len;

  len += 8; /* above */
  len += extra;

PRINTF("my_fwrite: %d (%d), %08x\n", len, extra, flags);

  while (amp_fwrite(buf, src, len, do_time, file_pos) == BUF_FALSE) {
    if (decoders_running == 0) {
      decoders_running = 1;

//exit(0);

      plugin_init();

      if (do_video) {
        plugin_set_id(vi.id, vi.subid);
        if (plugin_video_init(SUBTYPE_NONE, vi.fourcc, vi.width, vi.height, vi.depth, vi.framerate) != CORE_OK) {
          do_video = 0;
        }
      }

      if (do_audio) {
        if (plugin_audio_init(SUBTYPE_A52, FOURCC_NONE, ai.rate, ai.bits, ai.channels) != CORE_OK) {
          do_audio = 0;
        }
      }

      plugin_start();
    }
  }
}

static int rm_read_packet(void)
{
  int buf, len, num, timestamp, tmp, j, flags;
  unsigned char *ptr, *src;
  double do_time = 0.0;

  if ((do_video == 0) && (do_audio == 0)) {
    return -1;
  }

  get_be16();
  len = get_be16();
  if (len < 12) {
    return -EIO;
  }
  num = get_be16();
  timestamp = get_be32();
  get_byte(); /* reserved */
  flags = get_byte(); /* flags */
  rm->nb_packets--;
  len -= 12;

  buf = -1;
  do_time = (double)timestamp / (double)1000.0;

PRINTF("%.2f\n", do_time);

  if ((num == vi.stream) && (do_video)) {
    buf = 0; // Video
  } else if ((num == ai.stream) && (do_audio)) {
    buf = 1; // Audio
  }

  if (buf == 0) {
      /* CODEC_TYPE_VIDEO */
      if ((codec_tag == 0x30335652) || (codec_tag == 0x30325652)) {
        int vpkg_header, vpkg_length, vpkg_offset, vpkg_seqnum = -1, extra_add;
        unsigned int *extra;

        PRINTF("len: %d\n", len);

extra_add = 0;

        while (len > 0) {
          vpkg_header = get_byte(); len--;

          PRINTF("vpkg_header: %02x\n", vpkg_header & 0xff);

          if ((vpkg_header & 0xc0) == 0x40) {
            // seems to be a very short header
            // 2 bytes, purpose of the second byte yet unknown
            int bummer;
            bummer = get_byte(); len--;

            vpkg_offset = 0;
            vpkg_length = len;
          } else {
            if ((vpkg_header & 0x40) == 0) {
              // sub-seqnum (bits 0-6: number of fragment. bit 7: ???)
              vpkg_seqnum = get_byte(); len--;
            }

            // size of the complete packet
            // bit 14 is always one (same applies to the offset)
            vpkg_length = get_be16(); len -= 2;

PRINTF("vpkg_length 1: %2d\n", vpkg_length);

            if (vpkg_length == 0) {
              vpkg_length = get_be16(); len -= 2;

PRINTF("vpkg_length 2: %2d\n", vpkg_length);

            } else {
              vpkg_length &= 0x3fff;

PRINTF("vpkg_length 3: %2d\n", vpkg_length);

            }

            // offset of the following data inside the complete packet
            // Note: if (hdr&0xC0)==0x80 then offset is relative to the
            // _end_ of the packet, so it's equal to fragment size!!!
            vpkg_offset = get_be16(); len -= 2;

PRINTF("vpkg_offset 1: %2d\n", vpkg_offset);

            if (vpkg_offset == 0) {
              vpkg_offset = get_be16(); len -= 2;

PRINTF("vpkg_offset 2: %2d\n", vpkg_offset);

            } else {
              vpkg_offset &= 0x3fff;

PRINTF("vpkg_offset 3: %2d\n", vpkg_offset);

            }

            vpkg_seqnum = get_byte(); len--;

PRINTF("vpkg_seqnum: %2x\n", vpkg_seqnum & 0xff);
          }

          if(asf_packet) {
PRINTF("asf_packet\n");
            if (asf_seq != vpkg_seqnum) {
              // this fragment is for new packet, close the old one
PRINTF("new packet\n");
              asf_packet = NULL;

PRINTF("dp_len: %d\n", dp_len);

            PRINTF("decode: %d, flags: %08x\n", dp_len, flags);
//            rp_decode(read_buf, dp_len, (flags & 0x2) ? 0x10 : 0);
            my_fwrite(0 /* VIDEO */, read_buf, dp_len, do_time, flags, extra_add, rm->nb_packets);

            } else {
              // append data to it!
              extra = (unsigned int *)(read_buf + vpkg_length);
              extra[0]++;
              extra[2 + 2*extra[0]]=1;
              extra[3 + 2*extra[0]]=dp_len;

extra_add = 4 * (4 + 2*extra[0]); /* */

              if ((vpkg_header & 0xc0) == 0x80){
                // last fragment!
                ampio_fread(read_buf + dp_len, vpkg_offset);
                dp_len += vpkg_offset;
                len -= vpkg_offset;
                asf_packet = NULL;

PRINTF("dp_len: %d\n", dp_len);

                PRINTF("decode: %d, flags: %08x\n", dp_len, flags);
//                rp_decode(read_buf, dp_len, (flags & 0x2) ? 0x10 : 0);
                my_fwrite(0 /* VIDEO */, read_buf, dp_len, do_time, flags, extra_add, rm->nb_packets);

                // continue parsing
                continue;
              }

              // non-last fragment:
              ampio_fread(read_buf + dp_len, len);
              dp_len += len;
              break; // no more fragments in this chunk!
            }
          }

          memset(read_buf, 0, vpkg_length+8*5);

          asf_seq = vpkg_seqnum;
          extra = (unsigned int *)(read_buf + vpkg_length);
          extra[0] = 0; // blocks
          extra[1] = timestamp;
          extra[2] = 1; // sub-1
          extra[3] = 0;

extra_add = 4 * 4; /* 4 * sizeof(int) */

          if ((vpkg_header & 0xc0) == 0x00) {
            // first fragment:
            dp_len = len;

PRINTF("first fragment\n");

            ampio_fread(read_buf, len);
            asf_packet = read_buf;
            break;
          } else {
            // whole packet (not fragmented):

PRINTF("whole packet\n");

            ampio_fread(read_buf, vpkg_length);

            PRINTF("decode: %d, flags: %08x\n", vpkg_length, flags);
//            rp_decode(read_buf, vpkg_length, (flags & 0x2) ? 0x10 : 0);
            my_fwrite(0 /* VIDEO */, read_buf, vpkg_length, do_time, flags, extra_add, rm->nb_packets);
          }
          len -= vpkg_length;
        }
      } else {
//        get_byte();
//        get_byte();
//        get_be16();
//        get_be16();
//        get_byte();
//        len -= 7;

//        ampio_fread(read_buf, len);

//        PRINTF("decode: %d, flags: %08x\n", len, flags);
//        rp_decode(read_buf, len, (flags & 0x2) ? 0x10 : 0);
//        my_fwrite(0 /* VIDEO */, read_buf, len, do_time, flags, rm->nb_packets);

        while (amp_fwrite(buf, NULL, len, do_time, rm->nb_packets) == BUF_FALSE) {
          if (decoders_running == 0) {
            decoders_running = 1;

            plugin_init();

            if (do_video) {
              if (plugin_video_init(SUBTYPE_NONE, vi.fourcc, vi.width, vi.height, vi.depth, vi.framerate) != CORE_OK) {
                do_video = 0;
              }
            }

            if (do_audio) {
              if (plugin_audio_init(SUBTYPE_A52, FOURCC_NONE, ai.rate, ai.bits, ai.channels) != CORE_OK) {
                do_audio = 0;
              }
            }

            if ((do_video == 0) && (do_audio == 0)) {
              return -1;
            }

            plugin_start();
          }
        }
      }
  } else if (buf == 1) {
    // Can ONLY be AC3 for now!
    src = bad_buf; /* push */

    ampio_fread(bad_buf, len); /* Bad (=slow), but what can I do ? */
    ptr = bad_buf;
    for (j=0;j<len;j+=2) {
      tmp = ptr[0];
      ptr[0] = ptr[1];
      ptr[1] = tmp;
      ptr += 2;
    }

    while (amp_fwrite(1, src, len, do_time, rm->nb_packets) == BUF_FALSE) {
      if (decoders_running == 0) {
        decoders_running = 1;

        plugin_init();

        if (do_video) {
          if (plugin_video_init(SUBTYPE_NONE, vi.fourcc, vi.width, vi.height, vi.depth, vi.framerate) != CORE_OK) {
            do_video = 0;
          }
        }

        if (do_audio) {
          if (plugin_audio_init(SUBTYPE_A52, FOURCC_NONE, ai.rate, ai.bits, ai.channels) != CORE_OK) {
            do_audio = 0;
          }
        }

        if ((do_video == 0) && (do_audio == 0)) {
          return -1;
        }

        plugin_start();
      }
    }
  } else {
    ampio_fskip(len);
  }

  return 0;
}

static int rm_read_close(void) //AVFormatContext *s)
{
  free(rm);
  return 0;
}

int rm_parser(char *filename, int video, int audio)
{
  int i = 0, total;

  if (ampio_fopen(filename) != 1) {
    return 0;
  }

  true_read_buf = malloc(1024*64+8);
  memset(true_read_buf, 0, 1024*64+8);
  read_buf = true_read_buf + 8;

  bad_buf = malloc(1024*64);
  memset(bad_buf, 0, 1024*64);

  do_video = video;
  do_audio = audio;

  memset(&vi, 0, sizeof(vi));
  memset(&ai, 0, sizeof(ai));
  vi.stream = -1;
  ai.stream = -1;

  if (rm_read_header() < 0) {
    printf("Failed to read header\n");
    return 0;
  }

/*
printf("vi: %08lx, %d, %d %d %.2f\n", vi.fourcc, vi.stream, vi.width, vi.height, vi.framerate);
printf("ai: %08lx, %d, %d %d %d\n", ai.id, ai.stream, ai.rate, ai.bits, ai.channels);
*/

  if ((vi.stream == -1) && (ai.stream == -1)) {
    printf("Both video and audio codecs are unsupported\n");
    return 0;
  }

  total = rm->nb_packets;
  while (rm->nb_packets > 0) {
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
      int check, set, packet;

      if (do_video) {
        packet = video_ftell();
      } else {
        packet = audio_ftell();
      }

      set = ((total - packet) * 65536) / total;
      check = core_input(set);

      if (check == INPUT_QUIT) {
        video_fempty();
        audio_fempty();
        break;
      }
    }
    i++;
#endif
    if (rm_read_packet() < 0) {
      break;
    }
  }

  rm_read_close();

  if (decoders_running) {
    plugin_exit();
  }

  ampio_fclose();

  return 0;
}

/*
tag=PROP (504f5250) size=50
tag=MDPR (5250444d) size=155
tag=MDPR (5250444d) size=116
tag=CONT (544e4f43) size=247
tag=DATA (41544144) size=1582384
tag=INDX (58444e49) size=3072
tag=INDX (58444e49) size=20
*/
