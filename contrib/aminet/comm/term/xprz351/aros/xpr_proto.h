#ifndef PROTO_XPR_H
#define PROTO_XPR_H

/*
    (C) 1995-2003 AROS - The Amiga Research OS
*/

#ifndef AROS_SYSTEM_H
#   include <aros/system.h>
#endif

#ifndef XProtocolBase
extern struct Library * XProtocolBase;
#endif

//#include <clib/xpr_protos.h>

#if defined(_AMIGA) && defined(__GNUC__)
#   include <inline/xpr.h>
#else
#   include <defines/xpr.h>
#endif

#endif /* PROTO_XPR_H */
