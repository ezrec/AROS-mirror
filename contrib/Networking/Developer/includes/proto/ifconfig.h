#ifndef PROTO_IFCONFIG_H
#define PROTO_IFCONFIG_H
/*
    Copyright © 2003-2004, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef __NOLIBBASE__       
extern struct Library *IfConfigBase;
#endif

#include <libraries/ifconfig.h>
#include <clib/ifconfig_protos.h>
#if __SASC
#include <pragmas/ifconfig_pragmas.h>
#elif __GNUC__
#include <inline/ifconfig.h>
#endif

#endif /* PROTO_IFCONFIG_H */
