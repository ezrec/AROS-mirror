/*
 * amiaudiodef.h
 */

#ifndef AMIAUDIODEF_H
#define AMIAUDIODEF_H

/* sample */
#define AUDIO_8BIT   ( 0)
#define AUDIO_16BIT  ( 1)
#define AUDIO_24BIT  ( 2)
#define AUDIO_32BIT  ( 3)
#define AUDIO_FLOAT  ( 4) /* 32 bit (-1.0 ... 1.0) or (0.0 ... 1.0) */
#define AUDIO_DOUBLE ( 5) /* 64 bit (-1.0 ... 1.0) or (0.0 ... 1.0) */

/* sign */
#define AUDIO_SIGNED   (0 << 6)
#define AUDIO_UNSIGNED (1 << 6)

/* endian */
#define AUDIO_BE (0 << 7)
#define AUDIO_LE (1 << 7)

/* channel layout (F = Front, R = Rear)
 *
 * 1F        | 1F1F      | 2F        | 3F        | 2F1R
 *
 * -   X   - | -   X   - | X   -   X | X   X   X | X   -   X
 * -   -   - | -   -   - | -   -   - | -   -   - | -   X   -
 *   - - -   |   - - -   |   - - -   |   - - -   |   - - -
 *
 * 3F1R      | 2F2R      | 3F2R      | 3F3R      | 3F4R
 *
 * X   X   X | X   -   X | X   X   X | X   X   X | X   X   X
 * -   X   - | X   -   X | X   -   X | X   -   X | X   -   X
 *   - - -   |   - - -   |   - - -   |   - X -   |   X - X
 */
#define AUDIO_1F   ( 0 << 8)
#define AUDIO_1F1F ( 1 << 8) /* 1 + 1 (dual mono, 2 independent channels) */
#define AUDIO_2F   ( 2 << 8)
#define AUDIO_3F   ( 3 << 8)
#define AUDIO_2F1R ( 4 << 8)
#define AUDIO_3F1R ( 5 << 8)
#define AUDIO_2F2R ( 6 << 8)
#define AUDIO_3F2R ( 7 << 8)
#define AUDIO_3F3R ( 8 << 8)
#define AUDIO_3F4R ( 9 << 8)
#define AUDIO_LFE  (16 << 8) /* low frequency effects channel (subwoofer) */

/* format helpers */
#define AUDIO_FORMAT(x) ((x) & 0x000000ff)
#define AUDIO_LAYOUT(x) ((x) & 0x0000ff00)

/* setup */
#define AUDIO_DONE            ( 0)
#define AUDIO_STEREO          ( 1)
#define AUDIO_AHI             ( 2)
#define AUDIO_PAULA           ( 3)
#define AUDIO_PAULA_56KHZ     ( 4)
#define AUDIO_PAULA_14BIT     ( 5)
#define AUDIO_PAULA_CALIB     ( 6)

/* format */
#define MODE_16     (0x01) /* 16 bit */
#define MODE_STEREO (0x02) /* stereo */

/* format helpers */
#define AUDIO_M8S  (0x00 | 0x00)  /*  8 bit signed, mono */
#define AUDIO_M16S (0x00 | 0x01)  /* 16 bit signed, mono */
#define AUDIO_S8S  (0x02 | 0x00)  /*  8 bit signed, stereo */
#define AUDIO_S16S (0x02 | 0x01)  /* 16 bit signed, stereo */

/* time */
#define TIMEBASE (22500)      /* ticks per second */
#define NOTIME   (0xffffffff) /* when no timestamp is available */

#endif /* AMIAUDIODEF_H */
