// plugin.h
// $Date$
// $Revision$

// Include "plugin_data.h" without PLUGIN_H_INCLUDED defined to make it also work
// when "plugin.h" is (indirectly) included from "plugin_data.h"
#include "plugin_data.h"

#ifndef PLUGIN_H_INCLUDED
#define PLUGIN_H_INCLUDED

#include <exec/types.h>
#include <exec/libraries.h>

#define OOP		1
#define FILETYPE	2
#define PREVIEW		3
#define PREFS		4

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack(2)
#endif /* __GNUC__ */

// DeviceFilter library base
struct PluginBase
	{
	struct Library pl_LibNode;
#if PLUGIN_TYPE == OOP
	ULONG pl_PlugID;
#endif
	BPTR pl_SegList;
	UBYTE pl_Initialized;
	};

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack()
#endif /* __GNUC__ */
#endif /* PLUGIN_H_INCLUDED */

BOOL initPlugin(struct PluginBase *pluginbase);
VOID closePlugin(struct PluginBase *pluginbase);

#if PLUGIN_TYPE == OOP
#include <intuition/classes.h>

M68KFUNC_P3_PROTO(ULONG, CI_HOOKFUNC,
	A0, Class *, cl,
	A2, Object *, obj,
	A1, Msg, msg);
#endif // PLUGIN_TYPE == OOP

#if PLUGIN_TYPE == FILETYPE
#include <scalos/scalos.h>

LIBFUNC_P3_PROTO(STRPTR, FT_INFOSTRING,
	A0, struct ScaToolTipInfoHookData *, ttshd,
	A1, CONST_STRPTR, args,
	A6, struct PluginBase *, PluginBase);
#endif // PLUGIN_TYPE == FILETYPE

#if PLUGIN_TYPE == PREVIEW
#include <scalos/scalos.h>

LIBFUNC_P5_PROTO(LONG, LIBSCAPreviewGenerate,
	A0, struct ScaWindowTask *, wt,
	A1, BPTR, dirLock,
	A2, CONST_STRPTR, iconName,
	A3, struct TagItem *, tagList,
	A6, struct PluginBase *, PluginBase);
#endif // PLUGIN_TYPE == PREVIEW

#if PLUGIN_TYPE == PREFS
LIBFUNC_P2_PROTO(ULONG, LIBSCAGetPrefsInfo,
	D0, ULONG, which,
	A6, struct PluginBase *, PluginBase);
#endif // PLUGIN_TYPE == PREFS

