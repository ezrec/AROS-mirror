#include <stdio.h>
#include "quicktime.h"

int quicktime_ctab_init(quicktime_ctab_t *ctab)
{
     ctab->seed = 0;
     ctab->flags = 0;
     ctab->size = 0;
     ctab->alpha = 0;
     ctab->red = 0;
     ctab->green = 0;
     ctab->blue = 0;
     return 0;
}

int quicktime_ctab_delete(quicktime_ctab_t *ctab)
{
     if(ctab->alpha) free(ctab->alpha);
     if(ctab->red) free(ctab->red);
     if(ctab->green) free(ctab->green);
     if(ctab->blue) free(ctab->blue);
     return 0;
}

int quicktime_read_ctab(quicktime_t *file, quicktime_ctab_t *ctab)
{
     int i;
     
     ctab->seed = quicktime_read_int32(file);
     ctab->flags = quicktime_read_int16(file);
     ctab->size = quicktime_read_int16(file) + 1;
     ctab->alpha = malloc(sizeof(int16_t) * ctab->size);
     ctab->red = malloc(sizeof(int16_t) * ctab->size);
     ctab->green = malloc(sizeof(int16_t) * ctab->size);
     ctab->blue = malloc(sizeof(int16_t) * ctab->size);
     
     for(i = 0; i < ctab->size; i++)
     {
          ctab->alpha[i] = quicktime_read_int16(file);
          ctab->red[i] = quicktime_read_int16(file);
          ctab->green[i] = quicktime_read_int16(file);
          ctab->blue[i] = quicktime_read_int16(file);
     }

     return 0;
}
