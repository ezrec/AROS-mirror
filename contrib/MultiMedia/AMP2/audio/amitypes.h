/*
 * amitypes.h
 */

#ifndef AMITYPES_H
#define AMITYPES_H

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

/* 8bit */
typedef signed char s8;
typedef unsigned char u8;

/* 16bit */
typedef signed short s16;
typedef unsigned short u16;

/* 32bit */
typedef signed long s32;
typedef unsigned long u32;
typedef float f32;

/* 64bit */
typedef signed long long s64;
typedef unsigned long long u64;
typedef double f64;

/* 32bit or 64bit */
typedef signed int si;
typedef unsigned int ui;

/* empty declarations */
typedef struct rect_s rect_t;
typedef struct amiaudio_s amiaudio_t;
typedef struct amivideo_s amivideo_t;

#endif /* AMITYPES_H */
