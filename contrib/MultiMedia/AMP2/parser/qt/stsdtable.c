#include "quicktime.h"

void quicktime_mjqt_init(quicktime_mjqt_t *mjqt)
{
}

void quicktime_mjqt_delete(quicktime_mjqt_t *mjqt)
{
}

void quicktime_mjht_init(quicktime_mjht_t *mjht)
{
}

void quicktime_mjht_delete(quicktime_mjht_t *mjht)
{
}

void quicktime_read_stsd_audio(quicktime_t *file, quicktime_stsd_table_t *table, quicktime_atom_t *parent_atom)
{
     table->version = quicktime_read_int16(file);
     table->revision = quicktime_read_int16(file);
     quicktime_read_data(file, table->vendor, 4);
     table->channels = quicktime_read_int16(file);
     table->sample_size = quicktime_read_int16(file);
     table->compression_id = quicktime_read_int16(file);
     table->packet_size = quicktime_read_int16(file);
     table->sample_rate = quicktime_read_fixed32(file);
// Kluge for fixed32 limitation
if(table->sample_rate + 65536 == 96000) table->sample_rate += 65536;

}

void quicktime_read_stsd_video(quicktime_t *file, quicktime_stsd_table_t *table, quicktime_atom_t *parent_atom)
{
     quicktime_atom_t leaf_atom;
     int len;
     
     table->version = quicktime_read_int16(file);
     table->revision = quicktime_read_int16(file);
     quicktime_read_data(file, table->vendor, 4);
     table->temporal_quality = quicktime_read_int32(file);
     table->spatial_quality = quicktime_read_int32(file);
     table->width = quicktime_read_int16(file);
     table->height = quicktime_read_int16(file);
     table->dpi_horizontal = quicktime_read_fixed32(file);
     table->dpi_vertical = quicktime_read_fixed32(file);
     table->data_size = quicktime_read_int32(file);
     table->frames_per_sample = quicktime_read_int16(file);
     len = quicktime_read_char(file);
     quicktime_read_data(file, table->compressor_name, 31);
     table->depth = quicktime_read_int16(file);
     table->ctab_id = quicktime_read_int16(file);
     
     while(quicktime_position(file) < parent_atom->end)
     {
          quicktime_atom_read_header(file, &leaf_atom);
          
          if(quicktime_atom_is(&leaf_atom, "ctab"))
          {
               quicktime_read_ctab(file, &(table->ctab));
          }
          else
          if(quicktime_atom_is(&leaf_atom, "gama"))
          {
               table->gamma = quicktime_read_fixed32(file);
          }
          else
          if(quicktime_atom_is(&leaf_atom, "fiel"))
          {
               table->fields = quicktime_read_char(file);
               table->field_dominance = quicktime_read_char(file);
          }
          else
/*        if(quicktime_atom_is(&leaf_atom, "mjqt")) */
/*        { */
/*             quicktime_read_mjqt(file, &(table->mjqt)); */
/*        } */
/*        else */
/*        if(quicktime_atom_is(&leaf_atom, "mjht")) */
/*        { */
/*             quicktime_read_mjht(file, &(table->mjht)); */
/*        } */
/*        else */
          quicktime_atom_skip(file, &leaf_atom);
     }
}

void quicktime_read_stsd_table(quicktime_t *file, quicktime_minf_t *minf, quicktime_stsd_table_t *table)
{
     quicktime_atom_t leaf_atom;

     quicktime_atom_read_header(file, &leaf_atom);
     
     table->format[0] = leaf_atom.type[0];
     table->format[1] = leaf_atom.type[1];
     table->format[2] = leaf_atom.type[2];
     table->format[3] = leaf_atom.type[3];
     quicktime_read_data(file, table->reserved, 6);
     table->data_reference = quicktime_read_int16(file);

     if(minf->is_audio) quicktime_read_stsd_audio(file, table, &leaf_atom);
     if(minf->is_video) quicktime_read_stsd_video(file, table, &leaf_atom);
}

void quicktime_stsd_table_init(quicktime_stsd_table_t *table)
{
     int i;
     table->format[0] = 'y';
     table->format[1] = 'u';
     table->format[2] = 'v';
     table->format[3] = '2';
     for(i = 0; i < 6; i++) table->reserved[i] = 0;
     table->data_reference = 1;

     table->version = 0;
     table->revision = 0;
     table->vendor[0] = 'l';
     table->vendor[1] = 'n';
     table->vendor[2] = 'u';
     table->vendor[3] = 'x';

     table->temporal_quality = 100;
     table->spatial_quality = 258;
     table->width = 0;
     table->height = 0;
     table->dpi_horizontal = 72;
     table->dpi_vertical = 72;
     table->data_size = 0;
     table->frames_per_sample = 1;
     for(i = 0; i < 32; i++) table->compressor_name[i] = 0;
     table->depth = 24;
     table->ctab_id = 65535;
     quicktime_ctab_init(&(table->ctab));
     table->gamma = 0;
     table->fields = 0;
     table->field_dominance = 1;
     quicktime_mjqt_init(&(table->mjqt));
     quicktime_mjht_init(&(table->mjht));
     
     table->channels = 0;
     table->sample_size = 0;
     table->compression_id = 0;
     table->packet_size = 0;
     table->sample_rate = 0;
}

void quicktime_stsd_table_delete(quicktime_stsd_table_t *table)
{
     quicktime_ctab_delete(&(table->ctab));
     quicktime_mjqt_delete(&(table->mjqt));
     quicktime_mjht_delete(&(table->mjht));
}

