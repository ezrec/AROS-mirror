#include "quicktime.h"

void quicktime_stsc_init(quicktime_stsc_t *stsc)
{
     stsc->version = 0;
     stsc->flags = 0;
     stsc->total_entries = 0;
     stsc->entries_allocated = 0;
}

void quicktime_stsc_init_table(quicktime_t *file, quicktime_stsc_t *stsc)
{
     if(!stsc->entries_allocated)
     {
          stsc->total_entries = 0;
          stsc->entries_allocated = 2000;
          stsc->table = (quicktime_stsc_table_t*)calloc(1, sizeof(quicktime_stsc_table_t) * stsc->entries_allocated);
     }
}

void quicktime_stsc_init_video(quicktime_t *file, quicktime_stsc_t *stsc)
{
     quicktime_stsc_table_t *table;
     quicktime_stsc_init_table(file, stsc);
     table = &(stsc->table[0]);
     table->chunk = 1;
     table->samples = 1;
     table->id = 1;
}

void quicktime_stsc_init_audio(quicktime_t *file, quicktime_stsc_t *stsc, int sample_rate)
{
     quicktime_stsc_table_t *table;
     quicktime_stsc_init_table(file, stsc);
     table = &(stsc->table[0]);
     table->chunk = 1;
     table->samples = 0;         /* set this after completion or after every audio chunk is written */
     table->id = 1;
}

void quicktime_stsc_delete(quicktime_stsc_t *stsc)
{
     if(stsc->total_entries) free(stsc->table);
     stsc->total_entries = 0;
}

void quicktime_read_stsc(quicktime_t *file, quicktime_stsc_t *stsc)
{
     int i;
     stsc->version = quicktime_read_char(file);
     stsc->flags = quicktime_read_int24(file);
     stsc->total_entries = quicktime_read_int32(file);
     
     stsc->entries_allocated = stsc->total_entries;
     stsc->table = (quicktime_stsc_table_t*)malloc(sizeof(quicktime_stsc_table_t) * stsc->total_entries);
     for(i = 0; i < stsc->total_entries; i++)
     {
          stsc->table[i].chunk = quicktime_read_int32(file);
          stsc->table[i].samples = quicktime_read_int32(file);
          stsc->table[i].id = quicktime_read_int32(file);
     }
}

int quicktime_update_stsc(quicktime_stsc_t *stsc, long chunk, long samples)
{
     if(chunk > stsc->entries_allocated)
     {
          stsc->entries_allocated = chunk * 2;
          stsc->table =(quicktime_stsc_table_t*)realloc(stsc->table, sizeof(quicktime_stsc_table_t) * stsc->entries_allocated);
     }

     stsc->table[chunk - 1].samples = samples;
     stsc->table[chunk - 1].chunk = chunk;
     stsc->table[chunk - 1].id = 1;
     if(chunk > stsc->total_entries) stsc->total_entries = chunk;
     return 0;
}

/* Optimizing while writing doesn't allow seeks during recording so */
/* entries are created for every chunk and only optimized during */
/* writeout.  Unfortunately there's no way to keep audio synchronized */
/* after overwriting  a recording as the fractional audio chunk in the */
/* middle always overwrites the previous location of a larger chunk.  On */
/* writing, the table must be optimized.  RealProducer requires an  */
/* optimized table. */

