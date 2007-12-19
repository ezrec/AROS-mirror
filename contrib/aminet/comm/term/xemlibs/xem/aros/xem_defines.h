#ifndef DEFINES_XEM_PROTOS_H
#define DEFINES_XEM_PROTOS_H

#include <aros/libcall.h>
#include <exec/types.h>

#ifndef XEmulatorSetup
#define XEmulatorSetup(io) \
	AROS_LC1(BOOL  ,XEmulatorSetup, \
	AROS_LCA(  struct XEM_IO *,io,A0),\
	struct XEmulatorBase *, XEmulatorBase, 5, XEmulatorBase)
#endif

#ifndef XEmulatorCleanup
#define XEmulatorCleanup(io) \
	AROS_LC1NR(VOID  ,XEmulatorCleanup, \
	AROS_LCA(  struct XEM_IO *,io,A0),\
	struct XEmulatorBase *, XEmulatorBase, 8, XEmulatorBase)
#endif

#ifndef XEmulatorOpenConsole
#define XEmulatorOpenConsole(io) \
	AROS_LC1(BOOL  ,XEmulatorOpenConsole, \
	AROS_LCA(  struct XEM_IO *,io,A0),\
	struct XEmulatorBase *, XEmulatorBase, 6, XEmulatorBase)
#endif

#ifndef XEmulatorCloseConsole
#define XEmulatorCloseConsole(io) \
	AROS_LC1NR(VOID  ,XEmulatorCloseConsole, \
	AROS_LCA(  struct XEM_IO *,io,A0),\
	struct XEmulatorBase *, XEmulatorBase, 7, XEmulatorBase)
#endif

#ifndef XEmulatorWrite
#define XEmulatorWrite(io,buff,buflen) \
	AROS_LC3NR(VOID  ,XEmulatorWrite, \
	AROS_LCA(  struct XEM_IO *,io,A0),\
	AROS_LCA(  UBYTE *,buff,A1),\
	AROS_LCA(  ULONG,buflen,D0),\
	struct XEmulatorBase *, XEmulatorBase, 9, XEmulatorBase)
#endif

#ifndef XEmulatorSignal
#define XEmulatorSignal(io,sig) \
	AROS_LC2(BOOL  ,XEmulatorSignal, \
	AROS_LCA(  struct XEM_IO *,io,A0),\
	AROS_LCA(  ULONG,sig,D0),\
	struct XEmulatorBase *, XEmulatorBase, 10, XEmulatorBase)
#endif

#ifndef XEmulatorUserMon
#define XEmulatorUserMon(io,retstr,maxlen,imsg) \
	AROS_LC4(ULONG  ,XEmulatorUserMon, \
	AROS_LCA(  struct XEM_IO *,io,A0),\
	AROS_LCA(  UBYTE *,retstr,A1),\
	AROS_LCA(  ULONG,maxlen,D0),\
	AROS_LCA(  struct IntuiMessage *,imsg,A2),\
	struct XEmulatorBase *, XEmulatorBase, 12, XEmulatorBase)
#endif

#ifndef XEmulatorHostMon
#define XEmulatorHostMon(io,hostData,actual) \
	AROS_LC3(ULONG  ,XEmulatorHostMon, \
	AROS_LCA(  struct XEM_IO *,io,A0),\
	AROS_LCA(  struct XEmulatorHostData *,hostData,A1),\
	AROS_LCA(  ULONG,actual,D0),\
	struct XEmulatorBase *, XEmulatorBase, 11, XEmulatorBase)
#endif

#ifndef XEmulatorClearConsole
#define XEmulatorClearConsole(io) \
	AROS_LC1(BOOL  ,XEmulatorClearConsole, \
	AROS_LCA(  struct XEM_IO *,io,A0),\
	struct XEmulatorBase *, XEmulatorBase, 14, XEmulatorBase)
#endif

#ifndef XEmulatorResetConsole
#define XEmulatorResetConsole(io) \
	AROS_LC1(BOOL  ,XEmulatorResetConsole, \
	AROS_LCA(  struct XEM_IO *,io,A0),\
	struct XEmulatorBase *, XEmulatorBase, 15, XEmulatorBase)
#endif

#ifndef XEmulatorResetTextStyles
#define XEmulatorResetTextStyles(io) \
	AROS_LC1(BOOL  ,XEmulatorResetTextStyles, \
	AROS_LCA(  struct XEM_IO *,io,A0),\
	struct XEmulatorBase *, XEmulatorBase, 16, XEmulatorBase)
#endif

#ifndef XEmulatorResetCharset
#define XEmulatorResetCharset(io) \
	AROS_LC1(BOOL  ,XEmulatorResetCharset, \
	AROS_LCA(  struct XEM_IO *,io,A0),\
	struct XEmulatorBase *, XEmulatorBase, 17, XEmulatorBase)
#endif

#ifndef XEmulatorOptions
#define XEmulatorOptions(io) \
	AROS_LC1(BOOL  ,XEmulatorOptions, \
	AROS_LCA(  struct XEM_IO *,io,A0),\
	struct XEmulatorBase *, XEmulatorBase, 13, XEmulatorBase)
#endif

#ifndef XEmulatorGetFreeMacroKeys
#define XEmulatorGetFreeMacroKeys(io,qualifier) \
	AROS_LC2(ULONG  ,XEmulatorGetFreeMacroKeys, \
	AROS_LCA(  struct XEM_IO *,io,A0),\
	AROS_LCA(  ULONG,qualifier,D0),\
	struct XEmulatorBase *, XEmulatorBase, 18, XEmulatorBase)
#endif

#ifndef XEmulatorMacroKeyFilter
#define XEmulatorMacroKeyFilter(io,macrokeys) \
	AROS_LC2(BOOL  ,XEmulatorMacroKeyFilter, \
	AROS_LCA(  struct XEM_IO *,io,A0),\
	AROS_LCA(  struct XEmulatorMacroKey *,macrokeys,A1),\
	struct XEmulatorBase *, XEmulatorBase, 19, XEmulatorBase)
#endif

#ifndef XEmulatorInfo
#define XEmulatorInfo(io,type) \
	AROS_LC2(LONG  ,XEmulatorInfo, \
	AROS_LCA(  struct XEM_IO *,io,A0),\
	AROS_LCA(  ULONG,type,D0),\
	struct XEmulatorBase *, XEmulatorBase, 20, XEmulatorBase)
#endif

#ifndef XEmulatorPreferences
#define XEmulatorPreferences(io,filename,mode) \
	AROS_LC3(BOOL  ,XEmulatorPreferences, \
	AROS_LCA(  struct XEM_IO *,io,A0),\
	AROS_LCA(  STRPTR,filename,A1),\
	AROS_LCA(  ULONG,mode,D0),\
	struct XEmulatorBase *, XEmulatorBase, 21, XEmulatorBase)
#endif

#ifndef XEmulatorNewSize
#define XEmulatorNewSize(io,frame) \
	AROS_LC2NR(VOID  ,XEmulatorNewSize, \
	AROS_LCA(  struct XEM_IO *,io,A0),\
	AROS_LCA(  struct IBox *,frame,A1),\
	struct XEmulatorBase *, XEmulatorBase, 22, XEmulatorBase)
#endif

#endif
