#include "quicktime.h"

void quicktime_minf_init(quicktime_minf_t *minf)
{
     minf->is_video = minf->is_audio = 0;
     quicktime_vmhd_init(&(minf->vmhd));
     quicktime_smhd_init(&(minf->smhd));
     quicktime_hdlr_init(&(minf->hdlr));
     quicktime_dinf_init(&(minf->dinf));
     quicktime_stbl_init(&(minf->stbl));
}

void quicktime_minf_init_video(quicktime_t *file, 
                                        quicktime_minf_t *minf, 
                                        int frame_w,
                                        int frame_h, 
                                        int time_scale, 
                                        float frame_rate,
                                        char *compressor)
{
     minf->is_video = 1;
     quicktime_vmhd_init_video(file, &(minf->vmhd), frame_w, frame_h, frame_rate);
     quicktime_stbl_init_video(file, &(minf->stbl), frame_w, frame_h, time_scale, frame_rate, compressor);
     quicktime_hdlr_init_data(&(minf->hdlr));
     quicktime_dinf_init_all(&(minf->dinf));
}

void quicktime_minf_init_audio(quicktime_t *file, 
                                   quicktime_minf_t *minf, 
                                   int channels, 
                                   int sample_rate, 
                                   int bits, 
                                   char *compressor)
{
     minf->is_audio = 1;
/* smhd doesn't store anything worth initializing */
     quicktime_stbl_init_audio(file, &(minf->stbl), channels, sample_rate, bits, compressor);
     quicktime_hdlr_init_data(&(minf->hdlr));
     quicktime_dinf_init_all(&(minf->dinf));
}

void quicktime_minf_delete(quicktime_minf_t *minf)
{
     quicktime_vmhd_delete(&(minf->vmhd));
     quicktime_smhd_delete(&(minf->smhd));
     quicktime_dinf_delete(&(minf->dinf));
     quicktime_stbl_delete(&(minf->stbl));
     quicktime_hdlr_delete(&(minf->hdlr));
}

int quicktime_read_minf(quicktime_t *file, quicktime_minf_t *minf, quicktime_atom_t *parent_atom)
{
     quicktime_atom_t leaf_atom;

     do
     {
          quicktime_atom_read_header(file, &leaf_atom);

/* mandatory */
          if(quicktime_atom_is(&leaf_atom, "vmhd"))
               { minf->is_video = 1; quicktime_read_vmhd(file, &(minf->vmhd)); }
          else
          if(quicktime_atom_is(&leaf_atom, "smhd"))
               { minf->is_audio = 1; quicktime_read_smhd(file, &(minf->smhd)); }
          else
          if(quicktime_atom_is(&leaf_atom, "hdlr"))
               { 
                    quicktime_read_hdlr(file, &(minf->hdlr)); 
/* Main Actor doesn't write component name */
                    quicktime_atom_skip(file, &leaf_atom);
               }
          else
          if(quicktime_atom_is(&leaf_atom, "dinf"))
               { quicktime_read_dinf(file, &(minf->dinf), &leaf_atom); }
          else
          if(quicktime_atom_is(&leaf_atom, "stbl"))
               { quicktime_read_stbl(file, minf, &(minf->stbl), &leaf_atom); }
          else
               quicktime_atom_skip(file, &leaf_atom);
     }while(quicktime_position(file) < parent_atom->end);

     return 0;
}

