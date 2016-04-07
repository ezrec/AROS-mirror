#ifndef XADMASTER_FUNCTIONS_H
#define XADMASTER_FUNCTIONS_H

/*  $Id$
    declarations and prototypes

    XAD library system for archive handling
    Copyright (C) 1998 and later by Dirk Stöcker <soft@dstoecker.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* xadmaster.h is included without its public library prototypes, so we
 * can define them ourselves using xadMasterBaseP instead of xadMasterBase */
#define XAD_NO_PROTOTYPES 1
#include <xadmaster.h>

/* include all library-internal structures, including xadMasterBaseP */
#include <privdefs.h>

/************************************************************************
*                                                                       *
*    library accessable function                                        *
*                                                                       *
************************************************************************/

/*** BEGIN auto-generated section (INTERNAL AMIGA) */
#include <proto/utility.h>
#define SDI_TO_ANSI
#include "SDI_ASM_STD_protos.h"
#include "SDI_compiler.h"
#if !defined(__AROS__)
#ifdef NO_INLINE_STDARG
#include "stubs.h"
#endif
#endif

#if !defined(__AROS__)
#   define PROTOHOOK(name) \
  ASM(xadINT32) name( \
  REG(a0, struct Hook * hook), \
  REG(a2, struct xadArchiveInfoP *ai), \
  REG(a1, struct xadHookParam * param))
#   define FUNCHOOK(name) PROTOHOOK(name) {
#   define ENDFUNC }
#   define ENDHOOK ENDFUNC
#else
#   include <aros/asmcall.h>
#   define PROTOHOOK(name) \
  AROS_UFP3(xadINT32, name, \
  AROS_UFPA(struct Hook *, hook, A0), \
  AROS_UFPA(struct xadArchiveInfoP *, ai, A2), \
  AROS_UFPA(struct xadHookParam *, param,  A1))
#   define FUNCHOOK(name) \
  AROS_UFH3(xadINT32, name, \
  AROS_UFHA(struct Hook *, hook, A0), \
  AROS_UFHA(struct xadArchiveInfoP *, ai, A2), \
  AROS_UFHA(struct xadHookParam *, param,  A1)) \
  { \
      AROS_USERFUNC_INIT
#   define ENDFUNC AROS_LIBFUNC_EXIT \
}
#   define ENDHOOK AROS_USERFUNC_EXIT \
}
#endif

#if !defined(__AROS__)
/* xadAddDiskEntry - clientfunc.c */
ASM(xadERROR) LIBxadAddDiskEntryA(REG(a0, struct xadDiskInfo *di), REG(a1, struct xadArchiveInfoP *ai), REG(a2, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadAddDiskEntry ASM(xadERROR) LIBxadAddDiskEntryA(REG(a0, struct xadDiskInfo *di), REG(a1, struct xadArchiveInfoP *ai), REG(a2, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase)) { \
  struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadAddFileEntry - clientfunc.c */
ASM(xadERROR) LIBxadAddFileEntryA(REG(a0, struct xadFileInfo *fi), REG(a1, struct xadArchiveInfoP *ai), REG(a2, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadAddFileEntry ASM(xadERROR) LIBxadAddFileEntryA(REG(a0, struct xadFileInfo *fi), REG(a1, struct xadArchiveInfoP *ai), REG(a2, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase)) { \
  struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadAllocObject - objects.c */
ASM(xadPTR) LIBxadAllocObjectA(REG(d0, xadUINT32 type), REG(a0, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadAllocObject ASM(xadPTR) LIBxadAllocObjectA(REG(d0, xadUINT32 type), REG(a0, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase)) { \
  struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadAllocVec - objects.c */
ASM(xadPTR) LIBxadAllocVec(REG(d0, xadSize size), REG(d1, xadUINT32 flags), REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadAllocVec ASM(xadPTR) LIBxadAllocVec(REG(d0, xadSize size), REG(d1, xadUINT32 flags), REG(a6, struct xadMasterBaseP *xadMasterBase)) {

/* xadCalcCRC16 - crc.c */
ASM(xadUINT16) LIBxadCalcCRC16(REG(d0, xadUINT16 id), REG(d1, xadUINT16 init), REG(d2, xadSize size), REG(a0, const xadUINT8 *buffer), REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadCalcCRC16 ASM(xadUINT16) LIBxadCalcCRC16(REG(d0, xadUINT16 id), REG(d1, xadUINT16 init), REG(d2, xadSize size), REG(a0, const xadUINT8 *buffer), REG(a6, struct xadMasterBaseP *xadMasterBase)) {

/* xadCalcCRC32 - crc.c */
ASM(xadUINT32) LIBxadCalcCRC32(REG(d0, xadUINT32 id), REG(d1, xadUINT32 init), REG(d2, xadSize size), REG(a0, const xadUINT8 *buffer), REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadCalcCRC32 ASM(xadUINT32) LIBxadCalcCRC32(REG(d0, xadUINT32 id), REG(d1, xadUINT32 init), REG(d2, xadSize size), REG(a0, const xadUINT8 *buffer), REG(a6, struct xadMasterBaseP *xadMasterBase)) {

/* xadConvertDates - dates.c */
ASM(xadERROR) LIBxadConvertDatesA(REG(a0, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadConvertDates ASM(xadERROR) LIBxadConvertDatesA(REG(a0, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase)) { \
  struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadConvertName - filename.c */
ASM(xadSTRPTR) LIBxadConvertNameA(REG(d0, xadUINT32 charset), REG(a0, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadConvertName ASM(xadSTRPTR) LIBxadConvertNameA(REG(d0, xadUINT32 charset), REG(a0, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase)) { \
  struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadConvertProtection - protection.c */
ASM(xadERROR) LIBxadConvertProtectionA(REG(a0, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadConvertProtection ASM(xadERROR) LIBxadConvertProtectionA(REG(a0, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase)) { \
  struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadCopyMem - copymem.c */
ASM(void) LIBxadCopyMem(REG(a0, const void *s), REG(a1, xadPTR d), REG(d0, xadSize size));
#define FUNCxadCopyMem ASM(void) LIBxadCopyMem(REG(a0, const void *s), REG(a1, xadPTR d), REG(d0, xadSize size)) {

/* xadDiskUnArc - diskunarc.c */
ASM(xadERROR) LIBxadDiskUnArcA(REG(a0, struct xadArchiveInfoP *ai), REG(a1, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadDiskUnArc ASM(xadERROR) LIBxadDiskUnArcA(REG(a0, struct xadArchiveInfoP *ai), REG(a1, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase)) { \
  struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadFileUnArc - fileunarc.c */
ASM(xadERROR) LIBxadFileUnArcA(REG(a0, struct xadArchiveInfoP *ai), REG(a1, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadFileUnArc ASM(xadERROR) LIBxadFileUnArcA(REG(a0, struct xadArchiveInfoP *ai), REG(a1, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase)) { \
  struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadFreeHookAccess - hook.c */
ASM(void) LIBxadFreeHookAccessA(REG(a0, struct xadArchiveInfoP *ai), REG(a1, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadFreeHookAccess ASM(void) LIBxadFreeHookAccessA(REG(a0, struct xadArchiveInfoP *ai), REG(a1, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase)) { \
  struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadFreeInfo - info.c */
ASM(void) LIBxadFreeInfo(REG(a0, struct xadArchiveInfoP *ai), REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadFreeInfo ASM(void) LIBxadFreeInfo(REG(a0, struct xadArchiveInfoP *ai), REG(a6, struct xadMasterBaseP *xadMasterBase)) {

/* xadFreeObject - objects.c */
ASM(void) LIBxadFreeObjectA(REG(a0, xadPTR object), REG(a1, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadFreeObject ASM(void) LIBxadFreeObjectA(REG(a0, xadPTR object), REG(a1, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase)) {

/* xadGetClientInfo - clientfunc.c */
ASM(struct xadClient *) LIBxadGetClientInfo(REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadGetClientInfo ASM(struct xadClient *) LIBxadGetClientInfo(REG(a6, struct xadMasterBaseP *xadMasterBase)) {

/* xadGetDefaultName - filename.c */
ASM(xadSTRPTR) LIBxadGetDefaultNameA(REG(a0, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadGetDefaultName ASM(xadSTRPTR) LIBxadGetDefaultNameA(REG(a0, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase)) { \
  struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadGetDiskInfo - diskfile.c */
ASM(xadERROR) LIBxadGetDiskInfoA(REG(a0, struct xadArchiveInfoP *ai), REG(a1, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadGetDiskInfo ASM(xadERROR) LIBxadGetDiskInfoA(REG(a0, struct xadArchiveInfoP *ai), REG(a1, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase)) { \
  struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadGetErrorText - error.c */
ASM(xadSTRPTR) LIBxadGetErrorText(REG(d0, xadERROR errnum));
#define FUNCxadGetErrorText ASM(xadSTRPTR) LIBxadGetErrorText(REG(d0, xadERROR errnum)) {

/* xadGetFilename - filename.c */
ASM(xadERROR) LIBxadGetFilenameA(REG(d0, xadUINT32 buffersize), REG(a0, xadSTRPTR buffer), REG(a1, const xadSTRING *path), REG(a2, const xadSTRING *name), REG(a3, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadGetFilename ASM(xadERROR) LIBxadGetFilenameA(REG(d0, xadUINT32 buffersize), REG(a0, xadSTRPTR buffer), REG(a1, const xadSTRING *path), REG(a2, const xadSTRING *name), REG(a3, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase)) { \
  struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadGetInfo - info.c */
ASM(xadERROR) LIBxadGetInfoA(REG(a0, struct xadArchiveInfoP *ai), REG(a1, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadGetInfo ASM(xadERROR) LIBxadGetInfoA(REG(a0, struct xadArchiveInfoP *ai), REG(a1, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase)) {

/* xadGetHookAccess - hook.c */
ASM(xadERROR) LIBxadGetHookAccessA(REG(a0, struct xadArchiveInfoP *ai), REG(a1, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadGetHookAccess ASM(xadERROR) LIBxadGetHookAccessA(REG(a0, struct xadArchiveInfoP *ai), REG(a1, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase)) { \
  struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadGetSystemInfo - info.c */
ASM(struct xadSystemInfo *) LIBxadGetSystemInfo(REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadGetSystemInfo ASM(struct xadSystemInfo *) LIBxadGetSystemInfo(REG(a6, struct xadMasterBaseP *xadMasterBase)) {

/* xadHookAccess - clientfunc.c */
ASM(xadERROR) LIBxadHookAccess(REG(d0, xadUINT32 command), REG(d1, xadSignSize data), REG(a0, xadPTR buffer), REG(a1, struct xadArchiveInfoP *ai), REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadHookAccess ASM(xadERROR) LIBxadHookAccess(REG(d0, xadUINT32 command), REG(d1, xadSignSize data), REG(a0, xadPTR buffer), REG(a1, struct xadArchiveInfoP *ai), REG(a6, struct xadMasterBaseP *xadMasterBase)) {

/* xadHookTagAccess - clientfunc.c */
ASM(xadERROR) LIBxadHookTagAccessA(REG(d0, xadUINT32 command), REG(d1, xadSignSize data), REG(a0, xadPTR buffer), REG(a1, struct xadArchiveInfoP *ai), REG(a2, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadHookTagAccess ASM(xadERROR) LIBxadHookTagAccessA(REG(d0, xadUINT32 command), REG(d1, xadSignSize data), REG(a0, xadPTR buffer), REG(a1, struct xadArchiveInfoP *ai), REG(a2, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase)) { \
  struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadRecogFile - info.c */
ASM(struct xadClient *) LIBxadRecogFileA(REG(d0, xadSize size), REG(a0, const void *mem), REG(a1, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase));
#define FUNCxadRecogFile ASM(struct xadClient *) LIBxadRecogFileA(REG(d0, xadSize size), REG(a0, const void *mem), REG(a1, xadTAGPTR tags), REG(a6, struct xadMasterBaseP *xadMasterBase)) { \
  struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;
  
#else

/********************** AROS **************************/

/* xadAddDiskEntry - clientfunc.c */
//AROS_LP3(xadERROR, xadAddDiskEntryA, AROS_LPA(struct xadDiskInfo *, di, A0), AROS_LPA(struct xadArchiveInfoP *, ai, A1), AROS_LPA(xadTAGPTR, tags, A2), struct xadMasterBaseP *, xadMasterBase, 26, xadmaster);
#define FUNCxadAddDiskEntry \
AROS_LH3(xadERROR, xadAddDiskEntryA, AROS_LHA(struct xadDiskInfo *, di, A0), AROS_LHA(struct xadArchiveInfoP *, ai, A1), AROS_LHA(xadTAGPTR, tags, A2), struct xadMasterBaseP *, xadMasterBase, 26, xadmaster) { \
    AROS_LIBFUNC_INIT \
    struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadAddFileEntry - clientfunc.c */
//AROS_LP3(xadERROR, xadAddFileEntryA, AROS_LPA(struct xadFileInfo *, fi, A0), AROS_LPA(struct xadArchiveInfoP *, ai, A1), AROS_LPA(xadTAGPTR, tags, A2), struct xadMasterBaseP *, xadMasterBase, 25, xadmaster);
#define FUNCxadAddFileEntry \
AROS_LH3(xadERROR, xadAddFileEntryA, AROS_LHA(struct xadFileInfo *, fi, A0), AROS_LHA(struct xadArchiveInfoP *, ai, A1), AROS_LHA(xadTAGPTR, tags, A2), struct xadMasterBaseP *, xadMasterBase, 25, xadmaster) { \
    AROS_LIBFUNC_INIT \
    struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadAllocObject - objects.c */
//AROS_LP2(xadPTR, xadAllocObjectA, AROS_LPA(xadUINT32, type, D0), AROS_LPA(xadTAGPTR, tags, A0), struct xadMasterBaseP *, xadMasterBase, 5, xadmaster);
#define FUNCxadAllocObject \
AROS_LH2(xadPTR, xadAllocObjectA, AROS_LHA(xadUINT32, type, D0), AROS_LHA(xadTAGPTR, tags, A0), struct xadMasterBaseP *, xadMasterBase, 5, xadmaster) { \
    AROS_LIBFUNC_INIT \
    struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadAllocVec - objects.c */
//AROS_LP2(xadPTR, xadAllocVec, AROS_LPA(xadSize, size, D0), AROS_LPA(xadUINT32, flags, D1), struct xadMasterBaseP *, xadMasterBase, 18, xadmaster);
#define FUNCxadAllocVec \
AROS_LH2(xadPTR, xadAllocVec, AROS_LHA(xadSize, size, D0), AROS_LHA(xadUINT32, flags, D1), struct xadMasterBaseP *, xadMasterBase, 18, xadmaster) { \
    AROS_LIBFUNC_INIT \

/* xadCalcCRC16 - crc.c */
//AROS_LP4(xadUINT16, xadCalcCRC16, AROS_LPA(xadUINT16, id, D0), AROS_LPA(xadUINT16, init, D1), AROS_LPA(xadSize, size, D2), AROS_LPA(const xadUINT8 *, buffer, A0), struct xadMasterBaseP *, xadMasterBase, 16, xadmaster);
#define FUNCxadCalcCRC16 \
AROS_LH4(xadUINT16, xadCalcCRC16, AROS_LHA(xadUINT16, id, D0), AROS_LHA(xadUINT16, init, D1), AROS_LHA(xadSize, size, D2), AROS_LHA(const xadUINT8 *, buffer, A0), struct xadMasterBaseP *, xadMasterBase, 16, xadmaster) { \
    AROS_LIBFUNC_INIT \

/* xadCalcCRC32 - crc.c */
//AROS_LP4(xadUINT32, xadCalcCRC32, AROS_LPA(xadUINT32, id, D0), AROS_LPA(xadUINT32, init, D1), AROS_LPA(xadSize, size, D2), AROS_LPA(const xadUINT8 *, buffer, A0), struct xadMasterBaseP *, xadMasterBase, 17, xadmaster);
#define FUNCxadCalcCRC32 \
AROS_LH4(xadUINT32, xadCalcCRC32, AROS_LHA(xadUINT32, id, D0), AROS_LHA(xadUINT32, init, D1), AROS_LHA(xadSize, size, D2), AROS_LHA(const xadUINT8 *, buffer, A0), struct xadMasterBaseP *, xadMasterBase, 17, xadmaster) { \
    AROS_LIBFUNC_INIT \

/* xadConvertDates - dates.c */
//AROS_LP1(xadERROR, xadConvertDatesA, AROS_LPA(xadTAGPTR, tags, A0), struct xadMasterBaseP *, xadMasterBase, 15, xadmaster);
#define FUNCxadConvertDates \
AROS_LH1(xadERROR, xadConvertDatesA, AROS_LHA(xadTAGPTR, tags, A0), struct xadMasterBaseP *, xadMasterBase, 15, xadmaster) { \
    AROS_LIBFUNC_INIT \
    struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadConvertName - filename.c */
//AROS_LP2(xadSTRPTR, xadConvertNameA, AROS_LPA(xadUINT32, charset, D0), AROS_LPA(xadTAGPTR, tags, A0), struct xadMasterBaseP *, xadMasterBase, 28, xadmaster);
#define FUNCxadConvertName \
AROS_LH2(xadSTRPTR, xadConvertNameA, AROS_LHA(xadUINT32, charset, D0), AROS_LHA(xadTAGPTR, tags, A0), struct xadMasterBaseP *, xadMasterBase, 28, xadmaster) { \
    AROS_LIBFUNC_INIT \
    struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadConvertProtection - protection.c */
//AROS_LP1(xadERROR, xadConvertProtectionA, AROS_LPA(xadTAGPTR, tags, A0), struct xadMasterBaseP *, xadMasterBase, 21, xadmaster);
#define FUNCxadConvertProtection \
AROS_LH1(xadERROR, xadConvertProtectionA, AROS_LHA(xadTAGPTR, tags, A0), struct xadMasterBaseP *, xadMasterBase, 21, xadmaster) { \
    AROS_LIBFUNC_INIT \
    struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadCopyMem - copymem.c */
//AROS_LP3(void, xadCopyMem, AROS_LPA(const void *, s, A0), AROS_LPA(xadPTR, d, A1), AROS_LPA(xadSize, size, D0), struct xadMasterBaseP *, xadMasterBase, 19, xadmaster);
#define FUNCxadCopyMem \
AROS_LH3(void, xadCopyMem, AROS_LHA(const void *, s, A0), AROS_LHA(xadPTR, d, A1), AROS_LHA(xadSize, size, D0), struct xadMasterBaseP *, xadMasterBase, 19, xadmaster) { \
    AROS_LIBFUNC_INIT \

/* xadDiskUnArc - diskunarc.c */
//AROS_LP2(xadERROR, xadDiskUnArcA, AROS_LPA(struct xadArchiveInfoP *, ai, A0), AROS_LPA(xadTAGPTR, tags, A1), struct xadMasterBaseP *, xadMasterBase, 11, xadmaster);
#define FUNCxadDiskUnArc \
AROS_LH2(xadERROR, xadDiskUnArcA, AROS_LHA(struct xadArchiveInfoP *, ai, A0), AROS_LHA(xadTAGPTR, tags, A1), struct xadMasterBaseP *, xadMasterBase, 11, xadmaster) { \
    AROS_LIBFUNC_INIT \
    struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadFileUnArc - fileunarc.c */
//AROS_LP2(xadERROR, xadFileUnArcA, AROS_LPA(struct xadArchiveInfoP *, ai, A0), AROS_LPA(xadTAGPTR, tags, A1), struct xadMasterBaseP *, xadMasterBase, 10, xadmaster);
#define FUNCxadFileUnArc \
AROS_LH2(xadERROR, xadFileUnArcA, AROS_LHA(struct xadArchiveInfoP *, ai, A0), AROS_LHA(xadTAGPTR, tags, A1), struct xadMasterBaseP *, xadMasterBase, 10, xadmaster) { \
    AROS_LIBFUNC_INIT \
    struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadFreeHookAccess - hook.c */
//AROS_LP2(void, xadFreeHookAccessA, AROS_LPA(struct xadArchiveInfoP *, ai, A0), AROS_LPA(xadTAGPTR, tags, A1), struct xadMasterBaseP *, xadMasterBase, 24, xadmaster);
#define FUNCxadFreeHookAccess \
AROS_LH2(void, xadFreeHookAccessA, AROS_LHA(struct xadArchiveInfoP *, ai, A0), AROS_LHA(xadTAGPTR, tags, A1), struct xadMasterBaseP *, xadMasterBase, 24, xadmaster) { \
    AROS_LIBFUNC_INIT \
    struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadFreeInfo - info.c */
//AROS_LP1(void, xadFreeInfo, AROS_LPA(struct xadArchiveInfoP *, ai, A0), struct xadMasterBaseP *, xadMasterBase, 9, xadmaster);
#define FUNCxadFreeInfo \
AROS_LH1(void, xadFreeInfo, AROS_LHA(struct xadArchiveInfoP *, ai, A0), struct xadMasterBaseP *, xadMasterBase, 9, xadmaster) { \
    AROS_LIBFUNC_INIT \

/* xadFreeObject - objects.c */
//AROS_LP2(void, xadFreeObjectA, AROS_LPA(xadPTR, object, A0), AROS_LPA(xadTAGPTR, tags, A1), struct xadMasterBaseP *, xadMasterBase, 6, xadmaster);
#define FUNCxadFreeObject \
AROS_LH2(void, xadFreeObjectA, AROS_LHA(xadPTR, object, A0), AROS_LHA(xadTAGPTR, tags, A1), struct xadMasterBaseP *, xadMasterBase, 6, xadmaster) { \
    AROS_LIBFUNC_INIT \

/* xadGetClientInfo - clientfunc.c */
//AROS_LP0(struct xadClient *, xadGetClientInfo, struct xadMasterBaseP *, xadMasterBase, 13, xadmaster);
#define FUNCxadGetClientInfo \
AROS_LH0(struct xadClient *, xadGetClientInfo, struct xadMasterBaseP *, xadMasterBase, 13, xadmaster) { \
    AROS_LIBFUNC_INIT \

/* xadGetDefaultName - filename.c */
//AROS_LP1(xadSTRPTR, xadGetDefaultNameA, AROS_LPA(xadTAGPTR, tags, A0), struct xadMasterBaseP *, xadMasterBase, 29, xadmaster);
#define FUNCxadGetDefaultName \
AROS_LH1(xadSTRPTR, xadGetDefaultNameA, AROS_LHA(xadTAGPTR, tags, A0), struct xadMasterBaseP *, xadMasterBase, 29, xadmaster) { \
    AROS_LIBFUNC_INIT \
    struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadGetDiskInfo - diskfile.c */
//AROS_LP2(xadERROR, xadGetDiskInfoA, AROS_LPA(struct xadArchiveInfoP *, ai, A0), AROS_LPA(xadTAGPTR, tags, A1), struct xadMasterBaseP *, xadMasterBase, 22, xadmaster);
#define FUNCxadGetDiskInfo \
AROS_LH2(xadERROR, xadGetDiskInfoA, AROS_LHA(struct xadArchiveInfoP *, ai, A0), AROS_LHA(xadTAGPTR, tags, A1), struct xadMasterBaseP *, xadMasterBase, 22, xadmaster) { \
    AROS_LIBFUNC_INIT \
    struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadGetErrorText - error.c */
//AROS_LP1(xadSTRPTR, xadGetErrorText, AROS_LPA(xadERROR, errnum, D0), struct xadMasterBaseP *, xadMasterBase, 12, xadmaster);
#define FUNCxadGetErrorText \
AROS_LH1(xadSTRPTR, xadGetErrorText, AROS_LHA(xadERROR, errnum, D0), struct xadMasterBaseP *, xadMasterBase, 12, xadmaster) { \
    AROS_LIBFUNC_INIT \

/* xadGetFilename - filename.c */
//AROS_LP5(xadERROR, xadGetFilenameA, AROS_LPA(xadUINT32, buffersize, D0), AROS_LPA(xadSTRPTR, buffer, A0), AROS_LPA(const xadSTRING *, path, A1), AROS_LPA(const xadSTRING *, name, A2), AROS_LPA(xadTAGPTR, tags, A3), struct xadMasterBaseP *, xadMasterBase, 27, xadmaster);
#define FUNCxadGetFilename \
AROS_LH5(xadERROR, xadGetFilenameA, AROS_LHA(xadUINT32, buffersize, D0), AROS_LHA(xadSTRPTR, buffer, A0), AROS_LHA(const xadSTRING *, path, A1), AROS_LHA(const xadSTRING *, name, A2), AROS_LHA(xadTAGPTR, tags, A3), struct xadMasterBaseP *, xadMasterBase, 27, xadmaster) { \
    AROS_LIBFUNC_INIT \
    struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadGetInfo - info.c */
//AROS_LP2(xadERROR, xadGetInfoA, AROS_LPA(struct xadArchiveInfoP *, ai, A0), AROS_LPA(xadTAGPTR, tags, A1), struct xadMasterBaseP *, xadMasterBase, 8, xadmaster);
#define FUNCxadGetInfo \
AROS_LH2(xadERROR, xadGetInfoA, AROS_LHA(struct xadArchiveInfoP *, ai, A0), AROS_LHA(xadTAGPTR, tags, A1), struct xadMasterBaseP *, xadMasterBase, 8, xadmaster) { \
    AROS_LIBFUNC_INIT \

/* xadGetHookAccess - hook.c */
//AROS_LP2(xadERROR, xadGetHookAccessA, AROS_LPA(struct xadArchiveInfoP *, ai, A0), AROS_LPA(xadTAGPTR, tags, A1), struct xadMasterBaseP *, xadMasterBase, 23, xadmaster);
#define FUNCxadGetHookAccess \
AROS_LH2(xadERROR, xadGetHookAccessA, AROS_LHA(struct xadArchiveInfoP *, ai, A0), AROS_LHA(xadTAGPTR, tags, A1), struct xadMasterBaseP *, xadMasterBase, 23, xadmaster) { \
    AROS_LIBFUNC_INIT \
    struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadGetSystemInfo - info.c */
//AROS_LP0(struct xadSystemInfo *, xadGetSystemInfo, struct xadMasterBaseP *, xadMasterBase, 30, xadmaster);
#define FUNCxadGetSystemInfo \
AROS_LH0(struct xadSystemInfo *, xadGetSystemInfo, struct xadMasterBaseP *, xadMasterBase, 30, xadmaster) { \
    AROS_LIBFUNC_INIT \

/* xadHookAccess - clientfunc.c */
//AROS_LP4(xadERROR, xadHookAccess, AROS_LPA(xadUINT32, command, D0), AROS_LPA(xadSignSize, data, D1),  AROS_LPA(xadPTR, buffer, A0), AROS_LPA(struct xadArchiveInfoP *, ai, A1), struct xadMasterBaseP *, xadMasterBase, 14, xadmaster);
#define FUNCxadHookAccess \
AROS_LH4(xadERROR, xadHookAccess, AROS_LHA(xadUINT32, command, D0), AROS_LHA(xadSignSize, data, D1), AROS_LHA(xadPTR, buffer, A0), AROS_LHA(struct xadArchiveInfoP *, ai, A1), struct xadMasterBaseP *, xadMasterBase, 14, xadmaster) { \
    AROS_LIBFUNC_INIT \

/* xadHookTagAccess - clientfunc.c */
//AROS_LP5(xadERROR, xadHookTagAccessA, AROS_LPA(xadUINT32, command, D0), AROS_LPA(xadSignSize, data, D1),  AROS_LPA(xadPTR, buffer, A0), AROS_LPA(struct xadArchiveInfoP *, ai, A1), AROS_LPA(xadTAGPTR, tags, A2), struct xadMasterBaseP *, xadMasterBase, 20, xadmaster);
#define FUNCxadHookTagAccess \
AROS_LH5(xadERROR, xadHookTagAccessA, AROS_LHA(xadUINT32, command, D0), AROS_LHA(xadSignSize, data, D1), AROS_LHA(xadPTR, buffer, A0), AROS_LHA(struct xadArchiveInfoP *, ai, A1), AROS_LHA(xadTAGPTR, tags, A2), struct xadMasterBaseP *, xadMasterBase, 20, xadmaster) { \
    AROS_LIBFUNC_INIT \
    struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

/* xadRecogFile - info.c */
//AROS_LP3(struct xadClient *, xadRecogFileA, AROS_LPA(xadSize, size, D0), AROS_LPA(const void *, mem, A0), AROS_LPA(xadTAGPTR, tags, A1), struct xadMasterBaseP *, xadMasterBase, 7, xadmaster);
#define FUNCxadRecogFile \
AROS_LH3(struct xadClient *, xadRecogFileA, AROS_LHA(xadSize, size, D0), AROS_LHA(const void *, mem, A0), AROS_LHA(xadTAGPTR, tags, A1), struct xadMasterBaseP *, xadMasterBase, 7, xadmaster) { \
    AROS_LIBFUNC_INIT \
    struct UtilityBase *UtilityBase = xadMasterBase->xmb_UtilityBase;

#endif

/*** END auto-generated section */

PROTOHOOK(InHookFH);       /* hook_fh.c */
PROTOHOOK(OutHookFH);      /* hook_fh.c */
PROTOHOOK(InHookMem);      /* hook_mem.c */
PROTOHOOK(OutHookMem);     /* hook_mem.c */
PROTOHOOK(InHookStream);   /* hook_stream.c */
PROTOHOOK(OutHookStream);  /* hook_stream.c */
PROTOHOOK(InHookDisk);     /* hook_disk.c */
PROTOHOOK(OutHookDisk);    /* hook_disk.c */
PROTOHOOK(InHookSplitted); /* hook_splitted.c */
PROTOHOOK(InHookDiskArc);  /* hook_diskarc.c */

/* clientfunc.c */
xadUINT32 callprogress(
                const struct xadArchiveInfoP *ai,
                xadUINT32 stat, xadUINT32 mode,
                struct xadMasterBaseP *xadMasterBase);

xadUINT32 callprogressFN(
                const struct xadArchiveInfoP *ai,
                xadUINT32 stat, xadUINT32 mode, xadSTRPTR *filename,
                struct xadMasterBaseP *xadMasterBase);

xadSignSize getskipsize(
                xadSignSize data,
                const struct xadArchiveInfoP *ai);

xadBOOL xadAddClients(struct xadMasterBaseP *xadMasterBase,
                      const struct xadClient *clients,
                      xadUINT32 add_flags);

void xadFreeClients(struct xadMasterBaseP *xadMasterBase);

void MakeCRC16(xadUINT16 *buf, xadUINT16 ID);
void MakeCRC32(xadUINT32 *buf, xadUINT32 ID);

extern const struct xadClient * const RealFirstClient;

#ifdef DEBUG
void DebugFlagged(xadUINT32 flags, const xadSTRING *fmt, ...);

void DebugTagList(const xadSTRING *, xadTAGPTR, ...);           /* print with 'R' and tags with 'T' */
void DebugTagListOther(const xadSTRING *, xadTAGPTR, ...);      /* print with 'O' and tags with 'T' */
void DebugTagListMem(const xadSTRING *, xadTAGPTR, ...);        /* print with 'R' or 'M' and tags with 'T' */
void DebugError(const xadSTRING *, ...);                        /* print with 'E' */
void DebugHook(const xadSTRING *, ...);                         /* print with 'H' */
void DebugHookTagList(const xadSTRING *, xadTAGPTR, ...);       /* print with 'H' and tags with 'T' */
void DebugRunTime(const xadSTRING *, ...);                      /* print with 'R' */
void DebugOther(const xadSTRING *, ...);                        /* print with 'O' */
void DebugMem(const xadSTRING *, ...);                          /* print with 'M' */
void DebugMemError(const xadSTRING *, ...);                     /* print with 'M' or 'E' */
void DebugFileSearched(const struct xadArchiveInfo *ai, const xadSTRING *, ...); /* print with 'D' */
void DebugClient(const struct xadArchiveInfo *ai, const xadSTRING *, ...);       /* print with 'D' */
#endif
#ifdef DEBUGRESOURCE
#define XADOBJCOOKIE 0x58414494
void DebugResource(struct xadMasterBaseP *, const xadSTRING *, ...);       /* print with 'C' */
/* called with 0 does end result check for this task */
/* called with 1 does end result check for all tasks */
#endif
#if defined(DEBUG) || defined(DEBUGRESOURCE)
#define DEBUGFLAG_ERROR         (1<<0)
#define DEBUGFLAG_RUNTIME       (1<<1)
#define DEBUGFLAG_TAGLIST       (1<<2)
#define DEBUGFLAG_HOOK          (1<<3)
#define DEBUGFLAG_OTHER         (1<<4)
#define DEBUGFLAG_MEM           (1<<5)
#define DEBUGFLAG_FLAGS         (1<<6)
#define DEBUGFLAG_RESOURCE      (1<<7)
#define DEBUGFLAG_CLIENT        (1<<8)
#define DEBUGFLAG_SEARCHED      (1<<9)
#define DEBUGFLAG_STATIC        (1<<10)
#define DEBUGFLAG_CONTINUESTART (1<<11)
#define DEBUGFLAG_CONTINUE      (1<<12)
#define DEBUGFLAG_CONTINUEEND   (1<<13)

extern struct xadMasterBase * xadMasterBase;

xadSTRPTR xadGetObjectTypeName(xadUINT32 type); /* objects.c */
#endif

#endif /* XADMASTER_FUNCTIONS_H */
