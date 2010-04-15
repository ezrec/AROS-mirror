/* Automatically generated header! Do not edit! */

#ifndef PROTO_IDENTIFY_H
#define PROTO_IDENTIFY_H

#ifndef __NOLIBBASE__
extern struct Library *
#ifdef __CONSTLIBBASEDECL__
__CONSTLIBBASEDECL__
#endif /* __CONSTLIBBASEDECL__ */
IdentifyBase;
#endif /* !__NOLIBBASE__ */

#ifdef __amigaos4__
#include <interfaces/identify.h>
#ifdef __USE_INLINE__
#include <inline4/identify.h>
#endif /* __USE_INLINE__ */
#ifndef CLIB_IDENTIFY_PROTOS_H
#define CLIB_IDENTIFY_PROTOS_H
#endif /* CLIB_IDENTIFY_PROTOS_H */
#ifndef __NOGLOBALIFACE__
extern struct IdentifyIFace *IIdentify;
#endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
#include <clib/identify_protos.h>
#ifdef __GNUC__
#ifdef __AROS__
#ifndef NOLIBDEFINES
#ifndef IDENTIFY_NOLIBDEFINES
#include <defines/identify.h>
#endif /* IDENTIFY_NOLIBDEFINES */
#endif /* NOLIBDEFINES */
#else
#ifdef __PPC__
#ifndef _NO_PPCINLINE
#include <ppcinline/identify.h>
#endif /* _NO_PPCINLINE */
#else
#ifndef _NO_INLINE
#include <inline/identify.h>
#endif /* _NO_INLINE */
#endif /* __PPC__ */
#endif /* __AROS__ */
#else
#include <pragmas/identify_pragmas.h>
#endif /* __GNUC__ */
#endif /* __amigaos4__ */

#endif /* !PROTO_IDENTIFY_H */
