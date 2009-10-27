/*
 * $Id$
 *
 * :ts=8
 *
 * 'Roadshow' -- Amiga TCP/IP stack; "usergroup.library" API
 * Copyright © 2001-2005 by Olaf Barthel.
 * All Rights Reserved.
 *
 * Amiga specific TCP/IP 'C' header files;
 * Freely Distributable
 *
 * WARNING: The "usergroup.library" API must be considered obsolete and
 *          should not be used in new software. It is provided solely
 *          for backwards compatibility and legacy application software.
 */

#ifndef CLIB_USERGROUP_PROTOS_H
#define CLIB_USERGROUP_PROTOS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef  LIBRARIES_USERGROUP_H
#include <libraries/usergroup.h>
#endif
#ifndef  PWD_H
#include <pwd.h>
#endif
#ifndef  GRP_H
#include <grp.h>
#endif

UBYTE *crypt( UBYTE *key, UBYTE *set );
STRPTR getpass( STRPTR prompt );
STRPTR getlogin( VOID );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CLIB_USERGROUP_PROTOS_H */
