#include "quicktime.h"

void quicktime_stsz_init(quicktime_stsz_t *stsz)
{
     stsz->version = 0;
     stsz->flags = 0;
     stsz->sample_size = 0;
     stsz->total_entries = 0;
     stsz->entries_allocated = 0;
}

void quicktime_stsz_init_video(quicktime_t *file, quicktime_stsz_t *stsz)
{
     stsz->sample_size = 0;
     if(!stsz->entries_allocated)
     {
          stsz->entries_allocated = 2000;
          stsz->total_entries = 0;
          stsz->table = (quicktime_stsz_table_t*)malloc(sizeof(quicktime_stsz_table_t) * stsz->entries_allocated);
     }
}

void quicktime_stsz_init_audio(quicktime_t *file, 
     quicktime_stsz_t *stsz, 
     int channels, 
     int bits,
     char *compressor)
{
     /*stsz->sample_size = channels * bits / 8; */

     stsz->sample_size = 0;

     stsz->total_entries = 0;   /* set this when closing */
     stsz->entries_allocated = 0;
}

void quicktime_stsz_delete(quicktime_stsz_t *stsz)
{
     if(!stsz->sample_size && stsz->total_entries) free(stsz->table);
     stsz->total_entries = 0;
     stsz->entries_allocated = 0;
}

void quicktime_read_stsz(quicktime_t *file, quicktime_stsz_t *stsz)
{
     int i;
     stsz->version = quicktime_read_char(file);
     stsz->flags = quicktime_read_int24(file);
     stsz->sample_size = quicktime_read_int32(file);
     stsz->total_entries = quicktime_read_int32(file);
     stsz->entries_allocated = stsz->total_entries;
     if(!stsz->sample_size)
     {
          stsz->table = (quicktime_stsz_table_t*)malloc(sizeof(quicktime_stsz_table_t) * stsz->entries_allocated);
          for(i = 0; i < stsz->total_entries; i++)
          {
               stsz->table[i].size = quicktime_read_int32(file);
          }
     }
}

void quicktime_update_stsz(quicktime_stsz_t *stsz, 
     long sample, 
     long sample_size)
{
     if(!stsz->sample_size)
     {
          if(sample >= stsz->entries_allocated)
          {
               stsz->entries_allocated = sample * 2;
               stsz->table = (quicktime_stsz_table_t*)realloc(stsz->table, sizeof(quicktime_stsz_table_t) * stsz->entries_allocated);
          }

          stsz->table[sample].size = sample_size;
          if(sample >= stsz->total_entries) stsz->total_entries = sample + 1;
     }
}
