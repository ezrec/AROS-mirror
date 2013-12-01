
#ifndef DATATYPES_ANIMATIONCLASSEXT_H
#define DATATYPES_ANIMATIONCLASSEXT_H 1
/*
**  $VER: animationclass.h 41.2 (23.9.97)
**
**  Extended Interface definitions for DataType animation objects.
**  Note: This include file will be merged with animationclass.h !!
**
**  Written by Roland Mainz, gisburn@w-specht.rhein-ruhr.de
**
*/

#ifndef DATATYPES_ANIMATIONCLASS_H
#include <datatypes/animationclass.h>
#endif /* !DATATYPES_ANIMATIONCLASS_H */

#ifndef LIBRARIES_REALTIME_H
#include <libraries/realtime.h>
#endif /* !LIBRARIES_REALTIME_H */

/* Animation attributes */
#ifndef ADTA_Dummy
#define ADTA_Dummy (DTA_Dummy + 600UL)
#endif /* !ADTA_Dummy */

/* reserved tag space for CBM/AT/AI usage */
#define ADTA_CBMReserved0  (ADTA_Dummy  +  8)
#define ADTA_CBMReserved1  (ADTA_Dummy  +  9)
#define ADTA_CBMReserved2  (ADTA_Dummy  + 10)
#define ADTA_CBMReserved3  (ADTA_Dummy  + 11)
#define ADTA_CBMReserved4  (ADTA_Dummy  + 12)
#define ADTA_CBMReserved5  (ADTA_Dummy  + 13)
#define ADTA_CBMReserved6  (ADTA_Dummy  + 14)
#define ADTA_CBMReserved7  (ADTA_Dummy  + 15)
#define ADTA_CBMReserved8  (ADTA_Dummy  + 16)
#define ADTA_CBMReserved9  (ADTA_Dummy  + 17)
#define ADTA_CBMReserved10 (ADTA_Dummy  + 18)
#define ADTA_CBMReserved11 (ADTA_Dummy  + 19)
#define ADTA_CBMReserved12 (ADTA_Dummy  + 20)
#define ADTA_CBMReserved13 (ADTA_Dummy  + 21)
#define ADTA_CBMReserved14 (ADTA_Dummy  + 22)
#define ADTA_CBMReserved15 (ADTA_Dummy  + 23)

/* Alias */
#define ADTA_BitMapHeader        PDTA_BitMapHeader /* (struct BitMapHeader *) */
#define ADTA_VoiceHeader         SDTA_VoiceHeader  /* (struct VoiceHeader *)  */
#define ADTA_Grab                PDTA_Grab         /* (Point *)               */
#define ADTA_DestFrame           PDTA_DestBitMap   /* (struct BitMap *) -- current remapped frame. Only save to read when object os NOT playing */

/* Timing related */
#define ADTA_TicksPerFrame       (ADTA_Dummy  + 24) /* (ULONG) Like ADTA_FramesPerSecond: TICK_FREQ / fps == tpf */
#define ADTA_CurrTicksPerFrame   (ADTA_Dummy  + 25) /* (ULONG) Current tpf, as set by a slider etc, or set by adaptive
                                                     * FPS rate calculations
                                                     */
#define ADTA_CurrFramesPerSecond (ADTA_Dummy  + 26) /* (ULONG) Like TICK_FREQ / curr_tpf == curr_fps */

/* Touch and burn ! */
#define ADTA_Private0            (ADTA_Dummy  + 27) /* (APTR) Do NOT touch !! */
#define ADTA_Private1            (ADTA_Dummy  + 28) /* (APTR) Do NOT touch !! */

/* Misc */
#define ADTA_AdaptiveFPS         (ADTA_Dummy  + 30) /* (BOOL) Use adaptive FPS rate selection during playback */
#define ADTA_SmartSkip           (ADTA_Dummy  + 31) /* (BOOL) Skip frames based on display time weight        */
#define ADTA_NumPrefetchFrames   (ADTA_Dummy  + 32) /* Number of frames to prefetch by load process           */
#define ADTA_OvertakeScreen      (ADTA_Dummy  + 33) /* Overtake screen palette                                */

#endif /* !DATATYPES_ANIMATIONCLASSEXT_H */


