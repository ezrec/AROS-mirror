/*
    Copyright © 1995-2012, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/libcall.h>
#include <aros/asmcall.h>
#include <aros/symbolsets.h>
#include <libraries/mpega.h>

#include "mpega_intern.h"
#include "wrap_mpega.h"
#include "support.h"

#define LIBBASETYPE    struct PrivateBase
#define LIBBASETYPEPTR LIBBASETYPE *

AROS_LH2(MPEGA_STREAM *, MPEGA_open,
    AROS_LHA(char *, stream_name, A0),
    AROS_LHA(MPEGA_CTRL *, ctrl, A1),
    LIBBASETYPEPTR, PrivateBase, 5, mpega)
{
    AROS_LIBFUNC_INIT

    return WRAP_MPEGA_open(stream_name, ctrl, 0);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, MPEGA_close,
    AROS_LHA(MPEGA_STREAM *, mpega_stream, A0),
    LIBBASETYPEPTR, PrivateBase, 6, mpega)
{
    AROS_LIBFUNC_INIT

    WRAP_MPEGA_close(mpega_stream, 0);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(LONG, MPEGA_decode_frame,
    AROS_LHA(MPEGA_STREAM *, mpega_stream, A0),
    AROS_LHA(WORD *, pcm[MPEGA_MAX_CHANNELS], A1),
    LIBBASETYPEPTR, PrivateBase, 7, mpega)
{
    AROS_LIBFUNC_INIT

    return WRAP_MPEGA_decode_frame(mpega_stream, pcm, 0);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(LONG, MPEGA_seek,
    AROS_LHA(MPEGA_STREAM *, mpega_stream, A0),
    AROS_LHA(ULONG, ms_time_position, D0),
    LIBBASETYPEPTR, PrivateBase, 8, mpega)
{
    AROS_LIBFUNC_INIT

    return WRAP_MPEGA_seek(mpega_stream, ms_time_position, 0);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(LONG, MPEGA_time,
    AROS_LHA(MPEGA_STREAM *, mpega_stream, A0),
    AROS_LHA(ULONG *, ms_time_position, A1),
    LIBBASETYPEPTR, PrivateBase, 9, mpega)
{
    AROS_LIBFUNC_INIT

    return WRAP_MPEGA_time(mpega_stream, ms_time_position);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(LONG, MPEGA_find_sync,
    AROS_LHA(UBYTE *, buffer, A0),
    AROS_LHA(LONG, buffer_size, D0),
    LIBBASETYPEPTR, PrivateBase, 10, mpega)
{
    AROS_LIBFUNC_INIT

    return WRAP_MPEGA_find_sync(buffer, buffer_size);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(LONG, MPEGA_scale,
    AROS_LHA(MPEGA_STREAM *, mpega_stream, A0),
    AROS_LHA(LONG, scale_percent, D0),
    LIBBASETYPEPTR, PrivateBase, 11, mpega)
{
    AROS_LIBFUNC_INIT

    return WRAP_MPEGA_scale(mpega_stream, scale_percent);

    AROS_LIBFUNC_EXIT
}

ADD2INITLIB(InitSupport, 0);
ADD2EXPUNGELIB(RemoveSupport, 0);

