/*
 *
 * version.c
 *
 */

#include "version.h"

#define BETA

#ifdef BETA

const char ver[] = "$VER: AMP2-BETA (040716)";

#else

const char ver[] = "$VER: AMP v2.05 (030413)";

#endif