#ifndef _LIBRARIES_SHUTDOWN_H
#define _LIBRARIES_SHUTDOWN_H 1

#ifndef _EXEC_TYPES_H
#include <exec/types.h>
#endif	/* !_EXEC_TYPES_H */

#ifndef _EXEC_NODES_H
#include <exec/nodes.h>
#endif	/* !_EXEC_NODES_H */

#define SDB_RESET	0
#define SDF_RESET	(1 << 0)

#define SD_CHECK	0
#define SD_EXIT		1

#define SHUTDOWN_NORMAL	0
#define SHUTDOWN_FAST	1

struct ShutdownAction
{
	ULONG	sdm_Mode;
};

struct ShutdownInfo
{
	ULONG	sdi_Flags;
};

#ifdef AZTEC_C

#ifndef __VERSION
#define __VERSION 360
#endif	/* __VERSION */

#if __VERSION < 500

#ifdef __NO_PROTOS
#undef __NO_PROTOS
#endif	/* __NO_PROTOS */

#ifdef __NO_PRAGMAS
#undef __NO_PRAGMAS
#endif	/* __NO_PRAGMAS */

#define __NO_PROTOS	1
#define __NO_PRAGMAS	1

#endif	/* __VERSION */

#endif	/* AZTEC_C */

#ifdef __ARGS
#undef __ARGS
#endif	/* __ARGS */

#ifdef __NO_PROTOS
#define __ARGS(x) ()
#else
#define __ARGS(x) x
#endif	/* __NO_PROTOS */

VOID	Shutdown __ARGS((ULONG Mode));
APTR	AddShutdownInfoTagList __ARGS((struct Hook *Hook,STRPTR Name,struct TagItem *TagList));
APTR	AddShutdownInfoTags __ARGS((struct Hook *Hook,STRPTR Name,...));
LONG	RemShutdownInfo __ARGS((APTR Result));

#ifndef __NO_PRAGMAS
#ifndef __NO_PROTOS

#ifdef LATTICE
#pragma libcall ShutdownBase AddShutdownInfoTagList 24 a9803
#pragma libcall ShutdownBase RemShutdownInfo 2a 801
#pragma libcall ShutdownBase Shutdown 30 1
#endif	/* LATTICE */

#ifdef AZTEC_C
#pragma amicall(ShutdownBase, 0x24, AddShutdownInfoTagList(a0,a1,a2))
#pragma amicall(ShutdownBase, 0x2a, RemShutdownInfo(a0))
#pragma amicall(ShutdownBase, 0x30, Shutdown(d0))
#endif	/* AZTEC_C */

#endif	/* !__NO_PROTOS */
#endif	/* !__NO_PRAGMAS */

#endif	/* _LIBRARIES_SHUTDOWN_H */
