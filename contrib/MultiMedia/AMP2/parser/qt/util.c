#include <stdio.h>
#include <time.h>

#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "quicktime.h"

/* Disk I/O */

longest quicktime_ftell(quicktime_t *file)
{
     return file->ftell_position;
}

int quicktime_fseek(quicktime_t *file, longest offset)
{
     file->ftell_position = offset;
     if(offset > file->total_length || offset < 0) return 1;
     if(FSEEK(file->stream, file->ftell_position, SEEK_SET))
     {
          return 1;
     }
     return 0;
}

/* Read entire buffer from the preload buffer */
static int read_preload(quicktime_t *file, char *data, longest size)
{
     longest selection_start = 0;
     longest selection_end = 0;
     longest fragment_start = 0;
     longest fragment_len = 0;

     selection_start = file->file_position;
     selection_end = file->file_position + size;

     fragment_start = file->preload_ptr + (selection_start - file->preload_start);
     while(fragment_start < 0) fragment_start += file->preload_size;
     while(fragment_start >= file->preload_size) fragment_start -= file->preload_size;

     while(selection_start < selection_end)
     {
          fragment_len = selection_end - selection_start;
          if(fragment_start + fragment_len > file->preload_size)
               fragment_len = file->preload_size - fragment_start;

          memcpy(data, file->preload_buffer + fragment_start, fragment_len);
          fragment_start += fragment_len;
          data += fragment_len;

          if(fragment_start >= file->preload_size) fragment_start = (longest)0;
          selection_start += fragment_len;
     }
     return 0;
}

int quicktime_read_data(quicktime_t *file, char *data, longest size)
{
     int result = 1;

     if(file->decompressed_buffer)
     {
          if(file->decompressed_position < file->decompressed_buffer_size)
          {
               memcpy(data, file->decompressed_buffer+file->decompressed_position, size);
               file->decompressed_position+=size;
               return result;
          }
          else
          {
               file->decompressed_position = 0;
               file->decompressed_buffer_size = 0;
               free(file->decompressed_buffer);
               file->decompressed_buffer = NULL;
          }
     }

     if(!file->preload_size)
     {
          quicktime_fseek(file, file->file_position);
          result = fread(data, size, 1, file->stream);
          file->ftell_position += size;
     }
     else
     {
          longest selection_start = file->file_position;
          longest selection_end = file->file_position + size;
          longest fragment_start, fragment_len;

          if(selection_end - selection_start > file->preload_size)
          {
/* Size is larger than preload size.  Should never happen. */
               quicktime_fseek(file, file->file_position);
               result = fread(data, size, 1, file->stream);
               file->ftell_position += size;
          }
          else
          if(selection_start >= file->preload_start && 
               selection_start < file->preload_end &&
               selection_end <= file->preload_end &&
               selection_end > file->preload_start)
          {
/* Entire range is in buffer */
               read_preload(file, data, size);
          }
          else
          if(selection_end > file->preload_end && 
               selection_end - file->preload_size < file->preload_end)
          {
/* Range is after buffer */
/* Move the preload start to within one preload length of the selection_end */
               while(selection_end - file->preload_start > file->preload_size)
               {
                    fragment_len = selection_end - file->preload_start - file->preload_size;
                    if(file->preload_ptr + fragment_len > file->preload_size) 
                         fragment_len = file->preload_size - file->preload_ptr;
                    file->preload_start += fragment_len;
                    file->preload_ptr += fragment_len;
                    if(file->preload_ptr >= file->preload_size) file->preload_ptr = 0;
               }

/* Append sequential data after the preload end to the new end */
               fragment_start = file->preload_ptr + file->preload_end - file->preload_start;
               while(fragment_start >= file->preload_size) fragment_start -= file->preload_size;

               while(file->preload_end < selection_end)
               {
                    fragment_len = selection_end - file->preload_end;
                    if(fragment_start + fragment_len > file->preload_size) fragment_len = file->preload_size - fragment_start;
                    quicktime_fseek(file, file->preload_end);
                    result = fread(&(file->preload_buffer[fragment_start]), fragment_len, 1, file->stream);
                    file->ftell_position += fragment_len;
                    file->preload_end += fragment_len;
                    fragment_start += fragment_len;
                    if(fragment_start >= file->preload_size) fragment_start = 0;
               }

               read_preload(file, data, size);
          }
          else
          {
/* Range is before buffer or over a preload_size away from the end of the buffer. */
/* Replace entire preload buffer with range. */
               quicktime_fseek(file, file->file_position);
               result = fread(file->preload_buffer, size, 1, file->stream);
               file->ftell_position += size;
               file->preload_start = file->file_position;
               file->preload_end = file->file_position + size;
               file->preload_ptr = 0;
               read_preload(file, data, size);
          }
     }

     file->file_position += size;
     return result;
}

longest quicktime_byte_position(quicktime_t *file)
{
     return quicktime_position(file);
}


void quicktime_read_pascal(quicktime_t *file, char *data)
{
     /*char*/ int len = quicktime_read_char(file);

     quicktime_read_data(file, data, len);
     data[len] = 0;
}

float quicktime_read_fixed32(quicktime_t *file)
{
     unsigned long a, b, c, d;
     unsigned char data[4];

     quicktime_read_data(file, data, 4);
     a = data[0];
     b = data[1];
     c = data[2];
     d = data[3];
     
     a = (a << 8) + b;
     b = (c << 8) + d;

     if(b)
          return (float)a + (float)b / 65536;
     else
          return a;
}

float quicktime_read_fixed16(quicktime_t *file)
{
     unsigned char data[2];
     
     quicktime_read_data(file, data, 2);
     if(data[1])
          return (float)data[0] + (float)data[1] / 256;
     else
          return (float)data[0];
}

unsigned long quicktime_read_uint32(quicktime_t *file)
{
     unsigned long result;
     unsigned long a, b, c, d;
     char data[4];

     quicktime_read_data(file, data, 4);
     a = (unsigned char)data[0];
     b = (unsigned char)data[1];
     c = (unsigned char)data[2];
     d = (unsigned char)data[3];

     result = (a << 24) | (b << 16) | (c << 8) | d;
     return result;
}

long quicktime_read_int32(quicktime_t *file)
{
     unsigned long result;
     unsigned long a, b, c, d;
     char data[4];

     quicktime_read_data(file, data, 4);
     a = (unsigned char)data[0];
     b = (unsigned char)data[1];
     c = (unsigned char)data[2];
     d = (unsigned char)data[3];

     result = (a << 24) | (b << 16) | (c << 8) | d;
     return (long)result;
}

longest quicktime_read_int64(quicktime_t *file)
{
     ulongest result, a, b, c, d, e, f, g, h;
     char data[8];

     quicktime_read_data(file, data, 8);
     a = (unsigned char)data[0];
     b = (unsigned char)data[1];
     c = (unsigned char)data[2];
     d = (unsigned char)data[3];
     e = (unsigned char)data[4];
     f = (unsigned char)data[5];
     g = (unsigned char)data[6];
     h = (unsigned char)data[7];

     result = (a << 56) | 
          (b << 48) | 
          (c << 40) | 
          (d << 32) | 
          (e << 24) | 
          (f << 16) | 
          (g << 8) | 
          h;
     return (longest)result;
}


long quicktime_read_int24(quicktime_t *file)
{
     unsigned long result;
     unsigned long a, b, c;
     char data[4];
     
     quicktime_read_data(file, data, 3);
     a = (unsigned char)data[0];
     b = (unsigned char)data[1];
     c = (unsigned char)data[2];

     result = (a << 16) | (b << 8) | c;
     return (long)result;
}

int quicktime_read_int16(quicktime_t *file)
{
     unsigned long result;
     unsigned long a, b;
     char data[2];
     
     quicktime_read_data(file, data, 2);
     a = (unsigned char)data[0];
     b = (unsigned char)data[1];

     result = (a << 8) | b;
     return (int)result;
}

int quicktime_read_char(quicktime_t *file)
{
     char output;
     quicktime_read_data(file, &output, 1);
     return output;
}

void quicktime_read_char32(quicktime_t *file, char *string)
{
     quicktime_read_data(file, string, 4);
}

longest quicktime_position(quicktime_t *file) 
{
     if(file->decompressed_buffer)
          return file->decompressed_position;

     return file->file_position; 
}

int quicktime_set_position(quicktime_t *file, longest position) 
{
     if(file->decompressed_buffer)
          file->decompressed_position = position;
     else
          file->file_position = position;

     return 0;
}

void quicktime_copy_char32(char *output, char *input)
{
     *output++ = *input++;
     *output++ = *input++;
     *output++ = *input++;
     *output = *input;
}

unsigned long quicktime_current_time(void)
{
     time_t t;
     time (&t);
     return (t+(66*31536000)+1468800);
}

int quicktime_match_32(char *input, char *output)
{
     if(input[0] == output[0] &&
          input[1] == output[1] &&
          input[2] == output[2] &&
          input[3] == output[3])
          return 1;
     else 
          return 0;
}
