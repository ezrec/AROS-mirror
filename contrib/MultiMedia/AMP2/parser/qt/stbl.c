#include "quicktime.h"

void quicktime_stbl_init(quicktime_stbl_t *stbl)
{
     stbl->version = 0;
     stbl->flags = 0;
     quicktime_stsd_init(&(stbl->stsd));
     quicktime_stts_init(&(stbl->stts));
     quicktime_stss_init(&(stbl->stss));
     quicktime_stsc_init(&(stbl->stsc));
     quicktime_stsz_init(&(stbl->stsz));
     quicktime_stco_init(&(stbl->stco));
}

void quicktime_stbl_init_video(quicktime_t *file, 
                                        quicktime_stbl_t *stbl, 
                                        int frame_w,
                                        int frame_h, 
                                        int time_scale, 
                                        float frame_rate,
                                        char *compressor)
{
     quicktime_stsd_init_video(file, &(stbl->stsd), frame_w, frame_h, frame_rate, compressor);
     quicktime_stts_init_video(file, &(stbl->stts), time_scale, frame_rate);
     quicktime_stsc_init_video(file, &(stbl->stsc));
     quicktime_stsz_init_video(file, &(stbl->stsz));
     quicktime_stco_init_common(file, &(stbl->stco));
}


void quicktime_stbl_init_audio(quicktime_t *file, 
                                   quicktime_stbl_t *stbl, 
                                   int channels, 
                                   int sample_rate, 
                                   int bits, 
                                   char *compressor)
{
     quicktime_stsd_init_audio(file, &(stbl->stsd), 
          channels, 
          sample_rate, 
          bits, 
          compressor);
     quicktime_stts_init_audio(file, &(stbl->stts), sample_rate);
     quicktime_stsc_init_audio(file, &(stbl->stsc), sample_rate);
     quicktime_stsz_init_audio(file, &(stbl->stsz), channels, bits, compressor);
     quicktime_stco_init_common(file, &(stbl->stco));
}

void quicktime_stbl_delete(quicktime_stbl_t *stbl)
{
     quicktime_stsd_delete(&(stbl->stsd));
     quicktime_stts_delete(&(stbl->stts));
     quicktime_stss_delete(&(stbl->stss));
     quicktime_stsc_delete(&(stbl->stsc));
     quicktime_stsz_delete(&(stbl->stsz));
     quicktime_stco_delete(&(stbl->stco));
}

int quicktime_read_stbl(quicktime_t *file, quicktime_minf_t *minf, quicktime_stbl_t *stbl, quicktime_atom_t *parent_atom)
{
     quicktime_atom_t leaf_atom;

     do
     {
          quicktime_atom_read_header(file, &leaf_atom);

/* mandatory */
          if(quicktime_atom_is(&leaf_atom, "stsd"))
          { 
               quicktime_read_stsd(file, minf, &(stbl->stsd)); 
/* Some codecs store extra information at the end of this */
               quicktime_atom_skip(file, &leaf_atom);
          }
          else
          if(quicktime_atom_is(&leaf_atom, "stts"))
               { quicktime_read_stts(file, &(stbl->stts)); }
          else
          if(quicktime_atom_is(&leaf_atom, "stss"))
               { quicktime_read_stss(file, &(stbl->stss)); }
          else
          if(quicktime_atom_is(&leaf_atom, "stsc"))
               { quicktime_read_stsc(file, &(stbl->stsc)); }
          else
          if(quicktime_atom_is(&leaf_atom, "stsz"))
               { quicktime_read_stsz(file, &(stbl->stsz)); }
          else
          if(quicktime_atom_is(&leaf_atom, "co64"))
               { quicktime_read_stco64(file, &(stbl->stco)); }
          else
          if(quicktime_atom_is(&leaf_atom, "stco"))
               { quicktime_read_stco(file, &(stbl->stco)); }
          else
               quicktime_atom_skip(file, &leaf_atom);
     }while(quicktime_position(file) < parent_atom->end);

     return 0;
}

