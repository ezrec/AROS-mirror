#ifndef PROTO_USERGROUP_H
#define PROTO_USERGROUP_H
/*
    Copyright © 2003-2004, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef __NOLIBBASE__       
extern struct Library *UserGroupBase;
#endif

#include <libraries/usergroup.h>
#include <clib/usergroup_protos.h>
#if __SASC
#include <pragmas/usergroup_pragmas.h>
#elif __GNUC__
#include <inline/usergroup.h>
#endif

#endif /* PROTO_USERGROUP_H */
