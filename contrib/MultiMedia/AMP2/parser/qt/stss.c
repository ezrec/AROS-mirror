#include "quicktime.h"

void quicktime_stss_init(quicktime_stss_t *stss)
{
     stss->version = 0;
     stss->flags = 0;
     stss->total_entries = 0;
     stss->entries_allocated = 2;
     stss->table = (quicktime_stss_table_t*)calloc(1, sizeof(quicktime_stss_table_t) * stss->entries_allocated);
}

void quicktime_stss_delete(quicktime_stss_t *stss)
{
     if(stss->entries_allocated) free(stss->table);
     stss->total_entries = 0;
     stss->entries_allocated = 0;
     stss->table = 0;
}

void quicktime_read_stss(quicktime_t *file, quicktime_stss_t *stss)
{
     int i;
     stss->version = quicktime_read_char(file);
     stss->flags = quicktime_read_int24(file);
     stss->total_entries = quicktime_read_int32(file);

     if(stss->entries_allocated < stss->total_entries)
     {
          stss->entries_allocated = stss->total_entries;
          stss->table = (quicktime_stss_table_t*)realloc(stss->table, sizeof(quicktime_stss_table_t) * stss->entries_allocated);
     }

     for(i = 0; i < stss->total_entries; i++)
     {
          stss->table[i].sample = quicktime_read_int32(file);
     }
}

