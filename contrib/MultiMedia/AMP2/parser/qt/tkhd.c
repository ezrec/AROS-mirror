#include "quicktime.h"

int quicktime_tkhd_init(quicktime_tkhd_t *tkhd)
{
     int i;
     tkhd->version = 0;
     tkhd->flags = 15;
     tkhd->creation_time = quicktime_current_time();
     tkhd->modification_time = quicktime_current_time();
     tkhd->track_id = 0;
     tkhd->reserved1 = 0;
     tkhd->duration = 0;      /* need to set this when closing */
     for(i = 0; i < 8; i++) tkhd->reserved2[i] = 0;
     tkhd->layer = 0;
     tkhd->alternate_group = 0;
     tkhd->volume = 0.996094;
     tkhd->reserved3 = 0;
     quicktime_matrix_init(&(tkhd->matrix));
     tkhd->track_width = 0;
     tkhd->track_height = 0;
     return 0;
}

int quicktime_tkhd_delete(quicktime_tkhd_t *tkhd)
{
     return 0;
}

void quicktime_read_tkhd(quicktime_t *file, quicktime_tkhd_t *tkhd)
{
     tkhd->version = quicktime_read_char(file);
     tkhd->flags = quicktime_read_int24(file);
     tkhd->creation_time = quicktime_read_int32(file);
     tkhd->modification_time = quicktime_read_int32(file);
     tkhd->track_id = quicktime_read_int32(file);
     tkhd->reserved1 = quicktime_read_int32(file);
     tkhd->duration = quicktime_read_int32(file);
     quicktime_read_data(file, tkhd->reserved2, 8);
     tkhd->layer = quicktime_read_int16(file);
     tkhd->alternate_group = quicktime_read_int16(file);
     tkhd->volume = quicktime_read_fixed16(file);
     tkhd->reserved3 = quicktime_read_int16(file);
     quicktime_read_matrix(file, &(tkhd->matrix));
     tkhd->track_width = quicktime_read_fixed32(file);
     tkhd->track_height = quicktime_read_fixed32(file);
}

void quicktime_tkhd_init_video(quicktime_t *file, 
                                        quicktime_tkhd_t *tkhd, 
                                        int frame_w, 
                                        int frame_h)
{
     tkhd->track_width = frame_w;
     tkhd->track_height = frame_h;
     tkhd->volume = 0;
}
