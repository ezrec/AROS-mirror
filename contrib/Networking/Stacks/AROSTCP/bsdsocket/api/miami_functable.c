/*
 * Copyright (C) 2005 - 2007 The AROS Dev Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 */

#include <conf.h>

#include <aros/libcall.h>
#include <exec/types.h>
#include <sys/param.h>
#include <api/amiga_raf.h>

typedef VOID (* f_void)();

/*
 * Null used in both function tables
 */
extern VOID AROS_SLIB_ENTRY(Null, LIB)(VOID);

/*
 * "declarations" for ExecLibraryList_funcTable functions.
 */ 

extern void MiamiLIB_Open(void);
extern void MiamiLIB_Close(void);
extern VOID AROS_SLIB_ENTRY(Expunge, ELL)();

f_void Miami_InitFuncTable[]=
{
#ifdef __MORPHOS__
	FUNCARRAY_32BIT_NATIVE,
#endif
	MiamiLIB_Open,
    MiamiLIB_Close,
	AROS_SLIB_ENTRY(Expunge, ELL),
#warning "TODO: NicJA - LIB_Null??"
#if defined(__AROS__)
   NULL,
#else
  AROS_SLIB_ENTRY(Null, LIB),	     /* ELL_Reserved() is never called */
#endif
  (f_void)-1
};

/*
 * "declarations" for userLibrary_funcTable functions.
 */ 

extern void MiamiSysCtl(void);
extern void MiamiSetSysLogPort(void);
extern void MiamiDisallowDNS(void);
extern void MiamiGetPid(void);
extern void MiamiPFAddHook(void);
extern void MiamiPFRemoveHook(void);
extern void MiamiGetHardwareLen(void);
extern void MiamiOpenSSL(void);
extern void MiamiCloseSSL(void);
extern void MiamiSetSocksConn(void);
extern void MiamiIsOnline(void);
extern void MiamiOnOffline(void);
extern void inet_ntop(void);
extern void Miami_inet_aton(void);
extern void inet_pton(void);
extern void gethostbyname2(void);
extern void gai_strerror(void);
extern void freeaddrinfo(void);
extern void getaddrinfo(void);
extern void getnameinfo(void);
extern void if_nametoindex(void);
extern void if_indextoname(void);
extern void if_nameindex(void);
extern void if_freenameindex(void);
extern void MiamiSupportsIPV6(void);
extern void MiamiResGetOptions(void);
extern void MiamiResSetOptions(void);
extern void sockatmark(void);
extern void MiamiSupportedCPUs(void);
extern void MiamiGetFdCallback(void);
extern void MiamiSetFdCallback(void);
extern VOID AROS_SLIB_ENTRY(MiamiGetCredentials, Miami)();
extern void SetSysLogPort(void);
extern VOID AROS_SLIB_ENTRY(Miami_gethostent, Miami)();
extern VOID AROS_SLIB_ENTRY(Miami_endhostent, Miami)();
extern void Miami_getprotoent(void);
extern void Miami_endprotoent(void);
extern void ClearDynNameServ(void);
extern void ClearDynDomain(void);
extern void AddDynNameServ(void);
extern void AddDynDomain(void);
extern void EndDynNameServ(void);
extern void EndDynDomain(void);
extern void Miami_sethostname(void);
extern void Miami_QueryInterfaceTagList(void);

extern void *FindKernelVar(STRPTR name);

f_void	Miami_UserFuncTable[] =
{
#ifdef __MORPHOS__
	FUNCARRAY_BEGIN,
	FUNCARRAY_32BIT_NATIVE,
#endif
#warning "TODO: NicJA - LIB_Null??"
#if defined(__AROS__)
   NULL,
#else
  AROS_SLIB_ENTRY(Null, LIB),	     /* ELL_Open() is never called */
#endif
	MiamiLIB_Close,
#warning "TODO: NicJA - LIB_Null??"
#if defined(__AROS__)
   NULL,
#else
  AROS_SLIB_ENTRY(Null, LIB),	     /* ELL_Expunge() is never called */
#endif
#warning "TODO: NicJA - LIB_Null??"
#if defined(__AROS__)
   NULL,
#else
  AROS_SLIB_ENTRY(Null, LIB),	     /* ELL_Reserved() is never called */
#endif
	MiamiSysCtl,
	SetSysLogPort,
	Miami_QueryInterfaceTagList,
#warning "TODO: NicJA - LIB_Null??"
#if defined(__AROS__)
   NULL,
#else
  AROS_SLIB_ENTRY(Null, LIB),	     /* ELL_Reserved2() is never called */
#endif
	ClearDynNameServ,
	AROS_SLIB_ENTRY(Miami_gethostent, Miami),
	MiamiDisallowDNS,
	AROS_SLIB_ENTRY(Miami_endhostent, Miami),
	MiamiGetPid,
	Miami_getprotoent,
	Miami_endprotoent,
	MiamiPFAddHook,
	MiamiPFRemoveHook,
	MiamiGetHardwareLen,
	EndDynDomain,
	EndDynNameServ,
	AddDynNameServ,
	AddDynDomain,
	Miami_sethostname,
	ClearDynDomain,
	MiamiOpenSSL,
	MiamiCloseSSL,
#warning "TODO: NicJA - LIB_Null??"
#if defined(__AROS__)
   NULL,
#else
  AROS_SLIB_ENTRY(Null, LIB),	     /* ELL_Close() is never called */
#endif
#warning "TODO: NicJA - LIB_Null??"
#if defined(__AROS__)
   NULL,
#else
  AROS_SLIB_ENTRY(Null, LIB),	     /* ELL_Close() is never called */
#endif
#warning "TODO: NicJA - LIB_Null??"
#if defined(__AROS__)
   NULL,
#else
  AROS_SLIB_ENTRY(Null, LIB),	     /* ELL_Close() is never called */
#endif
#warning "TODO: NicJA - LIB_Null??"
#if defined(__AROS__)
   NULL,
#else
  AROS_SLIB_ENTRY(Null, LIB),	     /* ELL_Close() is never called */
#endif
#warning "TODO: NicJA - LIB_Null??"
#if defined(__AROS__)
   NULL,
#else
  AROS_SLIB_ENTRY(Null, LIB),	     /* ELL_Close() is never called */
#endif
#warning "TODO: NicJA - LIB_Null??"
#if defined(__AROS__)
   NULL,
#else
  AROS_SLIB_ENTRY(Null, LIB),	     /* ELL_Close() is never called */
#endif
	(ULONG)MiamiSetSocksConn,
#warning "TODO: NicJA - LIB_Null??"
#if defined(__AROS__)
   NULL,
#else
  AROS_SLIB_ENTRY(Null, LIB),	     /* ELL_Close() is never called */
#endif
	(ULONG)MiamiIsOnline,
	(ULONG)MiamiOnOffline,
#warning "TODO: NicJA - LIB_Null??"
#if defined(__AROS__)
   NULL,
#else
  AROS_SLIB_ENTRY(Null, LIB),	     /* ELL_Close() is never called */
#endif
	inet_ntop,
	Miami_inet_aton,
	inet_pton,
	gethostbyname2,
	gai_strerror,
	freeaddrinfo,
	getaddrinfo,
	getnameinfo,
	if_nametoindex,
	if_indextoname,
	if_nameindex,
	if_freenameindex,
	MiamiSupportsIPV6,
	MiamiResGetOptions,
	MiamiResSetOptions,
	sockatmark,
	MiamiSupportedCPUs,
	MiamiGetFdCallback,
	MiamiSetFdCallback,
#warning "TODO: NicJA - LIB_Null??"
#if defined(__AROS__)
   NULL,
#else
  AROS_SLIB_ENTRY(Null, LIB),	     /* ELL_Close() is never called */
#endif
	AROS_SLIB_ENTRY(MiamiGetCredentials, Miami),
#ifdef __MORPHOS__
	-1,
	FUNCARRAY_32BIT_SYSTEMV,
#endif
	FindKernelVar,
#ifdef __MORPHOS__
	(f_void)-1,
	FUNCARRAY_END
#else
    (f_void)-1
#endif
};

