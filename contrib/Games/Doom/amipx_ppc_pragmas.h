#ifndef _PPCPRAGMA_AMIPX_H
#define _PPCPRAGMA_AMIPX_H

#ifdef __GNUC__
#ifndef _PPCINLINE__AMIPX_H
#include <powerup/ppcinline/amipx.h>
#endif
#else

#ifndef POWERUP_PPCLIB_INTERFACE_H
#include <powerup/ppclib/interface.h>
#endif

#ifndef POWERUP_GCCLIB_PROTOS_H
#include <powerup/gcclib/powerup_protos.h>
#endif

#ifndef NO_PPCINLINE_STDARG
#define NO_PPCINLINE_STDARG
#endif/* SAS C PPC inlines */

#ifndef AMIPX_BASE_NAME
#define AMIPX_BASE_NAME AMIPX_Library
#endif /* !AMIPX_BASE_NAME */

#define	AMIPX_OpenSocket(socknum) _AMIPX_OpenSocket(AMIPX_BASE_NAME, socknum)

static __inline WORD
_AMIPX_OpenSocket(void *AMIPX_Library, UWORD socknum)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode	=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart	=	NULL;
//	MyCaos.M68kSize		=	0;
	MyCaos.PPCCacheMode	=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart		=	NULL;
//	MyCaos.PPCSize		=	0;
	MyCaos.d0		=(ULONG) socknum;
	MyCaos.caos_Un.Offset	=	(-0x1E);
	MyCaos.a6		=(ULONG) AMIPX_Library;	
	return((WORD)PPCCallOS(&MyCaos));
}

#define	AMIPX_CloseSocket(socknum) _AMIPX_CloseSocket(AMIPX_BASE_NAME, socknum)

static __inline void
_AMIPX_CloseSocket(void *AMIPX_Library, UWORD socknum)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode	=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart	=	NULL;
//	MyCaos.M68kSize		=	0;
	MyCaos.PPCCacheMode	=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart		=	NULL;
//	MyCaos.PPCSize		=	0;
	MyCaos.d0		=(ULONG) socknum;
	MyCaos.caos_Un.Offset	=	(-0x24);
	MyCaos.a6		=(ULONG) AMIPX_Library;	
	PPCCallOS(&MyCaos);
}

#define	AMIPX_ListenForPacket(ECB) _AMIPX_ListenForPacket(AMIPX_BASE_NAME, ECB)

static __inline WORD
_AMIPX_ListenForPacket(void *AMIPX_Library, struct AMIPX_ECB *ECB)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode	=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart	=	NULL;
//	MyCaos.M68kSize		=	0;
	MyCaos.PPCCacheMode	=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart		=	NULL;
//	MyCaos.PPCSize		=	0;
	MyCaos.a0		=(ULONG) ECB;
	MyCaos.caos_Un.Offset	=	(-0x2A);
	MyCaos.a6		=(ULONG) AMIPX_Library;	
	return((WORD)PPCCallOS(&MyCaos));
}

#define	AMIPX_SendPacket(ECB) _AMIPX_SendPacket(AMIPX_BASE_NAME, ECB)

static __inline WORD
_AMIPX_SendPacket(void *AMIPX_Library, struct AMIPX_ECB *ECB)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode	=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart	=	NULL;
//	MyCaos.M68kSize		=	0;
	MyCaos.PPCCacheMode	=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart		=	NULL;
//	MyCaos.PPCSize		=	0;
	MyCaos.a0		=(ULONG) ECB;
	MyCaos.caos_Un.Offset	=	(-0x30);
	MyCaos.a6		=(ULONG) AMIPX_Library;	
	return((WORD)PPCCallOS(&MyCaos));
}

#define	AMIPX_GetLocalAddr(addrarray) _AMIPX_GetLocalAddr(AMIPX_BASE_NAME, addrarray)

static __inline void
_AMIPX_GetLocalAddr(void *AMIPX_Library, UBYTE addrarray[10])
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode	=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart	=	NULL;
//	MyCaos.M68kSize		=	0;
	MyCaos.PPCCacheMode	=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart		=	NULL;
//	MyCaos.PPCSize		=	0;
	MyCaos.a0		=(ULONG) addrarray;
	MyCaos.caos_Un.Offset	=	(-0x36);
	MyCaos.a6		=(ULONG) AMIPX_Library;	
	PPCCallOS(&MyCaos);
}

#define	AMIPX_RelinquishControl() _AMIPX_RelinquishControl(AMIPX_BASE_NAME)

static __inline void
_AMIPX_RelinquishControl(void *AMIPX_Library)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode	=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart	=	NULL;
//	MyCaos.M68kSize		=	0;
	MyCaos.PPCCacheMode	=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart		=	NULL;
//	MyCaos.PPCSize		=	0;
	MyCaos.caos_Un.Offset	=	(-0x3C);
	MyCaos.a6		=(ULONG) AMIPX_Library;	
	PPCCallOS(&MyCaos);
}

#define	AMIPX_GetLocalTarget(addrarray, localtarget) _AMIPX_GetLocalTarget(AMIPX_BASE_NAME, addrarray, localtarget)

static __inline WORD
_AMIPX_GetLocalTarget(void *AMIPX_Library, UBYTE addrarray[12], UBYTE localtarget[6])
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode	=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart	=	NULL;
//	MyCaos.M68kSize		=	0;
	MyCaos.PPCCacheMode	=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart		=	NULL;
//	MyCaos.PPCSize		=	0;
	MyCaos.a0		=(ULONG) addrarray;
	MyCaos.a1		=(ULONG) localtarget;
	MyCaos.caos_Un.Offset	=	(-0x42);
	MyCaos.a6		=(ULONG) AMIPX_Library;	
	return((WORD)PPCCallOS(&MyCaos));
}

#endif /* SASC Pragmas */
#endif /* !_PPCPRAGMA_AMIGAGUIDE_H */
