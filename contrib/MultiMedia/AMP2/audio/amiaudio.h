/*
 * amiaudio.h
 */

#ifndef AMIAUDIO_H
#define AMIAUDIO_H

#include <amitypes.h>
#include <amiaudiodef.h>

extern amiaudio_t *audio_new(u32 *tags);
extern s32 audio_set(amiaudio_t *audio, u32 tag, u32 value);
extern void audio_dispose(amiaudio_t *audio);

#define AUDIO_NEW(tags...) \
  ({ u32 t[] = { tags, AUDIO_DONE }; audio_new(t); })

/* all sizes are in 'samples per channel' */

extern u32 audio_init(amiaudio_t *audio); /* returns suggested audiomode */
extern s32 audio_open(amiaudio_t *audio, u32 frequency, u32 mode, u32 fragsize, u32 frags, void (*audio_sync)(u32 time));
extern void audio_play(amiaudio_t *audio, void *data, u32 size, u32 time);
extern u32 audio_in_buffer(amiaudio_t *audio);
extern void audio_pause(amiaudio_t *audio);
extern void audio_close(amiaudio_t *audio);

/* size as bytes instead of samples */

extern void audio_play_bytes(amiaudio_t *audio, void *data, u32 size, u32 time);

/* setup */

extern s32 audio_setup(void);
extern void audio_cleanup(void);

#endif /* AMIAUDIO_H */
