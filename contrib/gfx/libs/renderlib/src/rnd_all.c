
#define LIBAPI static

#ifdef __MORPHOS__
#include "lib_init_mos.c"
#else
#include "lib_init_68k.c"
#endif

#include "lib_global.c"
#include "rnd_memory.c"
#include "rnd_histogram.c"
#include "rnd_palette.c"
#include "rnd_quant.c"
#include "rnd_mapping.c"
#include "rnd_conversion.c"
#include "rnd_render.c"
#include "rnd_diversity.c"
#include "rnd_alpha.c"
#include "rnd_scale.c"
