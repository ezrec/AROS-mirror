#include "quicktime.h"

int quicktime_mvhd_init(quicktime_mvhd_t *mvhd)
{
     int i;
     mvhd->version = 0;
     mvhd->flags = 0;
     mvhd->creation_time = quicktime_current_time();
     mvhd->modification_time = quicktime_current_time();
     mvhd->time_scale = 600;
     mvhd->duration = 0;
     mvhd->preferred_rate = 1.0;
     mvhd->preferred_volume = 0.996094;
     for(i = 0; i < 10; i++) mvhd->reserved[i] = 0;
     quicktime_matrix_init(&(mvhd->matrix));
     mvhd->preview_time = 0;
     mvhd->preview_duration = 0;
     mvhd->poster_time = 0;
     mvhd->selection_time = 0;
     mvhd->selection_duration = 0;
     mvhd->current_time = 0;
     mvhd->next_track_id = 1;
     return 0;
}

int quicktime_mvhd_delete(quicktime_mvhd_t *mvhd)
{
     return 0;
}

void quicktime_read_mvhd(quicktime_t *file, quicktime_mvhd_t *mvhd)
{
     mvhd->version = quicktime_read_char(file);
     mvhd->flags = quicktime_read_int24(file);
     mvhd->creation_time = quicktime_read_int32(file);
     mvhd->modification_time = quicktime_read_int32(file);
     mvhd->time_scale = quicktime_read_int32(file);
     mvhd->duration = quicktime_read_int32(file);
     mvhd->preferred_rate = quicktime_read_fixed32(file);
     mvhd->preferred_volume = quicktime_read_fixed16(file);
     quicktime_read_data(file, mvhd->reserved, 10);
     quicktime_read_matrix(file, &(mvhd->matrix));
     mvhd->preview_time = quicktime_read_int32(file);
     mvhd->preview_duration = quicktime_read_int32(file);
     mvhd->poster_time = quicktime_read_int32(file);
     mvhd->selection_time = quicktime_read_int32(file);
     mvhd->selection_duration = quicktime_read_int32(file);
     mvhd->current_time = quicktime_read_int32(file);
     mvhd->next_track_id = quicktime_read_int32(file);
}

void quicktime_mhvd_init_video(quicktime_t *file, quicktime_mvhd_t *mvhd, float frame_rate)
{
     mvhd->time_scale = quicktime_get_timescale(frame_rate);
}

