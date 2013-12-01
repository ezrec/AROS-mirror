// devicefilter.h
// $Date$
// $Revision$

#ifndef DEVICEFILTER_H_INCLUDED
#define DEVICEFILTER_H_INCLUDED

#include <scalos/scalos.h>

#include "plugin.h"

//-----------------------------------------------------------------------------

#define	MEMPOOL_MEMFLAGS	MEMF_PUBLIC
#define	MEMPOOL_PUDDLESIZE	4096
#define	MEMPOOL_THRESHSIZE	4096

//----------------------------------------------------------------------------

#define	Sizeof(array)		(sizeof(array) / sizeof(array[0]))

//----------------------------------------------------------------------------

#define d(x)    ;
#define d1(x)   ;
#define d2(x)   x;

// from debug.lib
extern int kprintf(const char *fmt, ...);
extern int KPrintF(const char *fmt, ...);

//----------------------------------------------------------------------------

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack(2)
#endif /* __GNUC__ */

// DeviceFilter library base
struct DeviceFilterBase
	{
	struct PluginBase df_LibNode;
	};

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack()
#endif /* __GNUC__ */

// Structure of the messages we want to be looking at
struct DeviceList_Filter_Msg
{
	ULONG                   MethodID;
	struct ScalosNodeList   *devicenodes;
};

//----------------------------------------------------------------------------

#endif /* DEVICEFILTER_H_INCLUDED */

