/*
 *
 * video_out.h
 *
 */

extern int vo_setup (void);
extern void vo_close (void);

#define VO_TOP_FIELD 1
#define VO_BOTTOM_FIELD 2
#define VO_BOTH_FIELDS (VO_TOP_FIELD | VO_BOTTOM_FIELD)
#define VO_PREDICTION_FLAG 4

static inline struct vo_frame_s * vo_get_frame (mpeg2dec_t * mpeg2dec, int flags)
{
  if (flags & VO_PREDICTION_FLAG) {
    mpeg2dec->prediction_index ^= 1;
    return &mpeg2dec->frame[mpeg2dec->prediction_index];
  } else {
    return &mpeg2dec->frame[2];
  }
}

static inline void vo_draw (mpeg2dec_t * mpeg2dec, struct vo_frame_s * frame)
{
  mpeg2dec->drop_flag = amp->video_refresh(frame->base, mpeg2dec->current_sync_time, mpeg2dec->current_video_time);
}
