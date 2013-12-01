// FsAbstraction.h
// $Date$
// $Revision$

#ifndef	SCALOS_FSABSTRACTION_H
#define SCALOS_FSABSTRACTION_H

#include <dos/dos.h>

#ifdef __amigaos4__

typedef struct ExamineData T_ExamineData;
typedef APTR T_ExamineDirHandle;

#else //__amigaos4__

typedef struct FileInfoBlock T_ExamineData;
typedef struct FileInfoBlock *T_ExamineDirHandle;

#endif //__amigaos4__


BOOL ScalosExamineBegin(T_ExamineData **exd);
void ScalosExamineEnd(T_ExamineData **exd);
BOOL ScalosExamineLock(BPTR lock, T_ExamineData **exd);
BOOL ScalosExamineFH(BPTR fh, T_ExamineData **exd);

struct DateStamp *ScalosExamineGetDate(T_ExamineData *exd);
ULONG ScalosExamineGetProtection(const T_ExamineData *exd);
ULONG64 ScalosExamineGetSize(const T_ExamineData *exd);
ULONG ScalosExamineGetBlockCount(const T_ExamineData *exd);
CONST_STRPTR ScalosExamineGetName(const T_ExamineData *exd);
CONST_STRPTR ScalosExamineGetComment(const T_ExamineData *exd);
LONG ScalosExamineGetDirEntryType(const T_ExamineData *exd);
LONG ScalosExamineGetDirEntryTypeRoot(const T_ExamineData *exd, BPTR fLock);
LONG ScalosExamineIsDrawer(const T_ExamineData *exd);
ULONG ScalosExamineGetDirUID(const T_ExamineData *exd);
ULONG ScalosExamineGetDirGID(const T_ExamineData *exd);

SLONG64 ScalosSeek(BPTR fh, SLONG64 pos, LONG mode);

BOOL ScalosExamineDirBegin(BPTR lock, T_ExamineDirHandle *edh);
void ScalosExamineDirEnd(T_ExamineDirHandle *edh);
BOOL ScalosExamineDir(BPTR lock, T_ExamineDirHandle *edh, T_ExamineData **exd);

BOOL ScalosReadLink(BPTR srcDirLock, CONST_STRPTR srcName, STRPTR Buffer, size_t BuffLen);

void ScalosDosPacketExamineDir(struct DosPacket *pkt, BPTR dirLock, T_ExamineDirHandle *edh);
BOOL ScalosDosPacketExamineDirResult(struct StandardPacket *pkt, T_ExamineData **exd,
	T_ExamineDirHandle *edh);
BOOL ScalosSupportsExamineDir(struct MsgPort *fileSysPort, BPTR dirLock, T_ExamineDirHandle *edh);

#endif /* SCALOS_FSABSTRACTION_H */

