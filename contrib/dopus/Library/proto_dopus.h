/*
    (C) 1995-97 AROS - The Amiga Research OS
    $Id$
*/
#ifndef PROTO_DOPUS_H
#define PROTO_DOPUS_H

#ifndef AROS_SYSTEM_H
#include <aros/system.h>
#endif

#include <clib/dopus_protos.h>

#if defined(_AMIGA) && defined(__GNUC__)
#include <inline/dopus.h>
#else
#include <defines/dopus.h>
#endif

#endif /* PROTO_DOPUS_H */
