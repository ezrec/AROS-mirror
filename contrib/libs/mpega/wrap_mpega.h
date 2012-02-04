#ifndef WRAP_MPEGA_H
#define WRAP_MPEGA_H

#include <libraries/mpega.h>

MPEGA_STREAM *WRAP_MPEGA_open(char *stream_name, MPEGA_CTRL *ctrl, APTR a4base);
void WRAP_MPEGA_close(MPEGA_STREAM *mpega_stream, APTR a4base);
LONG WRAP_MPEGA_decode_frame(MPEGA_STREAM *mpega_stream, WORD *pcm[MPEGA_MAX_CHANNELS], APTR a4base);
LONG WRAP_MPEGA_seek(MPEGA_STREAM *mpega_stream, ULONG ms_time_position, APTR a4base);
LONG WRAP_MPEGA_time(MPEGA_STREAM *mpega_stream, ULONG *ms_time_position);
LONG WRAP_MPEGA_find_sync(UBYTE *buffer, LONG buffer_size);
LONG WRAP_MPEGA_scale(MPEGA_STREAM *mpega_stream, LONG scale_percent);

#endif
