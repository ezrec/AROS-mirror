#include "quicktime.h"

void quicktime_mdhd_init(quicktime_mdhd_t *mdhd)
{
     mdhd->version = 0;
     mdhd->flags = 0;
     mdhd->creation_time = quicktime_current_time();
     mdhd->modification_time = quicktime_current_time();
     mdhd->time_scale = 0;
     mdhd->duration = 0;
     mdhd->language = 0;
     mdhd->quality = 100;
}

void quicktime_mdhd_init_video(quicktime_t *file, 
                                        quicktime_mdhd_t *mdhd, 
                                        int frame_w,
                                        int frame_h, 
                                        float frame_rate)
{
     mdhd->time_scale = quicktime_get_timescale(frame_rate);
     mdhd->duration = 0;      /* set this when closing */
}

void quicktime_mdhd_init_audio(quicktime_t *file, 
                                   quicktime_mdhd_t *mdhd, 
                                   int channels, 
                                   int sample_rate, 
                                   int bits, 
                                   char *compressor)
{
     mdhd->time_scale = sample_rate;
     mdhd->duration = 0;      /* set this when closing */
}

void quicktime_mdhd_delete(quicktime_mdhd_t *mdhd)
{
}

void quicktime_read_mdhd(quicktime_t *file, quicktime_mdhd_t *mdhd)
{
     mdhd->version = quicktime_read_char(file);
     mdhd->flags = quicktime_read_int24(file);
     mdhd->creation_time = quicktime_read_int32(file);
     mdhd->modification_time = quicktime_read_int32(file);
     mdhd->time_scale = quicktime_read_int32(file);
     mdhd->duration = quicktime_read_int32(file);
     mdhd->language = quicktime_read_int16(file);
     mdhd->quality = quicktime_read_int16(file);
}

