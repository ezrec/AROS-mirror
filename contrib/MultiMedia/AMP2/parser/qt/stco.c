#include "quicktime.h"

void quicktime_stco_init(quicktime_stco_t *stco)
{
     stco->version = 0;
     stco->flags = 0;
     stco->total_entries = 0;
     stco->entries_allocated = 0;
}

void quicktime_stco_delete(quicktime_stco_t *stco)
{
     if(stco->total_entries) free(stco->table);
     stco->total_entries = 0;
     stco->entries_allocated = 0;
}

void quicktime_stco_init_common(quicktime_t *file, quicktime_stco_t *stco)
{
     if(!stco->entries_allocated)
     {
          stco->entries_allocated = 2000;
          stco->total_entries = 0;
          stco->table = (quicktime_stco_table_t*)malloc(sizeof(quicktime_stco_table_t) * stco->entries_allocated);
     }
}

void quicktime_read_stco(quicktime_t *file, quicktime_stco_t *stco)
{
     int i;
     stco->version = quicktime_read_char(file);
     stco->flags = quicktime_read_int24(file);
     stco->total_entries = quicktime_read_int32(file);
     stco->entries_allocated = stco->total_entries;
     stco->table = (quicktime_stco_table_t*)calloc(1, sizeof(quicktime_stco_table_t) * stco->entries_allocated);
     for(i = 0; i < stco->total_entries; i++)
     {
          stco->table[i].offset = quicktime_read_uint32(file);
     }
}

void quicktime_read_stco64(quicktime_t *file, quicktime_stco_t *stco)
{
     int i;
     stco->version = quicktime_read_char(file);
     stco->flags = quicktime_read_int24(file);
     stco->total_entries = quicktime_read_int32(file);
     stco->entries_allocated = stco->total_entries;
     stco->table = (quicktime_stco_table_t*)calloc(1, sizeof(quicktime_stco_table_t) * stco->entries_allocated);
     for(i = 0; i < stco->total_entries; i++)
     {
          stco->table[i].offset = quicktime_read_int64(file);
     }
}

void quicktime_update_stco(quicktime_stco_t *stco, long chunk, longest offset)
{
     if(chunk > stco->entries_allocated)
     {
          stco->entries_allocated = chunk * 2;
          stco->table = (quicktime_stco_table_t*)realloc(stco->table, sizeof(quicktime_stco_table_t) * stco->entries_allocated);
     }
     
     stco->table[chunk - 1].offset = offset;
     if(chunk > stco->total_entries) stco->total_entries = chunk;
}

