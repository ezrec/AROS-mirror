/*
 *
 * mpeg.h
 *
 */

#ifndef MPEG_H
#define MPEG_H

/* BUF2PTS
 *
 * This is written as a define for speed reasons, but since the codesize will increase,
 * make sure it's used as little as possible!
 *
 * The PTS is 33 bits but I just care about the 31 most significant bits to keep the result
 * fit in a 32 bit signed integer since the conversion to double appears to require it,
 * atleast I got negative values when using the 32 most significant bits instead.
 *
 * 30-28 : (buf[0] & 0x0e) << 27;
 * 27-20 : (buf[1] << 20);
 * 19-13 : (buf[2] & 0xfe) << 12;
 *  2-5  : (buf[3] << 5);
 *  4-0  : (buf[4] >> 3);
 *
 * 90000 >> 2 = 22500 because (33 bits >> 2 bits = 31 bits)
 */

#define BUF2PTS(buf) ({ \
  unsigned char *b = (buf); unsigned long l; double p; \
  l = ((b[0] & 0x0e) << 27) | (b[1] << 20) | ((b[2] & 0xfe) << 12) | (b[3] << 5) | (b[4] >> 3); \
  p = (double)l / (double)22500; \
  p; })

/* MPEG Stream ID's */

#define AUDIO_STREAM_ID 0xc0
#define VIDEO_STREAM_ID 0xe0
#define EXTRA_STREAM_ID 0xbd /* AC3 and SUBTITLE */

#define RAW_SECTOR_SIZE 2352
#define VCD_SECTOR_SIZE 2324
#define DVD_SECTOR_SIZE 2048

#define PACK_START_CODE 0x000001ba

#endif
