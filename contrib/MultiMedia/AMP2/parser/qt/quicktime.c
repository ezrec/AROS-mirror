#include "quicktime.h"
#include <sys/stat.h>

#if 1

unsigned long samples_to_bytes = 0;

longest quicktime_samples_to_bytes(quicktime_trak_t *track, long samples)
{
  int channels = track->mdia.minf.stbl.stsd.table[0].channels;
  longest val;

  /* This is the idea. I calculate the following here:
   *
   * (samples * channels * sample_size / 8)
   *
   * Then the plugin need to supply the value to divide the above value with (if required)
   *
   */

  val = samples * channels * track->mdia.minf.stbl.stsd.table[0].sample_size / 8;

  if (samples_to_bytes > 1) {
    val /= (longest)samples_to_bytes;
  }

  return val;
}

#else

/* AmiDog, from codecs.c */
longest quicktime_samples_to_bytes(quicktime_trak_t *track, long samples)
{
     char *compressor = track->mdia.minf.stbl.stsd.table[0].format;
     int channels = track->mdia.minf.stbl.stsd.table[0].channels;

#define QUICKTIME_IMA4 "ima4"
     if(quicktime_match_32(compressor, QUICKTIME_IMA4)) 
          return samples * channels;

#define QUICKTIME_ULAW "ulaw"
     if(quicktime_match_32(compressor, QUICKTIME_ULAW)) 
          return samples * channels;

/* Default use the sample size specification for TWOS and RAW */
     return samples * channels * track->mdia.minf.stbl.stsd.table[0].sample_size / 8;
}

#endif

static longest get_file_length(quicktime_t *file)
{
     struct stat status;
     if(fstat(fileno(file->stream), &status)) {
          printf("Failed to get filelength\n");
     }
     return status.st_size;
}

char* quicktime_get_copyright(quicktime_t *file)
{
     return file->moov.udta.copyright;
}

char* quicktime_get_name(quicktime_t *file)
{
     return file->moov.udta.name;
}

char* quicktime_get_info(quicktime_t *file)
{
     return file->moov.udta.info;
}


int quicktime_video_tracks(quicktime_t *file)
{
     int i, result = 0;
     for(i = 0; i < file->moov.total_tracks; i++)
     {
          if(file->moov.trak[i]->mdia.minf.is_video) result++;
     }
     return result;
}

int quicktime_audio_tracks(quicktime_t *file)
{
     int i, result = 0;
     quicktime_minf_t *minf;
     for(i = 0; i < file->moov.total_tracks; i++)
     {
          minf = &(file->moov.trak[i]->mdia.minf);
          if(minf->is_audio)
               result++;
     }
     return result;
}

/* ============================= Initialization functions */

int quicktime_init(quicktime_t *file)
{
     memset(file, 0, sizeof(quicktime_t));
     quicktime_moov_init(&(file->moov));
     return 0;
}

int quicktime_delete(quicktime_t *file)
{
     int i;
     if(file->total_atracks) 
     {
          for(i = 0; i < file->total_atracks; i++)
               quicktime_delete_audio_map(&(file->atracks[i]));
          free(file->atracks);
     }
     if(file->total_vtracks)
     {
          for(i = 0; i < file->total_vtracks; i++)
               quicktime_delete_video_map(&(file->vtracks[i]));
          free(file->vtracks);
     }
     file->total_atracks = 0;
     file->total_vtracks = 0;
     if(file->preload_size)
     {
          free(file->preload_buffer);
          file->preload_size = 0;
     }
     quicktime_moov_delete(&(file->moov));
     quicktime_mdat_delete(&(file->mdat));
     return 0;
}

/* =============================== Optimization functions */

void quicktime_set_preload(quicktime_t *file, longest preload)
{
     if(preload)
          if(!file->preload_size)
          {
               file->preload_size = preload;
               file->preload_buffer = calloc(1, preload);
               file->preload_start = 0;
               file->preload_end = 0;
               file->preload_ptr = 0;
          }
}

int quicktime_get_timescale(float frame_rate)
{
     int timescale = 600;
/* Encode the 29.97, 23.976, 59.94 framerates */
     if(frame_rate - (int)frame_rate != 0) 
          timescale = (int)(frame_rate * 1001 + 0.5);
     else
     if((600 / frame_rate) - (int)(600 / frame_rate) != 0) 
               timescale = (int)(frame_rate * 100 + 0.5);
     return timescale;
}

int quicktime_seek_end(quicktime_t *file)
{
     quicktime_set_position(file, file->mdat.atom.size + file->mdat.atom.start + HEADER_LENGTH * 2);
     quicktime_update_positions(file);
     return 0;
}

int quicktime_seek_start(quicktime_t *file)
{
     quicktime_set_position(file, file->mdat.atom.start + HEADER_LENGTH * 2);
     quicktime_update_positions(file);
     return 0;
}

long quicktime_audio_length(quicktime_t *file, int track)
{
/* AmiDog
     if(file->total_atracks > 0) 
          return quicktime_track_samples(file, file->atracks[track].track);
*/
     if(file->total_atracks > 0) 
          return file->atracks[track].track->mdia.minf.stbl.stco.total_entries; /* FIXME */

     return 0;
}

long quicktime_video_length(quicktime_t *file, int track)
{
     if(file->total_vtracks > 0)
          return quicktime_track_samples(file, file->vtracks[track].track);
     return 0;
}

long quicktime_audio_position(quicktime_t *file, int track)
{
     return file->atracks[track].current_position;
}

long quicktime_video_position(quicktime_t *file, int track)
{
     return file->vtracks[track].current_position;
}

int quicktime_update_positions(quicktime_t *file)
{
/* Get the sample position from the file offset */
/* for routines that change the positions of all tracks, like */
/* seek_end and seek_start but not for routines that reposition one track, like */
/* set_audio_position. */

     longest mdat_offset = quicktime_position(file) - file->mdat.atom.start;
     longest sample, chunk, chunk_offset;
     int i;

     if(file->total_atracks)
     {
          sample = quicktime_offset_to_sample(file->atracks[0].track, mdat_offset);
          chunk = quicktime_offset_to_chunk(&chunk_offset, file->atracks[0].track, mdat_offset);
          for(i = 0; i < file->total_atracks; i++)
          {
               file->atracks[i].current_position = sample;
               file->atracks[i].current_chunk = chunk;
          }
     }

     if(file->total_vtracks)
     {
          sample = quicktime_offset_to_sample(file->vtracks[0].track, mdat_offset);
          chunk = quicktime_offset_to_chunk(&chunk_offset, file->vtracks[0].track, mdat_offset);
          for(i = 0; i < file->total_vtracks; i++)
          {
               file->vtracks[i].current_position = sample;
               file->vtracks[i].current_chunk = chunk;
          }
     }
     return 0;
}

int quicktime_set_audio_position(quicktime_t *file, longest sample, int track)
{
     longest offset, chunk_sample, chunk;
     quicktime_trak_t *trak;

     if(file->total_atracks)
     {
          trak = file->atracks[track].track;
          file->atracks[track].current_position = sample;
          quicktime_chunk_of_sample(&chunk_sample, &chunk, trak, sample);
          file->atracks[track].current_chunk = chunk;
          offset = quicktime_sample_to_offset(trak, sample);
          quicktime_set_position(file, offset);
     }

     return 0;
}

int quicktime_set_video_position(quicktime_t *file, longest frame, int track)
{
     longest offset, chunk_sample, chunk;
     quicktime_trak_t *trak;

     if(file->total_vtracks)
     {
          trak = file->vtracks[track].track;
          file->vtracks[track].current_position = frame;
          quicktime_chunk_of_sample(&chunk_sample, &chunk, trak, frame);
          file->vtracks[track].current_chunk = chunk;
          offset = quicktime_sample_to_offset(trak, frame);
          quicktime_set_position(file, offset);
     }
     return 0;
}

int quicktime_has_audio(quicktime_t *file)
{
     if(quicktime_audio_tracks(file)) return 1;
     return 0;
}

long quicktime_sample_rate(quicktime_t *file, int track)
{
     if(file->total_atracks)
          return file->atracks[track].track->mdia.minf.stbl.stsd.table[0].sample_rate;
     return 0;
}

int quicktime_audio_bits(quicktime_t *file, int track)
{
     if(file->total_atracks)
          return file->atracks[track].track->mdia.minf.stbl.stsd.table[0].sample_size;

     return 0;
}

char* quicktime_audio_compressor(quicktime_t *file, int track)
{
     return file->atracks[track].track->mdia.minf.stbl.stsd.table[0].format;
}

int quicktime_track_channels(quicktime_t *file, int track)
{
     if(track < file->total_atracks)
          return file->atracks[track].channels;

     return 0;
}

int quicktime_channel_location(quicktime_t *file, int *quicktime_track, int *quicktime_channel, int channel)
{
     int current_channel = 0, current_track = 0;
     *quicktime_channel = 0;
     *quicktime_track = 0;
     for(current_channel = 0, current_track = 0; current_track < file->total_atracks; )
     {
          if(channel >= current_channel)
          {
               *quicktime_channel = channel - current_channel;
               *quicktime_track = current_track;
          }

          current_channel += file->atracks[current_track].channels;
          current_track++;
     }
     return 0;
}

int quicktime_has_video(quicktime_t *file)
{
     if(quicktime_video_tracks(file)) return 1;
     return 0;
}

int quicktime_video_width(quicktime_t *file, int track)
{
     if(file->total_vtracks)
          return file->vtracks[track].track->tkhd.track_width;
     return 0;
}

int quicktime_video_height(quicktime_t *file, int track)
{
     if(file->total_vtracks)
          return file->vtracks[track].track->tkhd.track_height;
     return 0;
}

int quicktime_video_depth(quicktime_t *file, int track)
{
     if(file->total_vtracks)
          return file->vtracks[track].track->mdia.minf.stbl.stsd.table[0].depth;
     return 0;
}

float quicktime_frame_rate(quicktime_t *file, int track)
{
     if(file->total_vtracks > track)
          return (float)file->vtracks[track].track->mdia.mdhd.time_scale / 
               file->vtracks[track].track->mdia.minf.stbl.stts.table[0].sample_duration;

     return 0;
}

char* quicktime_video_compressor(quicktime_t *file, int track)
{
     return file->vtracks[track].track->mdia.minf.stbl.stsd.table[0].format;
}

long quicktime_read_audio(quicktime_t *file, char *audio_buffer, long samples, int track)
{
     longest chunk_sample, chunk;
     int result = 0 /*, track_num*/ ;
     quicktime_trak_t *trak = file->atracks[track].track;
     longest fragment_len, chunk_end;
     longest start_position = file->atracks[track].current_position;
     longest position = file->atracks[track].current_position;
     longest /*start = position,*/ end = position + samples;
     longest bytes, total_bytes = 0;
     longest buffer_offset;

     quicktime_chunk_of_sample(&chunk_sample, &chunk, trak, position);
     buffer_offset = 0;

     while(position < end && !result)
     {
          quicktime_set_audio_position(file, position, track);
          fragment_len = quicktime_chunk_samples(trak, chunk);
          chunk_end = chunk_sample + fragment_len;
          fragment_len -= position - chunk_sample;
          if(position + fragment_len > chunk_end) fragment_len = chunk_end - position;
          if(position + fragment_len > end) fragment_len = end - position;

          bytes = quicktime_samples_to_bytes(trak, fragment_len);
          result = !quicktime_read_data(file, &audio_buffer[buffer_offset], bytes);

          total_bytes += bytes;
          position += fragment_len;
          chunk_sample = position;
          buffer_offset += bytes;
          chunk++;
     }

// Create illusion of track being advanced only by samples
     file->atracks[track].current_position = start_position + samples;
     if(result) return 0;
     return total_bytes;
}

int quicktime_read_chunk(quicktime_t *file, char *output, int track, longest chunk, longest byte_start, longest byte_len)
{
     quicktime_set_position(file, quicktime_chunk_to_offset(file->atracks[track].track, chunk) + byte_start);
     if(quicktime_read_data(file, output, byte_len)) return 0;
     else
     return 1;
}

long quicktime_frame_size(quicktime_t *file, long frame, int track)
{
     long bytes = 0;
     quicktime_trak_t *trak = file->vtracks[track].track;

     if(trak->mdia.minf.stbl.stsz.sample_size)
     {
          bytes = trak->mdia.minf.stbl.stsz.sample_size;
     }
     else
     {
          long total_frames = quicktime_track_samples(file, trak);
          if(frame < 0) frame = 0;
          else
          if(frame > total_frames - 1) frame = total_frames - 1;
          bytes = trak->mdia.minf.stbl.stsz.table[frame].size;
     }


     return bytes;
}


/* AmiDog TEST -> */

/* Get size and read */
long quicktime_frame_size_audio(quicktime_t *file, int track, char *buffer)
{
     longest chunk, chunk_sample;
     long chunk_samples, chunk_bytes;
     quicktime_trak_t *trak = file->atracks[track].track;
     int result = 0;


/* Get the first chunk with this routine and then increase the chunk number. */
     quicktime_chunk_of_sample(&chunk_sample, &chunk, trak, file->atracks[track].current_position);

/* Get the byte count to read. */
     chunk_samples = quicktime_chunk_samples(trak, chunk);

     chunk_bytes = quicktime_samples_to_bytes(trak, chunk_samples);

/* Read the entire chunk regardless of where the desired sample range starts. */
     result = quicktime_read_chunk(file, buffer, track, chunk, 0, chunk_bytes);

     file->atracks[track].current_position += chunk_samples; /* Is this right ? */

     if (!result) {
          return chunk_bytes;
     }

     return 0;
}

/* Only get size */
long quicktime_audio_frame_size(quicktime_t *file, int track)
{
     longest chunk, chunk_sample;
     long chunk_samples, chunk_bytes;
     quicktime_trak_t *trak = file->atracks[track].track;


/* Get the first chunk with this routine and then increase the chunk number. */
     quicktime_chunk_of_sample(&chunk_sample, &chunk, trak, file->atracks[track].current_position);

/* Get the byte count to read. */
     chunk_samples = quicktime_chunk_samples(trak, chunk);

     chunk_bytes = quicktime_samples_to_bytes(trak, chunk_samples);

/* Just set the position so that I can add it to the index. */
     quicktime_set_position(file, quicktime_chunk_to_offset(file->atracks[track].track, chunk));

     file->atracks[track].current_position += chunk_samples; /* Is this right ? */

     return chunk_bytes;
}

/* <- AmiDog TEST */


long quicktime_read_frame(quicktime_t *file, unsigned char *video_buffer, int track)
{
     longest bytes;
     int result = 0;

     bytes = quicktime_frame_size(file, file->vtracks[track].current_position, track);

     quicktime_set_video_position(file, file->vtracks[track].current_position, track);
     result = quicktime_read_data(file, video_buffer, bytes);
     file->vtracks[track].current_position++;

     if(!result) return 0;
     return bytes;
}

long quicktime_get_keyframe_before(quicktime_t *file, long frame, int track)
{
     quicktime_trak_t *trak = file->vtracks[track].track;
     quicktime_stss_t *stss = &trak->mdia.minf.stbl.stss;
     int i;

// Offset 1
     frame++;

     for(i = stss->total_entries - 1; i >= 0; i--)
     {
          if(stss->table[i].sample <= frame) return stss->table[i].sample - 1;
     }

     return 0;
}

long quicktime_is_keyframe(quicktime_t *file, long frame, int track)
{
     quicktime_trak_t *trak = file->vtracks[track].track;
     quicktime_stss_t *stss = &trak->mdia.minf.stbl.stss;
     int i;

// Offset 1
     frame++;

     for(i = stss->total_entries - 1; i >= 0; i--)
     {
          if(stss->table[i].sample == frame) return 1;
     }

     return 0;
}

int quicktime_has_keyframes(quicktime_t *file, int track)
{
     quicktime_trak_t *trak = file->vtracks[track].track;
     quicktime_stss_t *stss = &trak->mdia.minf.stbl.stss;
     
     return stss->total_entries > 0;
}

int quicktime_read_frame_init(quicktime_t *file, int track)
{
     /*quicktime_trak_t *trak = file->vtracks[track].track;*/
     quicktime_set_video_position(file, file->vtracks[track].current_position, track);
     if(quicktime_ftell(file) != file->file_position) 
     {
          FSEEK(file->stream, file->file_position, SEEK_SET);
          file->ftell_position = file->file_position;
     }
     return 0;
}

int quicktime_read_frame_end(quicktime_t *file, int track)
{
     file->file_position = quicktime_ftell(file);
     file->vtracks[track].current_position++;
     return 0;
}

int quicktime_init_video_map(quicktime_video_map_t *vtrack, quicktime_trak_t *trak)
{
     vtrack->track = trak;
     vtrack->current_position = 0;
     vtrack->current_chunk = 1;

/*
printf("VCODEC: %s\n", vtrack->track->mdia.minf.stbl.stsd.table[0].format);
*/

     return 0;
}

int quicktime_delete_video_map(quicktime_video_map_t *vtrack)
{
     return 0;
}

int quicktime_init_audio_map(quicktime_audio_map_t *atrack, quicktime_trak_t *trak)
{
     atrack->track = trak;
     atrack->channels = trak->mdia.minf.stbl.stsd.table[0].channels;
     atrack->current_position = 0;
     atrack->current_chunk = 1;

/*
printf("ACODEC: %s\n", atrack->track->mdia.minf.stbl.stsd.table[0].format);
*/

     return 0;
}

int quicktime_delete_audio_map(quicktime_audio_map_t *atrack)
{
     return 0;
}

int quicktime_read_info(quicktime_t *file)
{
     int result = 0, found_moov = 0;
     int i, track;
     longest start_position = quicktime_position(file);
     quicktime_atom_t leaf_atom;

     quicktime_set_position(file, 0LL);

     do
     {
          result = quicktime_atom_read_header(file, &leaf_atom);

          if(!result)
          {
               if(quicktime_atom_is(&leaf_atom, "mdat")) 
               {
                    quicktime_read_mdat(file, &(file->mdat), &leaf_atom);
               }
               else
               if(quicktime_atom_is(&leaf_atom, "moov")) 
               {
/* Set preload and preload the moov atom here */
                    longest start_position = quicktime_position(file);
                    long temp_size = leaf_atom.end - start_position;
                    unsigned char *temp = malloc(temp_size);
                    quicktime_set_preload(file, (temp_size < 0x100000) ? 0x100000 : temp_size);
                    quicktime_read_data(file, temp, temp_size);
                    quicktime_set_position(file, start_position);
                    free(temp);
                    
                    quicktime_read_moov(file, &(file->moov), &leaf_atom);
                    found_moov = 1;
               }
               else
                    quicktime_atom_skip(file, &leaf_atom);
          }
     }while(!result && quicktime_position(file) < file->total_length);

/* go back to the original position */
     quicktime_set_position(file, start_position);

     if(found_moov)
     {
/* get tables for all the different tracks */
          file->total_atracks = quicktime_audio_tracks(file);

          if (file->total_atracks > 0) { // AmiDog: Required on PPC
               file->atracks = (quicktime_audio_map_t*)calloc(1, sizeof(quicktime_audio_map_t) * file->total_atracks);
               for(i = 0, track = 0; i < file->total_atracks; i++)
               {
                    while(!file->moov.trak[track]->mdia.minf.is_audio)
                         track++;
                    quicktime_init_audio_map(&(file->atracks[i]), file->moov.trak[track]);
                    track++; /* Skip to next track */
               }
          }

          file->total_vtracks = quicktime_video_tracks(file);

          if (file->total_vtracks > 0) { // AmiDog: Required on PPC
               file->vtracks = (quicktime_video_map_t*)calloc(1, sizeof(quicktime_video_map_t) * file->total_vtracks);
               for(track = 0, i = 0; i < file->total_vtracks; i++)
               {
                    while(!file->moov.trak[track]->mdia.minf.is_video)
                         track++;
                    quicktime_init_video_map(&(file->vtracks[i]), file->moov.trak[track]);
                    track++; /* Skip to next track */
               }
          }
     }

     return !found_moov;
}

// ================================== Entry points =============================

quicktime_t* quicktime_open(char *filename)
{
     quicktime_t *new_file = calloc(1, sizeof(quicktime_t));

     quicktime_init(new_file);
     new_file->mdat.atom.start = 0;

     new_file->decompressed_buffer_size = 0;
     new_file->decompressed_buffer = NULL;
     new_file->decompressed_position = 0;

     if(!(new_file->stream = fopen(filename, "r")))
     {
          printf("Failed to open file\n");
          free(new_file);
          return 0;
     }

// Get length.
     new_file->total_length = get_file_length(new_file);

     if(quicktime_read_info(new_file))
     {
          quicktime_close(new_file);
          printf("Error in quicktime header, playback failed\n");
          new_file = 0;
     }

     return new_file;
}

int quicktime_close(quicktime_t *file)
{
     int result;
     result = fclose(file->stream);

     quicktime_delete(file);
     free(file);
     return result;
}
