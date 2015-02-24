#ifndef CONFIG_H
#define CONFIG_H

#include <aros/cpu.h>

/* Define to the library version */
#define ALSOFT_VERSION "1.15.1"

/* Define any available alignment declaration */
#define ALIGN(x) __attribute__((aligned(x)))

/* Define to the appropriate 'restrict' keyword */
#define RESTRICT

/* Define if we have the C11 aligned_alloc function */
#undef HAVE_ALIGNED_ALLOC

/* Define if we have the posix_memalign function */
#undef HAVE_POSIX_MEMALIGN

/* Define if we have the _aligned_malloc function */
#undef HAVE__ALIGNED_MALLOC

/* Define if we have SSE CPU extensions */
#undef HAVE_SSE

/* Define if we have ARM Neon CPU extensions */
#undef HAVE_NEON

/* Define if we have the AHI backend */
#define HAVE_AHI

/* Define if we have the ALSA backend */
#undef HAVE_ALSA

/* Define if we have the OSS backend */
#undef HAVE_OSS

/* Define if we have the Solaris backend */
#undef HAVE_SOLARIS

/* Define if we have the DSound backend */
#undef HAVE_DSOUND

/* Define if we have the Windows Multimedia backend */
#undef HAVE_WINMM

/* Define if we have the PortAudio backend */
#undef HAVE_PORTAUDIO

/* Define if we have the sqrtf function */
#define HAVE_SQRTF

/* Define if we have the acosf function */
#define HAVE_ACOSF

/* Define if we have the atanf function */
#define HAVE_ATANF

/* Define if we have the fabsf function */
#define HAVE_FABSF

/* Define if we have the strtof function */
#undef HAVE_STRTOF

/* Define if we have stdint.h */
#define HAVE_STDINT_H

/* Define if we have the __int64 type */
#define HAVE___INT64

/* FIXME: The size macros need to be checked for x86_64 */
/* Define to the size of a long int type */
#define SIZEOF_LONG AROS_SIZEOFULONG

/* Define to the size of a long long int type */
#define SIZEOF_LONG_LONG 8

/* Define to the size of an unsigned int type */
#define SIZEOF_UINT AROS_SIZEOFULONG

/* Define to the size of a void pointer type */
#define SIZEOF_VOIDP AROS_SIZEOFPTR

/* Define if we have GCC's destructor attribute */
#define HAVE_GCC_DESTRUCTOR

/* Define if we have pthread_np.h */
#undef HAVE_PTHREAD_NP_H

/* Define if we have float.h */
#undef HAVE_FLOAT_H

/* Define if we have fenv.h */
#undef HAVE_FENV_H

/* Define if we have fesetround() */
#undef HAVE_FESETROUND

/* Define if we have _controlfp() */
#undef HAVE__CONTROLFP

/* Define if we have usleep() */
#define HAVE_USLEEP

#endif
