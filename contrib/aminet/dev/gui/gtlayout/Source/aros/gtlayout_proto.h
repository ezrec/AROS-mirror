#ifndef PROTO_GTLAYOUT_H
#define PROTO_GTLAYOUT_H

/*
    (C) 1995-2003 AROS - The Amiga Research OS
*/

#ifndef AROS_SYSTEM_H
#   include <aros/system.h>
#endif

//#include <clib/gtlayout_protos.h>

#if defined(_AMIGA) && defined(__GNUC__)
#   include <inline/gtlayout.h>
#else
#   include <defines/gtlayout.h>
#endif

#endif /* PROTO_GTLAYOUT_H */
