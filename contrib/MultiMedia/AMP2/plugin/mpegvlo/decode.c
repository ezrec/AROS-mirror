/*
 *
 * decode.c
 *
 */

#include <stdio.h>
#include <string.h> /* memcpy/memset, try to remove */
#include <stdlib.h>
#include <inttypes.h>
#include <plugin.h>

#include "mpeg2_internal.h"
#include "mpeg2.h"
#include "video_out.h"

#define BUFFER_SIZE (224 * 1024)

void mpeg2_init (mpeg2dec_t * mpeg2dec)
{
    idct_init ();
    motion_comp_init ();

    mpeg2dec->chunk_buffer = mallocalign (BUFFER_SIZE + 4);
    mpeg2dec->picture = mallocalign (sizeof (picture_t));

    mpeg2dec->shift = 0xffffff00;
    mpeg2dec->is_sequence_needed = 1;
    mpeg2dec->drop_flag = 0;
    mpeg2dec->drop_frame = 0;
    mpeg2dec->in_slice = 0;
    mpeg2dec->chunk_ptr = mpeg2dec->chunk_buffer;
    mpeg2dec->code = 0xb4;

    memset (mpeg2dec->picture, 0, sizeof (picture_t));

    /* initialize supstructures */
    header_state_init (mpeg2dec->picture);
}

static inline int parse_chunk (mpeg2dec_t * mpeg2dec, int code, uint8_t * buffer)
{
    picture_t * picture;
    int is_frame_done;

    /* wait for sequence_header_code */
    if (mpeg2dec->is_sequence_needed && (code != 0xb3))
     return 0;

    picture = mpeg2dec->picture;
    is_frame_done = mpeg2dec->in_slice && ((!code) || (code >= 0xb0));

    if (is_frame_done) {
     mpeg2dec->in_slice = 0;

     /* Do not display garbled frames / AmiDog */
     if (mpeg2dec->search_next_i_frame == 0) {
       if (((picture->picture_structure == FRAME_PICTURE) || (picture->second_field)) && (!(mpeg2dec->drop_frame))) {
        vo_draw (mpeg2dec, (picture->picture_coding_type == B_TYPE) ? picture->current_frame : picture->forward_reference_frame);
       }
     /* Skip 3 frames after the first I-frame (3, 2, 1) */
     } else if (mpeg2dec->search_next_i_frame < 4) {
       mpeg2dec->search_next_i_frame--;
     }
    }

    switch (code) {
    case 0x00: /* picture_start_code */
     if (header_process_picture_header (picture, buffer)) {
         /* bad picture header */
         return 0;
     }

     /* Calculate next frame time based upon the framerate /AmiDog */
     mpeg2dec->current_sync_time += mpeg2dec->current_frame_time;
     mpeg2dec->current_video_time += mpeg2dec->current_frame_time;

     /* Have we gotten a new PTS/DTS ? /AmiDog */
     if (mpeg2dec->sync_time > 0.0) {
       mpeg2dec->current_sync_time = mpeg2dec->sync_time;
       mpeg2dec->sync_time = -1.0;
     }

     /* This is to avoid garbled frames after a seek /AmiDog */
     if (picture->picture_coding_type == I_TYPE) {
       if (mpeg2dec->search_next_i_frame == 4) {
         mpeg2dec->search_next_i_frame = 3;
       }
     }

     mpeg2dec->drop_frame =
         mpeg2dec->drop_flag && (picture->picture_coding_type == B_TYPE);
     if (mpeg2dec->drop_frame) amp->skiped_frames++;
     break;

    case 0xb3: /* sequence_header_code */
     if (header_process_sequence_header (picture, buffer)) {
         /* bad sequence header */
         return 0;
     }
     if (mpeg2dec->is_sequence_needed) {
         mpeg2dec->frame_rate_code = picture->frame_rate_code;
         if (vo_setup ()) {
          /* display setup failed */
          return 0;
         }
         mpeg2dec->is_sequence_needed = 0;
         picture->forward_reference_frame =
          vo_get_frame (mpeg2dec,
                     VO_PREDICTION_FLAG | VO_BOTH_FIELDS);
         picture->backward_reference_frame =
          vo_get_frame (mpeg2dec,
                     VO_PREDICTION_FLAG | VO_BOTH_FIELDS);
     }
     break;

    case 0xb5: /* extension_start_code */
     if (header_process_extension (picture, buffer)) {
         /* bad extension */
         return 0;
     }
     break;

    default:
     if (code >= 0xb0)
         break;

     if (!(mpeg2dec->in_slice)) {
         mpeg2dec->in_slice = 1;

         if (!picture->second_field) {
          if (picture->picture_coding_type == B_TYPE)
              picture->current_frame =
               vo_get_frame (mpeg2dec,
                          picture->picture_structure);
          else {
              picture->current_frame =
               vo_get_frame (mpeg2dec,
                          (VO_PREDICTION_FLAG |
                           picture->picture_structure));
              picture->forward_reference_frame =
               picture->backward_reference_frame;
              picture->backward_reference_frame = picture->current_frame;
          }
         }
     }

     if ((!(mpeg2dec->drop_frame)) && (mpeg2dec->search_next_i_frame != 4)) {
         slice_process (picture, code, buffer);
     }
    }

    return is_frame_done;
}

static inline uint8_t * copy_chunk (mpeg2dec_t * mpeg2dec,
                        uint8_t * current, uint8_t * end)
{
    uint32_t shift;
    uint8_t * chunk_ptr;
    uint8_t * limit;
    uint8_t byte;

    shift = mpeg2dec->shift;
    chunk_ptr = mpeg2dec->chunk_ptr;
    limit = current + (mpeg2dec->chunk_buffer + BUFFER_SIZE - chunk_ptr);
    if (limit > end)
     limit = end;

    while (1) {
     byte = *current++;
     if (shift != 0x00000100) {
         shift = (shift | byte) << 8;
         *chunk_ptr++ = byte;
         if (current < limit)
          continue;
         if (current == end) {
          mpeg2dec->chunk_ptr = chunk_ptr;
          mpeg2dec->shift = shift;
          return NULL;
         } else {
          /* we filled the chunk buffer without finding a start code */
          mpeg2dec->code = 0xb4;   /* sequence_error_code */
          mpeg2dec->chunk_ptr = mpeg2dec->chunk_buffer;
          return current;
         }
     }
     mpeg2dec->code = byte;
     mpeg2dec->chunk_ptr = mpeg2dec->chunk_buffer;
     mpeg2dec->shift = 0xffffff00;
     return current;
    }
}

int mpeg2_decode_data (mpeg2dec_t * mpeg2dec, uint8_t * current, uint8_t * end)
{
    int ret;
    uint8_t code;

    ret = 0;

    while (current != end) {
     code = mpeg2dec->code;
     current = copy_chunk (mpeg2dec, current, end);
     if (current == NULL)
         return ret;
     ret += parse_chunk (mpeg2dec, code, mpeg2dec->chunk_buffer);
    }
    return ret;
}

void mpeg2_close (mpeg2dec_t * mpeg2dec)
{
    static uint8_t finalizer[] = {0,0,1,0};

    mpeg2_decode_data (mpeg2dec, finalizer, finalizer+4);

    freealign (mpeg2dec->chunk_buffer);
    freealign (mpeg2dec->picture);

    vo_close ();
}
