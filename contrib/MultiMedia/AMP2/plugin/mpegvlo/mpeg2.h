/*
 *
 * mpeg2.h
 *
 */

/* Structure for the mpeg2dec decoder */

typedef struct mpeg2dec_s {
    int prediction_index;
    struct vo_frame_s frame[3];

    /* this is where we keep the state of the decoder */
    struct picture_s * picture;

    uint32_t shift;
    int is_display_initialized;
    int is_sequence_needed;
    int drop_flag;
    int drop_frame;
    int in_slice;

    /* the maximum chunk size is determined by vbv_buffer_size */
    /* which is 224K for MP@ML streams. */
    /* (we make no pretenses of decoding anything more than that) */
    /* allocated in init - gcc has problems allocating such big structures */
    uint8_t * chunk_buffer;
    /* pointer to current position in chunk_buffer */
    uint8_t * chunk_ptr;
    /* last start code ? */
    uint8_t code;

    /* ONLY for 0.2.0 release - will not stay there later */
    int frame_rate_code;

    /* AmiDog, sync and frameskip */
    double sync_time;
    double current_sync_time;
    double current_video_time;
    double current_frame_rate;
    double current_frame_time;
    int search_next_i_frame;
} mpeg2dec_t ;

/* initialize mpegdec with a opaque user pointer */
void mpeg2_init (mpeg2dec_t * mpeg2dec);

/* destroy everything which was allocated, shutdown the output */
void mpeg2_close (mpeg2dec_t * mpeg2dec);

int mpeg2_decode_data (mpeg2dec_t * mpeg2dec, uint8_t * data_start, uint8_t * data_end);
