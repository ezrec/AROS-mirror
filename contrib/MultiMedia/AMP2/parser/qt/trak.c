#include "quicktime.h"

int quicktime_trak_init(quicktime_trak_t *trak)
{
     quicktime_tkhd_init(&(trak->tkhd));
     quicktime_edts_init(&(trak->edts));
     quicktime_mdia_init(&(trak->mdia));
     return 0;
}

int quicktime_trak_init_video(quicktime_t *file, 
                                   quicktime_trak_t *trak, 
                                   int frame_w, 
                                   int frame_h, 
                                   float frame_rate,
                                   char *compressor)
{
     quicktime_tkhd_init_video(file, 
          &(trak->tkhd), 
          frame_w, 
          frame_h);
     quicktime_mdia_init_video(file, 
          &(trak->mdia), 
          frame_w, 
          frame_h, 
          frame_rate, 
          compressor);
     quicktime_edts_init_table(&(trak->edts));

     return 0;
}

int quicktime_trak_init_audio(quicktime_t *file, 
                                   quicktime_trak_t *trak, 
                                   int channels, 
                                   int sample_rate, 
                                   int bits, 
                                   char *compressor)
{
     quicktime_mdia_init_audio(file, &(trak->mdia), channels, sample_rate, bits, compressor);
     quicktime_edts_init_table(&(trak->edts));

     return 0;
}

int quicktime_trak_delete(quicktime_trak_t *trak)
{
     quicktime_tkhd_delete(&(trak->tkhd));
     return 0;
}

quicktime_trak_t* quicktime_add_trak(quicktime_moov_t *moov)
{
     if(moov->total_tracks < MAXTRACKS)
     {
          moov->trak[moov->total_tracks] = malloc(sizeof(quicktime_trak_t));
          quicktime_trak_init(moov->trak[moov->total_tracks]);
          moov->total_tracks++;
     }
     return moov->trak[moov->total_tracks - 1];
}

int quicktime_delete_trak(quicktime_moov_t *moov)
{
     if(moov->total_tracks)
     {
          moov->total_tracks--;
          quicktime_trak_delete(moov->trak[moov->total_tracks]);
          free(moov->trak[moov->total_tracks]);
     }
     return 0;
}


int quicktime_read_trak(quicktime_t *file, quicktime_trak_t *trak, quicktime_atom_t *trak_atom)
{
     quicktime_atom_t leaf_atom;

     do
     {
          quicktime_atom_read_header(file, &leaf_atom);
/* mandatory */
          if(quicktime_atom_is(&leaf_atom, "tkhd"))
               { quicktime_read_tkhd(file, &(trak->tkhd)); }
          else
          if(quicktime_atom_is(&leaf_atom, "mdia"))
               { quicktime_read_mdia(file, &(trak->mdia), &leaf_atom); }
          else
/* optional */
          if(quicktime_atom_is(&leaf_atom, "clip"))
               { quicktime_atom_skip(file, &leaf_atom); }
          else
          if(quicktime_atom_is(&leaf_atom, "matt"))
               { quicktime_atom_skip(file, &leaf_atom); }
          else
          if(quicktime_atom_is(&leaf_atom, "edts"))
               { quicktime_read_edts(file, &(trak->edts), &leaf_atom); }
          else
          if(quicktime_atom_is(&leaf_atom, "load"))
               { quicktime_atom_skip(file, &leaf_atom); }
          else
          if(quicktime_atom_is(&leaf_atom, "tref"))
               { quicktime_atom_skip(file, &leaf_atom); }
          else
          if(quicktime_atom_is(&leaf_atom, "imap"))
               { quicktime_atom_skip(file, &leaf_atom); }
          else
          if(quicktime_atom_is(&leaf_atom, "udta"))
               { quicktime_atom_skip(file, &leaf_atom); }
          else
               quicktime_atom_skip(file, &leaf_atom);
     }while(quicktime_position(file) < trak_atom->end);

     return 0;
}

longest quicktime_track_end(quicktime_trak_t *trak)
{
/* get the byte endpoint of the track in the file */
     longest size = 0;
     longest chunk, chunk_offset, chunk_samples, sample;
     quicktime_stsz_t *stsz = &(trak->mdia.minf.stbl.stsz);
//     quicktime_stsz_table_t *table = stsz->table;
     quicktime_stsc_t *stsc = &(trak->mdia.minf.stbl.stsc);
     quicktime_stco_t *stco;

/* get the last chunk offset */
/* the chunk offsets contain the HEADER_LENGTH themselves */
     stco = &(trak->mdia.minf.stbl.stco);
     chunk = stco->total_entries;
     size = chunk_offset = stco->table[chunk - 1].offset;

/* get the number of samples in the last chunk */
     chunk_samples = stsc->table[stsc->total_entries - 1].samples;

/* get the size of last samples */
     if(stsz->sample_size)
     {
/* assume audio so calculate the sample size */
          size += chunk_samples * stsz->sample_size
               * trak->mdia.minf.stbl.stsd.table[0].channels 
               * trak->mdia.minf.stbl.stsd.table[0].sample_size / 8;
     }
     else
     {
/* assume video */
          for(sample = stsz->total_entries - chunk_samples; 
               sample < stsz->total_entries; sample++)
          {
               size += stsz->table[sample].size;
          }
     }

     return size;
}

long quicktime_track_samples(quicktime_t *file, quicktime_trak_t *trak)
{
/* get the sample count when reading only */
     quicktime_stts_t *stts = &(trak->mdia.minf.stbl.stts);
     int i;
     long total = 0;

     for(i = 0; i < stts->total_entries; i++)
     {
          total += stts->table[i].sample_count;
     }
     return total;
}

long quicktime_sample_of_chunk(quicktime_trak_t *trak, long chunk)
{
     quicktime_stsc_table_t *table = trak->mdia.minf.stbl.stsc.table;
     long total_entries = trak->mdia.minf.stbl.stsc.total_entries;
     long chunk1entry, chunk2entry;
     long chunk1, chunk2, chunks, total = 0;

     for(chunk1entry = total_entries - 1, chunk2entry = total_entries; 
          chunk1entry >= 0; 
          chunk1entry--, chunk2entry--)
     {
          chunk1 = table[chunk1entry].chunk;

          if(chunk > chunk1)
          {
               if(chunk2entry < total_entries)
               {
                    chunk2 = table[chunk2entry].chunk;

                    if(chunk < chunk2) chunk2 = chunk;
               }
               else
                    chunk2 = chunk;

               chunks = chunk2 - chunk1;

               total += chunks * table[chunk1entry].samples;
          }
     }

     return total;
}

int quicktime_chunk_of_sample(longest *chunk_sample, 
     longest *chunk, 
     quicktime_trak_t *trak, 
     long sample)
{
     quicktime_stsc_table_t *table = trak->mdia.minf.stbl.stsc.table;
     long total_entries = trak->mdia.minf.stbl.stsc.total_entries;
     long chunk2entry;
     long chunk1, chunk2, chunk1samples, range_samples, total = 0;

     chunk1 = 1;
     chunk1samples = 0;
     chunk2entry = 0;

     if(!total_entries)
     {
          *chunk_sample = 0;
          *chunk = 0;
          return 0;
     }

     do
     {
          chunk2 = table[chunk2entry].chunk;
          *chunk = chunk2 - chunk1;
          range_samples = *chunk * chunk1samples;

          if(sample < total + range_samples) break;

          chunk1samples = table[chunk2entry].samples;
          chunk1 = chunk2;

          if(chunk2entry < total_entries)
          {
               chunk2entry++;
               total += range_samples;
          }
     }while(chunk2entry < total_entries);

     if(chunk1samples)
          *chunk = (sample - total) / chunk1samples + chunk1;
     else
          *chunk = 1;

     *chunk_sample = total + (*chunk - chunk1) * chunk1samples;
     return 0;
}

longest quicktime_chunk_to_offset(quicktime_trak_t *trak, long chunk)
{
     quicktime_stco_table_t *table = trak->mdia.minf.stbl.stco.table;

     if(trak->mdia.minf.stbl.stco.total_entries && 
          chunk > trak->mdia.minf.stbl.stco.total_entries)
          return table[trak->mdia.minf.stbl.stco.total_entries - 1].offset;
     else
     if(trak->mdia.minf.stbl.stco.total_entries)
          return table[chunk - 1].offset;
     else
          return HEADER_LENGTH * 2;

     return 0;
}

long quicktime_offset_to_chunk(longest *chunk_offset, 
     quicktime_trak_t *trak, 
     longest offset)
{
     quicktime_stco_table_t *table = trak->mdia.minf.stbl.stco.table;
     int i;

     for(i = trak->mdia.minf.stbl.stco.total_entries - 1; i >= 0; i--)
     {
          if(table[i].offset <= offset)
          {
               *chunk_offset = table[i].offset;
               return i + 1;
          }
     }
     *chunk_offset = HEADER_LENGTH * 2;
     return 1;
}

longest quicktime_sample_range_size(quicktime_trak_t *trak, 
     long chunk_sample, 
     long sample)
{
//     quicktime_stsz_table_t *table = trak->mdia.minf.stbl.stsz.table;
     longest i, total;

     if(trak->mdia.minf.stbl.stsz.sample_size)
     {
/* assume audio */
          return quicktime_samples_to_bytes(trak, sample - chunk_sample);
/*        return (sample - chunk_sample) * trak->mdia.minf.stbl.stsz.sample_size  */
/*             * trak->mdia.minf.stbl.stsd.table[0].channels  */
/*             * trak->mdia.minf.stbl.stsd.table[0].sample_size / 8; */
     }
     else
     {
/* probably video */
          for(i = chunk_sample, total = 0; i < sample; i++)
          {
               total += trak->mdia.minf.stbl.stsz.table[i].size;
          }
     }
     return total;
}

longest quicktime_sample_to_offset(quicktime_trak_t *trak, long sample)
{
     longest chunk, chunk_sample, chunk_offset1, chunk_offset2;

     quicktime_chunk_of_sample(&chunk_sample, &chunk, trak, sample);
     chunk_offset1 = quicktime_chunk_to_offset(trak, chunk);
     chunk_offset2 = chunk_offset1 + quicktime_sample_range_size(trak, chunk_sample, sample);
     return chunk_offset2;
}

long quicktime_offset_to_sample(quicktime_trak_t *trak, longest offset)
{
     longest chunk_offset;
     longest chunk = quicktime_offset_to_chunk(&chunk_offset, trak, offset);
     longest chunk_sample = quicktime_sample_of_chunk(trak, chunk);
     longest sample, sample_offset;
     quicktime_stsz_table_t *table = trak->mdia.minf.stbl.stsz.table;
     longest total_samples = trak->mdia.minf.stbl.stsz.total_entries;

     if(trak->mdia.minf.stbl.stsz.sample_size)
     {
          sample = chunk_sample + (offset - chunk_offset) / 
               trak->mdia.minf.stbl.stsz.sample_size;
     }
     else
     for(sample = chunk_sample, sample_offset = chunk_offset; 
          sample_offset < offset && sample < total_samples; )
     {
          sample_offset += table[sample].size;
          if(sample_offset < offset) sample++;
     }
     
     return sample;
}

int quicktime_update_tables(quicktime_t *file, 
                                   quicktime_trak_t *trak, 
                                   longest offset, 
                                   longest chunk, 
                                   longest sample, 
                                   longest samples, 
                                   longest sample_size)
{
     if(offset + sample_size > file->mdat.atom.size) 
          file->mdat.atom.size = offset + sample_size;

     quicktime_update_stco(&(trak->mdia.minf.stbl.stco), chunk, offset);

     if(sample_size) 
          quicktime_update_stsz(&(trak->mdia.minf.stbl.stsz), sample, sample_size);

     quicktime_update_stsc(&(trak->mdia.minf.stbl.stsc), chunk, samples);
     return 0;
}

int quicktime_trak_duration(quicktime_trak_t *trak, 
     long *duration, 
     long *timescale)
{
     quicktime_stts_t *stts = &(trak->mdia.minf.stbl.stts);
     int i;
     *duration = 0;

     for(i = 0; i < stts->total_entries; i++)
     {
          *duration += stts->table[i].sample_duration * stts->table[i].sample_count;
     }

     *timescale = trak->mdia.mdhd.time_scale;
     return 0;
}

int quicktime_trak_fix_counts(quicktime_t *file, quicktime_trak_t *trak)
{
     long samples = quicktime_track_samples(file, trak);

     trak->mdia.minf.stbl.stts.table[0].sample_count = samples;

     if(!trak->mdia.minf.stbl.stsz.total_entries)
     {
          trak->mdia.minf.stbl.stsz.sample_size = 1;
          trak->mdia.minf.stbl.stsz.total_entries = samples;
     }

     return 0;
}

long quicktime_chunk_samples(quicktime_trak_t *trak, long chunk)
{
     long result, current_chunk;
     quicktime_stsc_t *stsc = &(trak->mdia.minf.stbl.stsc);
     long i = stsc->total_entries - 1;

     do
     {
          current_chunk = stsc->table[i].chunk;
          result = stsc->table[i].samples;
          i--;
     }while(i >= 0 && current_chunk > chunk);

     return result;
}

int quicktime_trak_shift_offsets(quicktime_trak_t *trak, longest offset)
{
     quicktime_stco_t *stco = &(trak->mdia.minf.stbl.stco);
     int i;

     for(i = 0; i < stco->total_entries; i++)
     {
          stco->table[i].offset += offset;
     }
     return 0;
}
