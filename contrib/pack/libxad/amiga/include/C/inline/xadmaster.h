#ifndef _INLINE_XADMASTER_H
#define _INLINE_XADMASTER_H

#ifndef CLIB_XADMASTER_PROTOS_H
#define CLIB_XADMASTER_PROTOS_H
#endif

#if !defined(__AROS__) 
#ifndef __INLINE_MACROS_H
#include <inline/macros.h>
#endif
#else
#include <aros/asmcall.h>
#include <aros/libcall.h>
#endif

#ifndef  UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif
#ifndef  LIBRARIES_XADMASTER_H
#include <libraries/xadmaster.h>
#endif

#ifndef XADMASTER_BASE_NAME
#define XADMASTER_BASE_NAME xadMasterBase
#endif

#if !defined(__AROS__)
#define xadAllocObjectA(type, tags) \
	LP2(0x1e, xadPTR, xadAllocObjectA, xadUINT32, type, d0, const struct TagItem *, tags, a0, \
	, XADMASTER_BASE_NAME)

#define xadFreeObjectA(object, tags) \
	LP2NR(0x24, xadFreeObjectA, xadPTR, object, a0, const struct TagItem *, tags, a1, \
	, XADMASTER_BASE_NAME)

#define xadRecogFileA(size, memory, tags) \
	LP3(0x2a, struct xadClient *, xadRecogFileA, xadSize, size, d0, const void *, memory, a0, const struct TagItem *, tags, a1, \
	, XADMASTER_BASE_NAME)

#define xadGetInfoA(ai, tags) \
	LP2(0x30, xadERROR, xadGetInfoA, struct xadArchiveInfo *, ai, a0, const struct TagItem *, tags, a1, \
	, XADMASTER_BASE_NAME)

#define xadFreeInfo(ai) \
	LP1NR(0x36, xadFreeInfo, struct xadArchiveInfo *, ai, a0, \
	, XADMASTER_BASE_NAME)

#define xadFileUnArcA(ai, tags) \
	LP2(0x3c, xadERROR, xadFileUnArcA, struct xadArchiveInfo *, ai, a0, const struct TagItem *, tags, a1, \
	, XADMASTER_BASE_NAME)

#define xadDiskUnArcA(ai, tags) \
	LP2(0x42, xadERROR, xadDiskUnArcA, struct xadArchiveInfo *, ai, a0, const struct TagItem *, tags, a1, \
	, XADMASTER_BASE_NAME)

#define xadGetErrorText(errnum) \
	LP1(0x48, xadSTRPTR, xadGetErrorText, xadERROR, errnum, d0, \
	, XADMASTER_BASE_NAME)

#define xadGetClientInfo() \
	LP0(0x4e, struct xadClient *, xadGetClientInfo, \
	, XADMASTER_BASE_NAME)

#define xadHookAccess(command, data, buffer, ai) \
	LP4(0x54, xadERROR, xadHookAccess, xadUINT32, command, d0, xadSignSize, data, d1, xadPTR, buffer, a0, struct xadArchiveInfo *, ai, a1, \
	, XADMASTER_BASE_NAME)

#define xadConvertDatesA(tags) \
	LP1(0x5a, xadERROR, xadConvertDatesA, const struct TagItem *, tags, a0, \
	, XADMASTER_BASE_NAME)

#define xadCalcCRC16(id, init, size, buffer) \
	LP4(0x60, xadUINT16, xadCalcCRC16, xadUINT32, id, d0, xadUINT32, init, d1, xadSize, size, d2, const xadUINT8 *, buffer, a0, \
	, XADMASTER_BASE_NAME)

#define xadCalcCRC32(id, init, size, buffer) \
	LP4(0x66, xadUINT32, xadCalcCRC32, xadUINT32, id, d0, xadUINT32, init, d1, xadSize, size, d2, const xadUINT8 *, buffer, a0, \
	, XADMASTER_BASE_NAME)

#define xadAllocVec(size, flags) \
	LP2(0x6c, xadPTR, xadAllocVec, xadSize, size, d0, xadUINT32, flags, d1, \
	, XADMASTER_BASE_NAME)

#define xadCopyMem(src, dest, size) \
	LP3NR(0x72, xadCopyMem, const void *, src, a0, xadPTR, dest, a1, xadSize, size, d0, \
	, XADMASTER_BASE_NAME)

#define xadHookTagAccessA(command, data, buffer, ai, tags) \
	LP5(0x78, xadERROR, xadHookTagAccessA, xadUINT32, command, d0, xadSignSize, data, d1, xadPTR, buffer, a0, struct xadArchiveInfo *, ai, a1, const struct TagItem *, tags, a2, \
	, XADMASTER_BASE_NAME)

#define xadConvertProtectionA(tags) \
	LP1(0x7e, xadERROR, xadConvertProtectionA, const struct TagItem *, tags, a0, \
	, XADMASTER_BASE_NAME)

#define xadGetDiskInfoA(ai, tags) \
	LP2(0x84, xadERROR, xadGetDiskInfoA, struct xadArchiveInfo *, ai, a0, const struct TagItem *, tags, a1, \
	, XADMASTER_BASE_NAME)

#define xadGetHookAccessA(ai, tags) \
	LP2(0x90, xadERROR, xadGetHookAccessA, struct xadArchiveInfo *, ai, a0, const struct TagItem *, tags, a1, \
	, XADMASTER_BASE_NAME)

#define xadFreeHookAccessA(ai, tags) \
	LP2NR(0x96, xadFreeHookAccessA, struct xadArchiveInfo *, ai, a0, const struct TagItem *, tags, a1, \
	, XADMASTER_BASE_NAME)

#define xadAddFileEntryA(fi, ai, tags) \
	LP3(0x9c, xadERROR, xadAddFileEntryA, struct xadFileInfo *, fi, a0, struct xadArchiveInfo *, ai, a1, const struct TagItem *, tags, a2, \
	, XADMASTER_BASE_NAME)

#define xadAddDiskEntryA(di, ai, tags) \
	LP3(0xa2, xadERROR, xadAddDiskEntryA, struct xadDiskInfo *, di, a0, struct xadArchiveInfo *, ai, a1, const struct TagItem *, tags, a2, \
	, XADMASTER_BASE_NAME)

#define xadGetFilenameA(buffersize, buffer, path, name, tags) \
	LP5(0xa8, xadERROR, xadGetFilenameA, xadUINT32, buffersize, d0, xadSTRPTR, buffer, a0, const xadSTRING *, path, a1, const xadSTRING *, name, a2, const struct TagItem *, tags, a3, \
	, XADMASTER_BASE_NAME)

#define xadConvertNameA(charset, tags) \
	LP2(0xae, xadSTRPTR, xadConvertNameA, xadUINT32, charset, d0, const struct TagItem *, tags, a0, \
	, XADMASTER_BASE_NAME)

#define xadGetDefaultNameA(tags) \
	LP1(0xb4, xadSTRPTR, xadGetDefaultNameA, const struct TagItem *, tags, a0, \
	, XADMASTER_BASE_NAME)

#define xadGetSystemInfo() \
	LP0(0xba, const struct xadSystemInfo *, xadGetSystemInfo, \
	, XADMASTER_BASE_NAME)
#else
#define xadAllocObjectA(type, tags) \
    	AROS_LC2(xadPTR, xadAllocObjectA, \
    		AROS_LCA(xadUINT32, type, D0), \
    		AROS_LCA(const struct TagItem *, tags, A0), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 5, xadmaster)

#define xadFreeObjectA(object, tags) \
    	AROS_LC2NR(void, xadFreeObjectA, \
    		AROS_LCA(xadPTR, object, A0), \
    		AROS_LCA(const struct TagItem *, tags, A1), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 6, xadmaster)

#define xadRecogFileA(size, memory, tags) \
    	AROS_LC3(struct xadClient *, xadRecogFileA, \
    		AROS_LCA(xadSize, size, D0), \
    		AROS_LCA(const void *, memory, A0), \
    		AROS_LCA(const struct TagItem *, tags, A1), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 7, xadmaster)

#define xadGetInfoA(ai, tags) \
    	AROS_LC2(xadERROR, xadGetInfoA, \
    		AROS_LCA(struct xadArchiveInfo *, ai, A0), \
    		AROS_LCA(const struct TagItem *, tags, A1), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 8, xadmaster)

#define xadFreeInfo(ai) \
    	AROS_LC1NR(void, xadFreeInfo, \
    		AROS_LCA(struct xadArchiveInfo *, ai, A0), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 9, xadmaster)

#define xadFileUnArcA(ai, tags) \
    	AROS_LC2(xadERROR, xadFileUnArcA, \
    		AROS_LCA(struct xadArchiveInfo *, ai, A0), \
    		AROS_LCA(const struct TagItem *, tags, A1), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 10, xadmaster)

#define xadDiskUnArcA(ai, tags) \
    	AROS_LC2(xadERROR, xadDiskUnArcA, \
    		AROS_LCA(struct xadArchiveInfo *, ai, A0), \
    		AROS_LCA(const struct TagItem *, tags, A1), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 11, xadmaster)

#define xadGetErrorText(errnum) \
    	AROS_LC1(xadSTRPTR, xadGetErrorText, \
    		AROS_LCA(xadERROR, errnum, D0), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 12, xadmaster)

#define xadGetClientInfo() \
    	AROS_LC0(struct xadClient *, xadGetClientInfo, \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 13, xadmaster)

#define xadHookAccess(command, data, buffer, ai) \
    	AROS_LC4(xadERROR, xadHookAccess, \
    		AROS_LCA(xadUINT32, command, D0), \
    		AROS_LCA(xadSignSize, data, D1), \
    		AROS_LCA(xadPTR, buffer, A0), \
    		AROS_LCA(struct xadArchiveInfo *, ai, A1), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 14, xadmaster)

#define xadConvertDatesA(tags) \
    	AROS_LC1(xadERROR, xadConvertDatesA, \
    		AROS_LCA(const struct TagItem *, tags, A0), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 15, xadmaster)

#define xadCalcCRC16(id, init, size, buffer) \
    	AROS_LC4(xadUINT16, xadCalcCRC16, \
    		AROS_LCA(xadUINT32, id, D0), \
    		AROS_LCA(xadUINT32, init, D1), \
    		AROS_LCA(xadSize, size, D2), \
    		AROS_LCA(const xadUINT8 *, buffer, A0), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 16, xadmaster)

#define xadCalcCRC32(id, init, size, buffer) \
    	AROS_LC4(xadUINT32, xadCalcCRC32, \
    		AROS_LCA(xadUINT32, id, D0), \
    		AROS_LCA(xadUINT32, init, D1), \
    		AROS_LCA(xadSize, size, D2), \
    		AROS_LCA(const xadUINT8 *, buffer, A0), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 17, xadmaster)

#define xadAllocVec(size, flags) \
    	AROS_LC2(xadPTR, xadAllocVec, \
    		AROS_LCA(xadSize, size, D0), \
    		AROS_LCA(xadUINT32, flags, D1), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 18, xadmaster)

#define xadCopyMem(src, dest, size) \
    	AROS_LC3NR(void, xadCopyMem, \
    		AROS_LCA(const void *, src, A0), \
    		AROS_LCA(xadPTR, dest, A1), \
    		AROS_LCA(xadSize, size, D0), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 19, xadmaster)

#define xadHookTagAccessA(command, data, buffer, ai, tags) \
    	AROS_LC5(xadERROR, xadHookTagAccessA, \
    		AROS_LCA(xadUINT32, command, D0), \
    		AROS_LCA(xadSignSize, data, D1), \
    		AROS_LCA(xadPTR, buffer, A0), \
    		AROS_LCA(struct xadArchiveInfo *, ai, A1), \
    		AROS_LCA(const struct TagItem *, tags, A2), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 20, xadmaster)

#define xadConvertProtectionA(tags) \
    	AROS_LC1(xadERROR, xadConvertProtectionA, \
    		AROS_LCA(const struct TagItem *, tags, A0), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 21, xadmaster)

#define xadGetDiskInfoA(ai, tags) \
    	AROS_LC2(xadERROR, xadGetDiskInfoA, \
    		AROS_LCA(struct xadArchiveInfo *, ai, A0), \
    		AROS_LCA(const struct TagItem *, tags, A1), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 22, xadmaster)

#define xadGetHookAccessA(ai, tags) \
    	AROS_LC2(xadERROR, xadGetHookAccessA, \
    		AROS_LCA(struct xadArchiveInfo *, ai, A0), \
    		AROS_LCA(const struct TagItem *, tags, A1), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 23, xadmaster)

#define xadFreeHookAccessA(ai, tags) \
    	AROS_LC2NR(void, xadFreeHookAccessA, \
    		AROS_LCA(struct xadArchiveInfo *, ai, A0), \
    		AROS_LCA(const struct TagItem *, tags, A1), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 24, xadmaster)

#define xadAddFileEntryA(fi, ai, tags) \
    	AROS_LC3(xadERROR, xadAddFileEntryA, \
    		AROS_LCA(struct xadFileInfo *, fi, A0), \
    		AROS_LCA(struct xadArchiveInfo *, ai, A1), \
    		AROS_LCA(const struct TagItem *, tags, A2), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 25, xadmaster)

#define xadAddDiskEntryA(di, ai, tags) \
    	AROS_LC3(xadERROR, xadAddDiskEntryA, \
    		AROS_LCA(struct xadDiskInfo *, di, A0), \
    		AROS_LCA(struct xadArchiveInfo *, ai, A1), \
    		AROS_LCA(const struct TagItem *, tags, A2), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 26, xadmaster)

#define xadGetFilenameA(buffersize, buffer, path, name, tags) \
    	AROS_LC5(xadERROR, xadGetFilenameA, \
    		AROS_LCA(xadUINT32, buffersize, D0), \
    		AROS_LCA(xadSTRPTR, buffer, A0), \
    		AROS_LCA(const xadSTRING *, path, A1), \
    		AROS_LCA(const xadSTRING *, name, A2), \
    		AROS_LCA(const struct TagItem *, tags, A3), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 27, xadmaster)

#define xadConvertNameA(charset, tags) \
    	AROS_LC2(xadSTRPTR, xadConvertNameA, \
    		AROS_LCA(xadUINT32, charset, D0), \
    		AROS_LCA(const struct TagItem *, tags, A0), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 28, xadmaster)

#define xadGetDefaultNameA(tags) \
    	AROS_LC1(xadSTRPTR, xadGetDefaultNameA, \
    		AROS_LCA(const struct TagItem *, tags, A0), \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 29, xadmaster)

#define xadGetSystemInfo() \
    	AROS_LC0(const struct xadSystemInfo *, xadGetSystemInfo, \
    		struct xadMasterBaseP *, XADMASTER_BASE_NAME, 30, xadmaster)
#endif

#ifndef NO_INLINE_STDARG
#define xadAllocObject(type, tags...) \
	({ULONG _tags[] = {tags}; xadAllocObjectA((type), (const struct TagItem *) _tags);})

#define xadFreeObject(object, tags...) \
	({ULONG _tags[] = {tags}; xadFreeObjectA((object), (const struct TagItem *) _tags);})

#define xadRecogFile(size, memory, tags...) \
	({ULONG _tags[] = {tags}; xadRecogFileA((size), (memory), (const struct TagItem *) _tags);})

#define xadGetInfo(ai, tags...) \
	({ULONG _tags[] = {tags}; xadGetInfoA((ai), (const struct TagItem *) _tags);})

#define xadFileUnArc(ai, tags...) \
	({ULONG _tags[] = {tags}; xadFileUnArcA((ai), (const struct TagItem *) _tags);})

#define xadDiskUnArc(ai, tags...) \
	({ULONG _tags[] = {tags}; xadDiskUnArcA((ai), (const struct TagItem *) _tags);})

#define xadConvertDates(tags...) \
	({ULONG _tags[] = {tags}; xadConvertDatesA((const struct TagItem *) _tags);})

#define xadHookTagAccess(command, data, buffer, ai, tags...) \
	({ULONG _tags[] = {tags}; xadHookTagAccessA((command), (data), (buffer), (ai), (const struct TagItem *) _tags);})

#define xadConvertProtection(tags...) \
	({ULONG _tags[] = {tags}; xadConvertProtectionA((const struct TagItem *) _tags);})

#define xadGetDiskInfo(ai, tags...) \
	({ULONG _tags[] = {tags}; xadGetDiskInfoA((ai), (const struct TagItem *) _tags);})

#define xadGetHookAccess(ai, tags...) \
	({ULONG _tags[] = {tags}; xadGetHookAccessA((ai), (const struct TagItem *) _tags);})

#define xadFreeHookAccess(ai, tags...) \
	({ULONG _tags[] = {tags}; xadFreeHookAccessA((ai), (const struct TagItem *) _tags);})

#define xadAddFileEntry(fi, ai, tags...) \
	({ULONG _tags[] = {tags}; xadAddFileEntryA((fi), (ai), (const struct TagItem *) _tags);})

#define xadAddDiskEntry(di, ai, tags...) \
	({ULONG _tags[] = {tags}; xadAddDiskEntryA((di), (ai), (const struct TagItem *) _tags);})

#define xadGetFilename(buffersize, buffer, path, name, tags...) \
	({ULONG _tags[] = {tags}; xadGetFilenameA((buffersize), (buffer), (path), (name), (const struct TagItem *) _tags);})

#define xadConvertName(charset, tags...) \
	({ULONG _tags[] = {tags}; xadConvertNameA((charset), (const struct TagItem *) _tags);})

#define xadGetDefaultName(tags...) \
	({ULONG _tags[] = {tags}; xadGetDefaultNameA((const struct TagItem *) _tags);})
#endif

#endif /*  _INLINE_XADMASTER_H  */
