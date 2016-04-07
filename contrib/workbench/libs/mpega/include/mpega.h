#ifndef LIBRARIES_MPEGA_H
#define LIBRARIES_MPEGA_H

/*
   Header recreated. Original one from mpega_library.lha on Aminet may
   likely not be distributed outside that archive. You can replace this
   header with the original one from mpega_library.lha on Aminet, if you
   want. It contains some comments and docs.
*/

#define MPEGA_VERSION 2

#ifndef EXEC_TYPES_H
#   include <exec/types.h>
#endif

#ifndef UTILITY_HOOKS_H
#   include <utility/hooks.h>
#endif

#define MPEGA_QUALITY_LOW   	0
#define MPEGA_QUALITY_MEDIUM	1
#define MPEGA_QUALITY_HIGH  	2

#define MPEGA_BSFUNC_OPEN   	0
#define MPEGA_BSFUNC_CLOSE  	1
#define MPEGA_BSFUNC_READ   	2
#define MPEGA_BSFUNC_SEEK   	3

struct MPEGA_ACCESS
{
    LONG     	  func;
    union
    {
	struct
	{
            char *stream_name;
            LONG  buffer_size;
            LONG  stream_size;
	} open;
	struct
	{
            void *buffer;
            LONG  num_bytes;
	} read;
	struct
	{
            LONG  abs_byte_seek_pos;
	} seek;
    } data;
};
typedef struct MPEGA_ACCESS MPEGA_ACCESS;

struct MPEGA_OUTPUT
{
    WORD freq_div;
    WORD quality;
    LONG freq_max;
};
typedef struct MPEGA_OUTPUT MPEGA_OUTPUT;

struct MPEGA_LAYER
{
    WORD     	 force_mono;
    struct MPEGA_OUTPUT mono;
    struct MPEGA_OUTPUT stereo;
};
typedef struct MPEGA_LAYER MPEGA_LAYER;

struct MPEGA_CTRL
{
    struct Hook *bs_access;
    struct MPEGA_LAYER  layer_1_2;
    struct MPEGA_LAYER  layer_3;
    WORD     	 check_mpeg;
    LONG     	 stream_buffer_size;
};
typedef struct MPEGA_CTRL MPEGA_CTRL;

#define MPEGA_MODE_STEREO   0
#define MPEGA_MODE_J_STEREO 1
#define MPEGA_MODE_DUAL     2
#define MPEGA_MODE_MONO     3

struct MPEGA_STREAM
{
    WORD   norm;
    WORD   layer;
    WORD   mode;
    WORD   bitrate;
    LONG   frequency;
    WORD   channels;
    ULONG  ms_duration;
    WORD   private_bit;
    WORD   copyright;
    WORD   original;
    WORD   dec_channels;
    WORD   dec_quality;
    LONG   dec_frequency;
    void  *handle;
};
typedef struct MPEGA_STREAM MPEGA_STREAM;

#define MPEGA_MAX_CHANNELS  2
#define MPEGA_PCM_SIZE	    1152

#define MPEGA_ERR_NONE      0
#define MPEGA_ERR_BASE      0
#define MPEGA_ERR_EOF       (MPEGA_ERR_BASE - 1)
#define MPEGA_ERR_BADFRAME  (MPEGA_ERR_BASE - 2)
#define MPEGA_ERR_MEM       (MPEGA_ERR_BASE - 3)
#define MPEGA_ERR_NO_SYNC   (MPEGA_ERR_BASE - 4)
#define MPEGA_ERR_BADVALUE  (MPEGA_ERR_BASE - 5)

#endif /* LIBRARIES_MPEGA_H */
