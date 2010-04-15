/* Automatically generated header! Do not edit! */

#ifndef PROTO_USERGROUP_H
#define PROTO_USERGROUP_H

#ifndef __NOLIBBASE__
extern struct Library *
#ifdef __CONSTLIBBASEDECL__
__CONSTLIBBASEDECL__
#endif /* __CONSTLIBBASEDECL__ */
UserGroupBase;
#endif /* !__NOLIBBASE__ */

#ifdef __amigaos4__
#include <interfaces/usergroup.h>
#ifdef __USE_INLINE__
#include <inline4/usergroup.h>
#endif /* __USE_INLINE__ */
#ifndef CLIB_USERGROUP_PROTOS_H
#define CLIB_USERGROUP_PROTOS_H
#endif /* CLIB_USERGROUP_PROTOS_H */
#ifndef __NOGLOBALIFACE__
extern struct UsergroupIFace *IUsergroup;
#endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
#include <clib/usergroup_protos.h>
#ifdef __GNUC__
#ifdef __AROS__
#ifndef NOLIBDEFINES
#ifndef USERGROUP_NOLIBDEFINES
#include <defines/usergroup.h>
#endif /* USERGROUP_NOLIBDEFINES */
#endif /* NOLIBDEFINES */
#else
#ifdef __PPC__
#ifndef _NO_PPCINLINE
#include <ppcinline/usergroup.h>
#endif /* _NO_PPCINLINE */
#else
#ifndef _NO_INLINE
#include <inline/usergroup.h>
#endif /* _NO_INLINE */
#endif /* __PPC__ */
#endif /* __AROS__ */
#else
#include <pragmas/usergroup_pragmas.h>
#endif /* __GNUC__ */
#endif /* __amigaos4__ */

#endif /* !PROTO_USERGROUP_H */
