#include "quicktime.h"

void quicktime_elst_table_init(quicktime_elst_table_t *table)
{
     table->duration = 0;
     table->time = 0;
     table->rate = 1;
}

void quicktime_elst_table_delete(quicktime_elst_table_t *table)
{
}

void quicktime_read_elst_table(quicktime_t *file, quicktime_elst_table_t *table)
{
     table->duration = quicktime_read_int32(file);
     table->time = quicktime_read_int32(file);
     table->rate = quicktime_read_fixed32(file);
}

void quicktime_elst_init(quicktime_elst_t *elst)
{
     elst->version = 0;
     elst->flags = 0;
     elst->total_entries = 0;
     elst->table = 0;
}

void quicktime_elst_init_all(quicktime_elst_t *elst)
{
     if(!elst->total_entries)
     {
          elst->total_entries = 1;
          elst->table = (quicktime_elst_table_t*)malloc(sizeof(quicktime_elst_table_t) * elst->total_entries);
          quicktime_elst_table_init(&(elst->table[0]));
     }
}

void quicktime_elst_delete(quicktime_elst_t *elst)
{
     int i;
     if(elst->total_entries)
     {
          for(i = 0; i < elst->total_entries; i++)
               quicktime_elst_table_delete(&(elst->table[i]));
          free(elst->table);
     }
     elst->total_entries = 0;
}

void quicktime_read_elst(quicktime_t *file, quicktime_elst_t *elst)
{
     int i;
//     quicktime_atom_t leaf_atom;

     elst->version = quicktime_read_char(file);
     elst->flags = quicktime_read_int24(file);
     elst->total_entries = quicktime_read_int32(file);
     elst->table = (quicktime_elst_table_t*)calloc(1, sizeof(quicktime_elst_table_t) * elst->total_entries);
     for(i = 0; i < elst->total_entries; i++)
     {
          quicktime_elst_table_init(&(elst->table[i]));
          quicktime_read_elst_table(file, &(elst->table[i]));
     }
}

