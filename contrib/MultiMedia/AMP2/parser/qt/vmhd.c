#include "quicktime.h"

void quicktime_vmhd_init(quicktime_vmhd_t *vmhd)
{
     vmhd->version = 0;
     vmhd->flags = 1;
     vmhd->graphics_mode = 64;
     vmhd->opcolor[0] = 32768;
     vmhd->opcolor[1] = 32768;
     vmhd->opcolor[2] = 32768;
}

void quicktime_vmhd_init_video(quicktime_t *file, 
                                        quicktime_vmhd_t *vmhd, 
                                        int frame_w,
                                        int frame_h, 
                                        float frame_rate)
{
}

void quicktime_vmhd_delete(quicktime_vmhd_t *vmhd)
{
}

void quicktime_read_vmhd(quicktime_t *file, quicktime_vmhd_t *vmhd)
{
     int i;
     vmhd->version = quicktime_read_char(file);
     vmhd->flags = quicktime_read_int24(file);
     vmhd->graphics_mode = quicktime_read_int16(file);
     for(i = 0; i < 3; i++)
          vmhd->opcolor[i] = quicktime_read_int16(file);
}

