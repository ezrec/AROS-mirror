/*
 *
 * core_types.h
 *
 */

#ifndef CORE_TYPES_H
#define CORE_TYPES_H

/* Layout of the 32-bit type. Each letter represents one bit of the longword.
 *
 * TTTFFFFF SSSSSSSS CCCCCCCC CCCCCCCC
 *
 * This gives:
 *
 * T : 3 types, no more should be needed.
 * F : 5 features, 3 used currently, 5 should be more than enough.
 * S : 8 subtypes, 3 used, don't think I'll ever need more than 8.
 * C : 16 bits of custom data, for example "samples to bytes" for QT.
 *
 * The features could be seen as a 5-bit number, giving 32 posibilities, but
 * that's something I'll have to consider later when there is need to add more.
 *
 * Only stream based formats will require a subtype, frame based formats does
 * not, or if one prefer, frame based formats have subtype zero.
 *
 * The custom data can be a bit pattern or a 16-bit number.
 *
 * The lores audio and video modes can mean anything that is somehow faster
 * but also less attractive than the non-lores types. It could be a lower
 * video resolution, lower audio sample rate, lower precision in general and
 * such. The idea is that it should be possible to have two almost identical
 * plugins with the only difference being that one is lores and one is not.
 * People with slow computers should use the lores version for better speed.
 */

/* Types */

#define TYPE_VIDEO (0x80000000)
#define TYPE_AUDIO (0x40000000)
#define TYPE_LORES (0x20000000) /* Must be used together with video or audio */

/* Subtypes */

#define SUBTYPE_NONE  (0x00000000) /* Use for frame based formats like AVI/QT */
#define SUBTYPE_MPEG1 (0x00010000)
#define SUBTYPE_MPEG2 (0x00020000)
#define SUBTYPE_MPEGX (0x00030000) /* Will automatically detect MPEG1 or MPEG2 */
#define SUBTYPE_A52   (0x00040000)

/* Features */

#define FEATURE_GRAY      (0x10000000) /* Image can be decoded as gray (8-bit chunky) */
#define FEATURE_DIVISOR_2 (0x08000000) /* Can decode audio at half the sample rate */
#define FEATURE_DIVISOR_4 (0x04000000) /* Can decode audio at a quarter the sample rate */

/* Helpers */

#define GET_TYPE(a)    ((a) & 0xe0000000) /* Mask out type */
#define GET_SUBTYPE(a) ((a) & 0x00ff0000) /* Mask out subtype */
#define GET_FEATURE(a) ((a) & 0x1f000000) /* Mask out feature */
#define GET_CUSTOM(a)  ((a) & 0x0000ffff) /* Mask out custom data */
#define GET_COMMON(a)  ((a) & 0xffff0000) /* Mask out type, subtype and feature */
#define GET_TYPES(a)   ((a) & 0xe0ff0000) /* Mask out type and subtype */

/* FOURCC */
#define FOURCC(a,b,c,d) ((a << 24) | (b << 16) | (c << 8) | (d)) /* Create FOURCC */

/* FOURCC, Common */
#define FOURCC_NONE FOURCC(0x00,0x00,0x00,0x00) /* Use for stream based formats like MPEG */

/* FOURCC, Video */
#define FOURCC_CVID FOURCC( 'C', 'V', 'I', 'D') /* Radius CinePak */
#define FOURCC_cvid FOURCC( 'c', 'v', 'i', 'd') /* Raduis CinePak */
#define FOURCC_U263 FOURCC( 'U', '2', '6', '3') /* H263 */
#define FOURCC_I263 FOURCC( 'I', '2', '6', '3') /* Intel H263 */
#define FOURCC_MJPG FOURCC( 'M', 'J', 'P', 'G') /* MJPEG */
#define FOURCC_jpeg FOURCC( 'j', 'p', 'e', 'g') /* JPEG */
#define FOURCC_DIVX FOURCC( 'D', 'I', 'V', 'X') /* MPEG4 */
#define FOURCC_divx FOURCC( 'd', 'i', 'v', 'x') /* MPEG4 */
#define FOURCC_MPG4 FOURCC(0x04,0x00,0x00,0x00) /* MPEG4, broken */
#define FOURCC_DIV3 FOURCC( 'D', 'I', 'V', '3') /* MSMPEG4, DivX */
#define FOURCC_MP43 FOURCC( 'M', 'P', '4', '3') /* MSMPEG4, DivX */
#define FOURCC_RV10 FOURCC( 'R', 'V', '1', '0') /* RV10, Real Video, ID 0 */
#define FOURCC_RV13 FOURCC( 'R', 'V', '1', '3') /* RV10, Real Video, ID 3 */
#define FOURCC_MSVC FOURCC( 'M', 'S', 'V', 'C') /* Microsoft Video 1 */
#define FOURCC_msvc FOURCC( 'm', 's', 'v', 'c') /* Microsoft Video 1 */
#define FOURCC_CRAM FOURCC( 'C', 'R', 'A', 'M') /* Microsoft Video 1 */
#define FOURCC_cram FOURCC( 'c', 'r', 'a', 'm') /* Microsoft Video 1 */
#define FOURCC_WHAM FOURCC( 'W', 'H', 'A', 'M') /* Microsoft Video 1 */
#define FOURCC_wham FOURCC( 'w', 'h', 'a', 'm') /* Microsoft Video 1 */
#define FOURCC_SVQ1 FOURCC( 'S', 'V', 'Q', '1') /* Sorenson Video 1 */
#define FOURCC_svq1 FOURCC( 's', 'v', 'q', '1') /* Sorenson Video 1 */

/* FOURCC, Audio */
#define FOURCC_twos FOURCC( 't', 'w', 'o', 's') /* TWOS */
#define FOURCC_raw  FOURCC( 'r', 'a', 'w', ' ') /* RAW, PCM */
#define FOURCC_PCM  FOURCC(0x00,0x00,0x00,0x01) /* 0x00000001, RAW PCM */
#define FOURCC_AC3  FOURCC(0x00,0x00,0x20,0x00) /* 0x00002000, AC3 */
#define FOURCC_MAC3 FOURCC( 'M', 'A', 'C', '3') /* MAC3, MACE */
#define FOURCC_MAC6 FOURCC( 'M', 'A', 'C', '6') /* MAC6, MACE */
#define FOURCC_ima4 FOURCC( 'i', 'm', 'a', '4') /* IMA4, IMAADPCM4 */
#define FOURCC_MP3  FOURCC(0x00,0x00,0x00,0x55) /* 0x00000055, MP3 */

/* Colormodes */
#define CMODE_YUV420 (0x00000001)
#define CMODE_YUV422 (0x00000002)
#define CMODE_YUV444 (0x00000004)
#define CMODE_ARGB32 (0x00000008)
#define CMODE_CHUNKY (0x00000010) /* 8-bit */

/* Result codes */
#define CORE_OK   (0)
#define CORE_FAIL (1)

#endif
