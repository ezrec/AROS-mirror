#include "quicktime.h"

void quicktime_stts_init(quicktime_stts_t *stts)
{
     stts->version = 0;
     stts->flags = 0;
     stts->total_entries = 0;
}

void quicktime_stts_init_table(quicktime_stts_t *stts)
{
     if(!stts->total_entries)
     {
          stts->total_entries = 1;
          stts->table = (quicktime_stts_table_t*)malloc(sizeof(quicktime_stts_table_t) * stts->total_entries);
     }
}

void quicktime_stts_init_video(quicktime_t *file, quicktime_stts_t *stts, int time_scale, float frame_rate)
{
     quicktime_stts_table_t *table;
     quicktime_stts_init_table(stts);
     table = &(stts->table[0]);

     table->sample_count = 0;      /* need to set this when closing */
     table->sample_duration = time_scale / frame_rate;
}

void quicktime_stts_init_audio(quicktime_t *file, quicktime_stts_t *stts, int sample_rate)
{
     quicktime_stts_table_t *table;
     quicktime_stts_init_table(stts);
     table = &(stts->table[0]);

     table->sample_count = 0;     /* need to set this when closing */
     table->sample_duration = 1;
}

void quicktime_stts_delete(quicktime_stts_t *stts)
{
     if(stts->total_entries) free(stts->table);
     stts->total_entries = 0;
}

void quicktime_read_stts(quicktime_t *file, quicktime_stts_t *stts)
{
     int i;
     stts->version = quicktime_read_char(file);
     stts->flags = quicktime_read_int24(file);
     stts->total_entries = quicktime_read_int32(file);

     stts->table = (quicktime_stts_table_t*)malloc(sizeof(quicktime_stts_table_t) * stts->total_entries);
     for(i = 0; i < stts->total_entries; i++)
     {
          stts->table[i].sample_count = quicktime_read_int32(file);
          stts->table[i].sample_duration = quicktime_read_int32(file);
     }
}

