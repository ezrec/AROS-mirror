/*
 *
 * vcd.c
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "../mpeg/mpeg.h"
#include "../../common/core.h"
#include "../../main/mpegio.h"
#include "../../main/buffer.h"
#include "../../main/prefs.h"

/* VCD, MPEG1 */

void vcd_main(int min_sector, int max_sector, int sector_size)
{
  #define VCD
  #include "vcd_internal.h"
  #undef VCD
}

/* SVCD, MPEG2 */

void svcd_main(int min_sector, int max_sector, int sector_size)
{
  #define SVCD
  #include "vcd_internal.h"
  #undef SVCD
}
