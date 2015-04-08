/*
**
** $Id$
**
*/

#ifndef V_PLUGIN_H
#define V_PLUGIN_H

/*
** Voyager Plugin definitions
** ==========================
**
** (C) 1997-2001 Oliver Wagner <owagner@vapor.com>
** All Rights Reserved
**
** Revision 2 (12-10-97)
** ---------------------
** - extended comments a bit
**
** Revision 3 (04-07-99)
** ---------------------
** - added VPLUG_Query_APIVersion
** - added VPLUG_Query_RegisterMIMEType
** - added function table
**
** Revision 4 (09-07-99)
** ---------------------
** - added vplug_domethoda()
**
** Revision 5 (18-07-99)
** ---------------------
** - added vplug_seturl()
** - added vplug_mergeurl()
** - added VPLUG_EmbedInfo_ParentURL
** - added VPLUG_EmbedInfo_Baseref
**
** Revision 6 (23-01-00)
** ---------------------
** - added VPLUG_Query_RegisterMIMEExtension
** - added VPLUG_GetInfo()
** - fixed for newer GCC and vbcc (Emmanuel Lesueur)
**
** Revision 7 (04-03-00)
** ---------------------
** - added VPLUG_Query_HasURLMethodGetSize
**   and the related plugin entry point
*/


#ifndef __reg
#ifdef __PPC__
#define __reg(x,y)  y
#elif defined _DCC
#define __reg(x,y) __ ## x y
#elif defined __GNUC__
#define __reg(x,y) y __asm__(#x)
#else
#define __reg(x,y) register __ ## x y
#endif
#endif

#ifndef PLFUNC
#if defined __MAXON__ || defined __STORM__ || defined _DCC || defined __GNUC__
#define PLFUNC
#else
#define PLFUNC __asm
#endif
#endif

#ifndef SAVEDS
#if defined __MAXON__ || defined __GNUC__
#define SAVEDS
#endif
#if defined __STORM__ || defined __SASC
#define SAVEDS __saveds
#endif
#ifdef _DCC
#define SAVEDS __geta4
#endif
#endif

#include <exec/types.h>
#ifdef __STORM__
#include <exec/libraries.h>
#endif
/* ^ needed for the #pragma libbase below  */

#include <exec/types.h>
#include <utility/tagitem.h>
#include <utility/hooks.h>

#define VPLUG_TAGBASE (TAG_USER+0x87112)

/*
** VPLUG_Query() is supposed to return a static TagList
** which describes the ability and requirements of
** a plugin.
**
** Everything is described in the autodocs
** 
*/

/*
** V1 = Voyager 2.95 and before
** V2 = Voyager 3.0
** V3 = Voyager 3.1
*/
#define VPLUG_API_VERSION 3

#define VPLUG_QUERYBASE (VPLUG_TAGBASE+100)

#define VPLUG_Query_Version (VPLUG_QUERYBASE+0)                         /* ULONG */
#define VPLUG_Query_Revision (VPLUG_QUERYBASE+1)                        /* ULONG  */
#define VPLUG_Query_Copyright (VPLUG_QUERYBASE+2)                       /* STRPTR */
#define VPLUG_Query_Infostring (VPLUG_QUERYBASE+3)                      /* STRPTR */
#define VPLUG_Query_APIVersion (VPLUG_QUERYBASE+6)                              /* ULONG */
#define VPLUG_Query_HasPrefs (VPLUG_QUERYBASE+7)                        /* LONG */
#define VPLUG_Query_PluginID (VPLUG_QUERYBASE+8)                        /* STRPTR */
#define VPLUG_Query_RegisterURLMethod (VPLUG_QUERYBASE+4)       /* STRPTR */
#define VPLUG_Query_HasURLMethodGetSize (VPLUG_QUERYBASE+10)    /* BOOL */
#define VPLUG_Query_RegisterMIMEType (VPLUG_QUERYBASE+5)                /* STRPTR */
#define VPLUG_Query_RegisterMIMEExtension (VPLUG_QUERYBASE+9)   /* STRPTR */

/*
** Arguments passed to <EMBED>
** ArgNames is an array of STRPTR with the names,
** ArgValues is an array of STRPTR with the values, or ""
** ArgCnt is the number of arguments
*/
#define VPLUG_EmbedInfo_ArgNames (VPLUG_QUERYBASE+4002)
#define VPLUG_EmbedInfo_ArgValues (VPLUG_QUERYBASE+4004)
#define VPLUG_EmbedInfo_ArgCnt (VPLUG_QUERYBASE+4005)
#define VPLUG_EmbedInfo_ParentURL (VPLUG_QUERYBASE+4006)
#define VPLUG_EmbedInfo_Baseref (VPLUG_QUERYBASE+4007)

/*
** The complete URL of the SRC (<EMBED SRC="...">)
*/
#define VPLUG_EmbedInfo_URL (VPLUG_QUERYBASE+4000)                              /* STRPTR */

/*
** The complete URL of the page containing the <EMBED>
*/
#define VPLUG_EmbedInfo_Container (VPLUG_QUERYBASE+4001)                /* STRPTR */


/* APTR handle to network stream, already opened. Do NOT vplug_nets_close()
** it! The plugin can call vplug_settofile() or vplug_settomem() to
** get the file to whereever it wants the data. The network handler
** then does send VPLUG_NetStream_GotData and VPLUG_NetStream_GotDone
** methods to the embedded objects, informing of download progress.
*/
#define VPLUG_EmbedInfo_NetStream (VPLUG_QUERYBASE+4003)

/*
** Those are methods sent to the plugin.
** GotInfo is sent when V got all the header of the stream,
** GotData is sent everytime V gets one chunk of data and
** GotDone is sent when V is done with the loading of the
** stream
*/
#define VPLUG_NetStream_GotInfo 0x851ba045
#define VPLUG_NetStream_GotData 0x851ba046
#define VPLUG_NetStream_GotDone 0x851ba047


/****************************************************************************/

/*
** Callback table
** If a plugin implements API spec >= 2, V will call
** VPLUG_Setup() with a pointer to this
** object. The plugin can call functions in that
** table anytime.
*/
#define VPLUG_FUNCTABVERSION 6

struct vplug_functable 
{
	int vplug_functabversion;

	APTR context; /* ***PRIVATE!!! DO NOT TOUCH!!!*** */

	APTR (PLFUNC * vplug_net_openurl)(
			__reg(a0, STRPTR url),
			__reg(a1, STRPTR referer),
			__reg(a2, APTR informobj),
			__reg(d0, int reload)
	);

	int (PLFUNC * vplug_net_state)(
			__reg(a0, APTR nethandle)
	);

	void (PLFUNC * vplug_net_close)(
			__reg(a0, APTR nethandle)
	);

	STRPTR (PLFUNC * vplug_net_mimetype)(
			__reg(a0, APTR nethandle)
	);

	APTR (PLFUNC * vplug_net_getdocmem)(
			__reg(a0, APTR nethandle)
	);

	int (PLFUNC * vplug_net_getdocptr)(
			__reg(a0, APTR nethandle)
	);
	
	int (PLFUNC * vplug_net_getdoclen)(
			__reg(a0, APTR nethandle)
	);

	void (PLFUNC * vplug_net_settomem)(
			__reg(a0, APTR nethandle)
	);

	void (PLFUNC * vplug_net_settofile)(
			__reg(a0, APTR nethandle),
			__reg(a1, STRPTR filename),
			__reg(d0, int resume)
	);

	STRPTR (PLFUNC * vplug_net_redirecturl)(
			__reg(a0, APTR nethandle)
	);
	
	STRPTR (PLFUNC * vplug_net_url)(
			__reg(a0, APTR nethandle)
	);

	void (PLFUNC * vplug_net_lockdocmem)(void);
	
	void (PLFUNC * vplug_net_unlockdocmem)(void);

	void (PLFUNC * vplug_net_abort)(
			__reg(a0, APTR nethandle)
	);

	STRPTR (PLFUNC * vplug_net_errorstring)(                        
			__reg(a0, APTR nethandle)
	);

	int (PLFUNC * vplug_domethoda)(
		__reg(a0, APTR obj),
		__reg(a1, APTR Msg)
	);

	void (PLFUNC * vplug_seturl)(
		__reg(a0, STRPTR url),
		__reg(a1, STRPTR target),
		__reg(d0, int reload)
	);

	void (PLFUNC * vplug_mergeurl)(
		__reg(a0, STRPTR url),
		__reg(a1, STRPTR partial),
		__reg(a2, STRPTR dest)
	);
	
	void (PLFUNC * vplug_colorspec2rgb)(
		__reg(a0, STRPTR colorspec),
		__reg(d0, ULONG *red),
		__reg(d1, ULONG *green),
		__reg(d2, ULONG *blue)
	);
};

/*
 * Macros to call the functions above. This allows things to
 * work on MorphOS.
 */
#ifdef __PPC__

#define VPLUG_NET_OPENURL(table,a,b,c,d) \
    ({                                                         \
	 struct EmulCaos MyCaos;                               \
	 MyCaos.reg_a0 = (ULONG) (a);                          \
	 MyCaos.reg_a1 = (ULONG) (b);                          \
	 MyCaos.reg_a2 = (ULONG) (c);                          \
	 MyCaos.reg_d0 = (ULONG) (d);                          \
	 MyCaos.caos_Un.Function = (table)->vplug_net_openurl; \
	 (APTR)(*MyEmulHandle->EmulCall68k)(&MyCaos);          \
     })

#define VPLUG_NET_STATE(table,a) \
    ({                                                         \
	 struct EmulCaos MyCaos;                               \
	 MyCaos.reg_a0 = (ULONG) (a);                          \
	 MyCaos.caos_Un.Function = (table)->vplug_net_state;   \
	 (int)(*MyEmulHandle->EmulCall68k)(&MyCaos);           \
     })

#define VPLUG_NET_CLOSE(table,a) \
    ({                                                         \
	 struct EmulCaos MyCaos;                               \
	 MyCaos.reg_a0 = (ULONG) (a);                          \
	 MyCaos.caos_Un.Function = (table)->vplug_net_close;   \
	 (*MyEmulHandle->EmulCall68k)(&MyCaos);                \
     })

#define VPLUG_NET_MIMETYPE(table,a) \
    ({                                                         \
	 struct EmulCaos MyCaos;                               \
	 MyCaos.reg_a0 = (ULONG) (a);                          \
	 MyCaos.caos_Un.Function = (table)->vplug_net_mimetype;\
	 (STRPTR)(*MyEmulHandle->EmulCall68k)(&MyCaos);        \
     })

#define VPLUG_NET_GETDOCMEM(table,a) \
    ({                                                         \
	 struct EmulCaos MyCaos;                               \
	 MyCaos.reg_a0 = (ULONG) (a);                          \
	 MyCaos.caos_Un.Function = (table)->vplug_net_getdocmem;\
	 (APTR)(*MyEmulHandle->EmulCall68k)(&MyCaos);          \
     })

#define VPLUG_NET_GETDOCPTR(table,a) \
    ({                                                         \
	 struct EmulCaos MyCaos;                               \
	 MyCaos.reg_a0 = (ULONG) (a);                          \
	 MyCaos.caos_Un.Function = (table)->vplug_net_getdocptr;\
	 (int)(*MyEmulHandle->EmulCall68k)(&MyCaos);           \
     })
	
#define VPLUG_NET_GETDOCLEN(table,a) \
    ({                                                         \
	 struct EmulCaos MyCaos;                               \
	 MyCaos.reg_a0 = (ULONG) (a);                          \
	 MyCaos.caos_Un.Function = (table)->vplug_net_getdoclen;\
	 (int)(*MyEmulHandle->EmulCall68k)(&MyCaos);           \
     })

#define VPLUG_NET_SETTOMEM(table,a) \
    ({                                                         \
	 struct EmulCaos MyCaos;                               \
	 MyCaos.reg_a0 = (ULONG) (a);                          \
	 MyCaos.caos_Un.Function = (table)->vplug_net_settomem;\
	 (*MyEmulHandle->EmulCall68k)(&MyCaos);                \
     })

#define VPLUG_NET_SETTOFILE(table,a,b,c) \
    ({                                                         \
	 struct EmulCaos MyCaos;                               \
	 MyCaos.reg_a0 = (ULONG) (a);                          \
	 MyCaos.reg_a1 = (ULONG) (b);                          \
	 MyCaos.reg_d0 = (ULONG) (c);                          \
	 MyCaos.caos_Un.Function = (table)->vplug_net_settofile;\
	 (*MyEmulHandle->EmulCall68k)(&MyCaos);                \
     })

#define VPLUG_NET_REDIRECTURL(table,a) \
    ({                                                         \
	 struct EmulCaos MyCaos;                               \
	 MyCaos.reg_a0 = (ULONG) (a);                          \
	 MyCaos.caos_Un.Function = (table)->vplug_net_redirecturl;\
	 (STRPTR)(*MyEmulHandle->EmulCall68k)(&MyCaos);        \
     })
	
#define VPLUG_NET_URL(table,a) \
    ({                                                         \
	 struct EmulCaos MyCaos;                               \
	 MyCaos.reg_a0 = (ULONG) (a);                          \
	 MyCaos.caos_Un.Function = (table)->vplug_net_url;     \
	 (STRPTR)(*MyEmulHandle->EmulCall68k)(&MyCaos);        \
     })

#define VPLUG_NET_LOCKDOCMEM(table) \
    ({                                                         \
	 struct EmulCaos MyCaos;                               \
	 MyCaos.caos_Un.Function = (table)->vplug_net_lockdocmem;\
	 (*MyEmulHandle->EmulCall68k)(&MyCaos);                \
     })
	
#define VPLUG_NET_UNLOCKDOCMEM(table) \
    ({                                                         \
	 struct EmulCaos MyCaos;                               \
	 MyCaos.caos_Un.Function = (table)->vplug_net_unlockdocmem;\
	 (*MyEmulHandle->EmulCall68k)(&MyCaos);                \
     })

#define VPLUG_NET_ABORT(table,a) \
    ({                                                         \
	 struct EmulCaos MyCaos;                               \
	 MyCaos.reg_a0 = (ULONG) (a);                          \
	 MyCaos.caos_Un.Function = (table)->vplug_net_abort;   \
	 (*MyEmulHandle->EmulCall68k)(&MyCaos);                \
     })

#define VPLUG_NET_ERRORSTRING(table,a) \
    ({                                                         \
	 struct EmulCaos MyCaos;                               \
	 MyCaos.reg_a0 = (ULONG) (a);                          \
	 MyCaos.caos_Un.Function = (table)->vplug_net_errorstring;\
	 (STRPTR)(*MyEmulHandle->EmulCall68k)(&MyCaos);        \
     })

#define VPLUG_DOMETHODA(table,a,b) \
    ({                                                         \
	 struct EmulCaos MyCaos;                               \
	 MyCaos.reg_a0 = (ULONG) (a);                          \
	 MyCaos.reg_a1 = (ULONG) (b);                          \
	 MyCaos.caos_Un.Function = (table)->vplug_net_domethoda;\
	 (int)(*MyEmulHandle->EmulCall68k)(&MyCaos);           \
     })

#define VPLUG_SETURL(table,a,b,c) \
    ({                                                         \
	 struct EmulCaos MyCaos;                               \
	 MyCaos.reg_a0 = (ULONG) (a);                          \
	 MyCaos.reg_a1 = (ULONG) (b);                          \
	 MyCaos.reg_d0 = (ULONG) (c);                          \
	 MyCaos.caos_Un.Function = (table)->vplug_seturl;      \
	 (*MyEmulHandle->EmulCall68k)(&MyCaos);                \
     })

#define VPLUG_MERGEURL(table,a,b,c) \
    ({                                                         \
	 struct EmulCaos MyCaos;                               \
	 MyCaos.reg_a0 = (ULONG) (a);                          \
	 MyCaos.reg_a1 = (ULONG) (b);                          \
	 MyCaos.reg_a2 = (ULONG) (c);                          \
	 MyCaos.caos_Un.Function = (table)->vplug_mergeurl;    \
	 (*MyEmulHandle->EmulCall68k)(&MyCaos);                \
     })
	
#define VPLUG_COLORSPEC2RGB(table,a,b,c,d) \
    ({                                                         \
	 struct EmulCaos MyCaos;                               \
	 MyCaos.reg_a0 = (ULONG) (a);                          \
	 MyCaos.reg_d0 = (ULONG) (b);                          \
	 MyCaos.reg_d1 = (ULONG) (c);                          \
	 MyCaos.reg_d2 = (ULONG) (d);                          \
	 MyCaos.caos_Un.Function = (table)->vplug_colorspec2rgb;\
	 (*MyEmulHandle->EmulCall68k)(&MyCaos);                \
     })

#else

#define VPLUG_NET_OPENURL(t,a,b,c,d) \
	(*(t)->vplug_net_openurl)((a),(b),(c),(d))

#define VPLUG_NET_STATE(t,a) \
	(*(t)->vplug_net_state)((a))

#define VPLUG_NET_CLOSE(t,a) \
	(*(t)->vplug_net_close)((a))

#define VPLUG_NET_MIMETYPE(t,a) \
	(*(t)->vplug_net_mimetype)((a))

#define VPLUG_NET_GETDOCMEM(t,a) \
	(*(t)->vplug_net_getdocmem)((a))

#define VPLUG_NET_GETDOCPTR(t,a) \
	(*(t)->vplug_net_getdocptr)((a))
	
#define VPLUG_NET_GETDOCLEN(t,a) \
	(*(t)->vplug_net_getdoclen)((a))

#define VPLUG_NET_SETTOMEM(t,a) \
	(*(t)->vplug_net_settomem)((a))

#define VPLUG_NET_SETTOFILE(t,a,b,c) \
	(*(t)->vplug_net_settofile)((a),(b),(c))

#define VPLUG_NET_REDIRECTURL(t,a) \
	(*(t)->vplug_net_redirecturl)((a))
	
#define VPLUG_NET_URL(t,a) \
	(*(t)->vplug_net_url)((a))

#define VPLUG_NET_LOCKDOCMEM(t) \
	(*(t)->vplug_net_lockdocmem)()
	
#define VPLUG_NET_UNLOCKDOCMEM(t) \
	(*(t)->vplug_net_unlockdocmem)()

#define VPLUG_NET_ABORT(t,a) \
	(*(t)->vplug_net_abort)((a))

#define VPLUG_NET_ERRORSTRING(t,a) \
	(*(t)->vplug_net_errorstring)((a))

#define VPLUG_DOMETHODA(t,a,b) \
	(*(t)->vplug_domethoda)((a),(b))

#define VPLUG_SETURL(t,a,b,c) \
	(*(t)->vplug_seturl)((a),(b),(c))

#define VPLUG_MERGEURL(t,a,b,c) \
	(*(t)->vplug_mergeurl)((a),(b),(c))
	
#define VPLUG_COLORSPEC2RGB(t,a,b,c,d) \
	(*(t)->vplug_colorspec2rgb)((a),(b),(c),(d))

#endif


/*
 * Network handle states
 */
	/* states */
#define VNS_FAILED        -1
#define VNS_INPROGRESS 0
#define VNS_DONE           1
 

/****************************************************************************/

/*
** holds your plugin's custom preference page information
** A pointer to this structure, which is already allocated for you 
** by Voyager, is passed to the VPLUG_Hook_Prefs() function.
*/
struct vplug_prefs {
	char *label;                    /* list item label, defaults to plugin name */
	struct BitMap *bitmap;  /* 24x14 list icon bitmap, defaults to plugin image */
	APTR colormap;                  /* bitmap's colormap, defaults to MWB palette (8 col.) */
	APTR object;                    /* preferences object */
};

/* 
** These are the methods you are expected to handle in _Hook_Prefs()
** whenever Voyager wants to know or do stuff with your prefs.
** The description of the method id is what Voyager expects you to
** do or what to return when it is requested. Check the autodocs for more infos
*/
enum {
	VPLUGPREFS_first = 16384,
	VPLUGPREFS_Setup,
	VPLUGPREFS_Cleanup,
	VPLUGPREFS_Create,
	VPLUGPREFS_Dispose,
	VPLUGPREFS_Use,
	VPLUGPREFS_Load,
	VPLUGPREFS_Save,
};

/* where to store the settings */
#define VPLUGPREFSPATH "PROGDIR:Plugins/Data"


/****************************************************************************/

/*
** Plugin library calls
*/

#ifndef BUILDPLUGIN


#ifndef NO_PROTO

struct TagItem *VPLUG_Query( void );
APTR VPLUG_ProcessURLMethod( STRPTR url );
APTR VPLUG_GetURLData( APTR handle );
STRPTR VPLUG_GetURLMIMEType( APTR handle ); 
void VPLUG_FreeURLData( APTR handle );
APTR VPLUG_GetClass( STRPTR mimetype );

/*
 * Embedding objects, asking for size. This structure might extend in the future.
 * Just modify the fields of VPlugInfo if you need to.
 */
struct VPlugInfo {
	ULONG x;                        /* horizontal size */
	ULONG y;                        /* vertical size */
	STRPTR wintitle;    /* title of the window */
};

BOOL VPLUG_GetInfo( struct VPlugInfo *, APTR nethandle );
BOOL VPLUG_Setup( struct vplug_functable *table );
void VPLUG_FinalSetup( void );
void VPLUG_Cleanup( void );
void VPLUG_Hook_Prefs( ULONG methodid, struct vplug_prefs *prefs );
int VPLUG_GetURLDataSize( APTR handle );

#endif /* NO_PROTO */

#ifndef NO_PRAGMAS
#ifdef __GNUC__

#ifdef __PPC__

#define VPLUG_Query() \
	LP0(0x1e, struct TagItem*, VPLUG_Query, \
	, classlib, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define VPLUG_ProcessURLMethod(url) \
	LP1(0x24, APTR, VPLUG_ProcessURLMethod, STRPTR, url, a0, \
	, classlib, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define VPLUG_GetURLData(handle) \
	LP1(0x2a, APTR, VPLUG_GetURLData, APTR, handle, a0, \
	, classlib, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define VPLUG_GetURLMIMEType(handle) \
	LP1(0x30, APTR, VPLUG_GetURLMIMEType, APTR, handle, a0, \
	, classlib, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define VPLUG_FreeURLData(handle) \
	LP1NR(0x36, VPLUG_FreeURLData, APTR, handle, a0, \
	, classlib, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define VPLUG_GetClass(mimetype) \
	LP1(0x3c, APTR, VPLUG_GetClass, STRPTR, mimetype, a0, \
	, classlib, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define VPLUG_Setup(table) \
	LP1(0x42, BOOL, VPLUG_Setup, struct vplug_functable *, table, a0, \
	, classlib, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define VPLUG_Cleanup() \
	LP0NR(0x48, VPLUG_Cleanup, \
	, classlib, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define VPLUG_FinalSetup() \
	LP0NR(0x4e, VPLUG_FinalSetup, \
	, classlib, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define VPLUG_Hook_Prefs(methodid, prefs) \
	LP2NR(0x54, VPLUG_Hook_Prefs, ULONG, methodid, d0, struct vplug_prefs *, prefs, a0, \
	, classlib, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define VPLUG_GetInfo(info, handle) \
	LP2(0x5a, BOOL, VPLUG_GetInfo, struct VPlugInfo *, info, a0, APTR, handle, a1, \
	, classlib, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define VPLUG_GetURLDataSize(handle) \
	LP1(0x60, int, VPLUG_GetURLDataSize, APTR, handle, a0, \
	, classlib, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#else

#define VPLUG_Query() \
	LP0(0x1e, struct TagItem*, VPLUG_Query, \
	, classlib)

#define VPLUG_ProcessURLMethod(url) \
	LP1(0x24, APTR, VPLUG_ProcessURLMethod, STRPTR, url, a0, \
	, classlib)

#define VPLUG_GetURLData(handle) \
	LP1(0x2a, APTR, VPLUG_GetURLData, APTR, handle, a0, \
	, classlib)

#define VPLUG_GetURLMIMEType(handle) \
	LP1(0x30, APTR, VPLUG_GetURLMIMEType, APTR, handle, a0, \
	, classlib)

#define VPLUG_FreeURLData(handle) \
	LP1NR(0x36, VPLUG_FreeURLData, APTR, handle, a0, \
	, classlib)

#define VPLUG_GetClass(mimetype) \
	LP1(0x3c, APTR, VPLUG_GetClass, STRPTR, mimetype, a0, \
	, classlib)

#define VPLUG_Setup(table) \
	LP1(0x42, BOOL, VPLUG_Setup, struct vplug_functable *, table, a0, \
	, classlib)

#define VPLUG_Cleanup() \
	LP0NR(0x48, VPLUG_Cleanup, \
	, classlib)

#define VPLUG_FinalSetup() \
	LP0NR(0x4e, VPLUG_FinalSetup, \
	, classlib)

#define VPLUG_Hook_Prefs(methodid, prefs) \
	LP2NR(0x54, VPLUG_Hook_Prefs, ULONG, methodid, d0, struct vplug_prefs *, prefs, a0, \
	, classlib)

#define VPLUG_GetInfo(info, handle) \
	LP2(0x5a, BOOL, VPLUG_GetInfo, struct VPlugInfo *, info, a0, APTR, handle, a1, \
	, classlib)

#define VPLUG_GetURLDataSize(handle) \
	LP1(0x60, int, VPLUG_GetURLDataSize, APTR, handle, a0, \
	, classlib)

#endif

#else
#pragma libcall classlib VPLUG_Query 1e 0
#pragma libcall classlib VPLUG_ProcessURLMethod 24 801
#pragma libcall classlib VPLUG_GetURLData 2a 801
#pragma libcall classlib VPLUG_GetURLMIMEType 30 801
#pragma libcall classlib VPLUG_FreeURLData 36 801
/* V2 API additions */
#pragma libcall classlib VPLUG_GetClass 3c 801
#pragma libcall classlib VPLUG_Setup 42 801
#pragma libcall classlib VPLUG_Cleanup 48 0
#pragma libcall classlib VPLUG_FinalSetup 4e 0
#pragma libcall classlib VPLUG_Hook_Prefs 54 8002
/* V3 API additions */
#pragma libcall classlib VPLUG_GetInfo 5a 9802
#pragma libcall classlib VPLUG_GetURLDataSize 60 801
#endif
#endif

#endif /* BUILDPLUGIN */

#endif /* V_PLUGIN_H */
