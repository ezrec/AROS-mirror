#ifndef DEFINES_XPR_PROTOS_H
#define DEFINES_XPR_PROTOS_H

#include <aros/libcall.h>
#include <exec/types.h>

#ifndef XProtocolSend
#define XProtocolSend(xio) \
	AROS_LC1(long ,XProtocolSend, \
	AROS_LCA(  struct XPR_IO *,xio,A0),\
	struct XProtocolBase *, XProtocolBase, 7, XProtocolBase)
#endif

#ifndef XProtocolReceive
#define XProtocolReceive(xio) \
	AROS_LC1(long ,XProtocolReceive, \
	AROS_LCA(  struct XPR_IO *,xio,A0),\
	struct XProtocolBase *, XProtocolBase, 8, XProtocolBase)
#endif

#ifndef XProtocolSetup
#define XProtocolSetup(xio) \
	AROS_LC1(long ,XProtocolSetup, \
	AROS_LCA(  struct XPR_IO *,xio,A0),\
	struct XProtocolBase *, XProtocolBase, 6, XProtocolBase)
#endif

#ifndef XProtocolCleanup
#define XProtocolCleanup(xio) \
	AROS_LC1(long ,XProtocolCleanup, \
	AROS_LCA(  struct XPR_IO *,xio,A0),\
	struct XProtocolBase *, XProtocolBase, 5, XProtocolBase)
#endif

#ifndef XProtocolHostMon
#define XProtocolHostMon(xio,serbuff,actual,maxsize) \
	AROS_LC4(long ,XProtocolHostMon, \
	AROS_LCA(  struct XPR_IO *,xio,A0),\
	AROS_LCA(  char *,serbuff,A1),\
	AROS_LCA(  long,actual,D0),\
	AROS_LCA(  long,maxsize,D1),\
	struct XProtocolBase *, XProtocolBase, 9, XProtocolBase)
#endif

#ifndef XProtocolUserMon
#define XProtocolUserMon(xio,serbuff,actual,maxsize) \
	AROS_LC4(long ,XProtocolUserMon, \
	AROS_LCA(  struct XPR_IO *,xio,A0),\
	AROS_LCA(  char *,serbuff,A1),\
	AROS_LCA(  long,actual,D0),\
	AROS_LCA(  long,maxsize,D1),\
	struct XProtocolBase *, XProtocolBase, 10, XProtocolBase)
#endif

#endif
