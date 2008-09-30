#ifndef CLIB_DOS_PROTOS_H
#define CLIB_DOS_PROTOS_H

/*
    *** Automatically generated from 'dos.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/record.h>
#include <dos/rdargs.h>
#include <dos/dosasl.h>
#include <dos/var.h>
#include <dos/notify.h>
#include <dos/datetime.h>
#include <dos/dostags.h>
#include <dos/exall.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

#define AllocDosObjectTagList(...) AllocDosObject(__VA_ARGS__)
#define CreateNewProcTagList(...) CreateNewProc(__VA_ARGS__)
#define NewLoadSegTagList(...) NewLoadSeg(__VA_ARGS__)
#define System(...) SystemTagList(__VA_ARGS__)

/* Prototypes for stubs in amiga.lib */
APTR AllocDosObjectTags (ULONG type, Tag tag1, ...) __stackparm;
struct Process * CreateNewProcTags (Tag tag1, ...) __stackparm;
BPTR NewLoadSegTags (STRPTR file, Tag tag1, ...) __stackparm;
LONG SystemTags (STRPTR command, Tag tag1, ...) __stackparm;

#if !defined(NO_INLINE_STDARG) && defined(__GNUC__)
#define FWritef(fh, format, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    VFWritef((fh), (format), __args); \
})
 
#define FPrintf(fh, format, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    VFPrintf((fh), (format), __args); \
})

#define Printf(format, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    VFPrintf(Output(), (format), __args); \
})
#endif

AROS_LP2(BPTR, Open,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(LONG, accessMode, D2),
         LIBBASETYPEPTR, DOSBase, 5, Dos
);
AROS_LP1(BOOL, Close,
         AROS_LPA(BPTR, file, D1),
         LIBBASETYPEPTR, DOSBase, 6, Dos
);
AROS_LP3(LONG, Read,
         AROS_LPA(BPTR, file, D1),
         AROS_LPA(APTR, buffer, D2),
         AROS_LPA(LONG, length, D3),
         LIBBASETYPEPTR, DOSBase, 7, Dos
);
AROS_LP3(LONG, Write,
         AROS_LPA(BPTR, file, D1),
         AROS_LPA(CONST_APTR, buffer, D2),
         AROS_LPA(LONG, length, D3),
         LIBBASETYPEPTR, DOSBase, 8, Dos
);
AROS_LP0(BPTR, Input,
         LIBBASETYPEPTR, DOSBase, 9, Dos
);
AROS_LP0(BPTR, Output,
         LIBBASETYPEPTR, DOSBase, 10, Dos
);
AROS_LP3(LONG, Seek,
         AROS_LPA(BPTR, file, D1),
         AROS_LPA(LONG, position, D2),
         AROS_LPA(LONG, mode, D3),
         LIBBASETYPEPTR, DOSBase, 11, Dos
);
AROS_LP1(BOOL, DeleteFile,
         AROS_LPA(CONST_STRPTR, name, D1),
         LIBBASETYPEPTR, DOSBase, 12, Dos
);
AROS_LP2(LONG, Rename,
         AROS_LPA(CONST_STRPTR, oldName, D1),
         AROS_LPA(CONST_STRPTR, newName, D2),
         LIBBASETYPEPTR, DOSBase, 13, Dos
);
AROS_LP2(BPTR, Lock,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(LONG, accessMode, D2),
         LIBBASETYPEPTR, DOSBase, 14, Dos
);
AROS_LP1(BPTR, DupLock,
         AROS_LPA(BPTR, lock, D1),
         LIBBASETYPEPTR, DOSBase, 16, Dos
);
AROS_LP2(BOOL, Examine,
         AROS_LPA(BPTR, lock, D1),
         AROS_LPA(struct FileInfoBlock*, fib, D2),
         LIBBASETYPEPTR, DOSBase, 17, Dos
);
AROS_LP2(BOOL, ExNext,
         AROS_LPA(BPTR, lock, D1),
         AROS_LPA(struct FileInfoBlock*, fileInfoBlock, D2),
         LIBBASETYPEPTR, DOSBase, 18, Dos
);
AROS_LP2(BOOL, Info,
         AROS_LPA(BPTR, lock, D1),
         AROS_LPA(struct InfoData*, parameterBlock, D2),
         LIBBASETYPEPTR, DOSBase, 19, Dos
);
AROS_LP1(BPTR, CreateDir,
         AROS_LPA(CONST_STRPTR, name, D1),
         LIBBASETYPEPTR, DOSBase, 20, Dos
);
AROS_LP1(BPTR, CurrentDir,
         AROS_LPA(BPTR, lock, D1),
         LIBBASETYPEPTR, DOSBase, 21, Dos
);
AROS_LP0(LONG, IoErr,
         LIBBASETYPEPTR, DOSBase, 22, Dos
);
AROS_LP4(struct MsgPort*, CreateProc,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(LONG, pri, D2),
         AROS_LPA(BPTR, segList, D3),
         AROS_LPA(LONG, stackSize, D4),
         LIBBASETYPEPTR, DOSBase, 23, Dos
);
AROS_LP1(void, Exit,
         AROS_LPA(LONG, returnCode, D1),
         LIBBASETYPEPTR, DOSBase, 24, Dos
);
AROS_LP1(BPTR, LoadSeg,
         AROS_LPA(CONST_STRPTR, name, D1),
         LIBBASETYPEPTR, DOSBase, 25, Dos
);
AROS_LP1(BOOL, UnLoadSeg,
         AROS_LPA(BPTR, seglist, D1),
         LIBBASETYPEPTR, DOSBase, 26, Dos
);
AROS_LP1(struct Device *, DeviceProc,
         AROS_LPA(CONST_STRPTR, name, D1),
         LIBBASETYPEPTR, DOSBase, 29, Dos
);
AROS_LP2(BOOL, SetComment,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(CONST_STRPTR, comment, D2),
         LIBBASETYPEPTR, DOSBase, 30, Dos
);
AROS_LP2(BOOL, SetProtection,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(ULONG, protect, D2),
         LIBBASETYPEPTR, DOSBase, 31, Dos
);
AROS_LP1(struct DateStamp *, DateStamp,
         AROS_LPA(struct DateStamp *, date, D1),
         LIBBASETYPEPTR, DOSBase, 32, Dos
);
AROS_LP1(void, Delay,
         AROS_LPA(ULONG, timeout, D1),
         LIBBASETYPEPTR, DOSBase, 33, Dos
);
AROS_LP2(BOOL, WaitForChar,
         AROS_LPA(BPTR, file, D1),
         AROS_LPA(LONG, timeout, D2),
         LIBBASETYPEPTR, DOSBase, 34, Dos
);
AROS_LP1(BPTR, ParentDir,
         AROS_LPA(BPTR, lock, D1),
         LIBBASETYPEPTR, DOSBase, 35, Dos
);
AROS_LP1(BOOL, IsInteractive,
         AROS_LPA(BPTR, file, D1),
         LIBBASETYPEPTR, DOSBase, 36, Dos
);
AROS_LP3(LONG, Execute,
         AROS_LPA(CONST_STRPTR, string, D1),
         AROS_LPA(BPTR, input, D2),
         AROS_LPA(BPTR, output, D3),
         LIBBASETYPEPTR, DOSBase, 37, Dos
);
AROS_LP2(APTR, AllocDosObject,
         AROS_LPA(ULONG, type, D1),
         AROS_LPA(struct TagItem *, tags, D2),
         LIBBASETYPEPTR, DOSBase, 38, Dos
);
AROS_LP2(void, FreeDosObject,
         AROS_LPA(ULONG, type, D1),
         AROS_LPA(APTR, ptr, D2),
         LIBBASETYPEPTR, DOSBase, 39, Dos
);
AROS_LP7(LONG, DoPkt,
         AROS_LPA(struct MsgPort*, port, D1),
         AROS_LPA(LONG, action, D2),
         AROS_LPA(LONG, arg1, D3),
         AROS_LPA(LONG, arg2, D4),
         AROS_LPA(LONG, arg3, D5),
         AROS_LPA(LONG, arg4, D6),
         AROS_LPA(LONG, arg5, D7),
         LIBBASETYPEPTR, DOSBase, 40, Dos
);
AROS_LP3(void, SendPkt,
         AROS_LPA(struct DosPacket *, dp, D1),
         AROS_LPA(struct MsgPort *, port, D2),
         AROS_LPA(struct MsgPort *, replyport, D3),
         LIBBASETYPEPTR, DOSBase, 41, Dos
);
AROS_LP0(struct DosPacket *, WaitPkt,
         LIBBASETYPEPTR, DOSBase, 42, Dos
);
AROS_LP3(void, ReplyPkt,
         AROS_LPA(struct DosPacket *, dp, D1),
         AROS_LPA(LONG, res1, D2),
         AROS_LPA(LONG, res2, D3),
         LIBBASETYPEPTR, DOSBase, 43, Dos
);
AROS_LP2(void, AbortPkt,
         AROS_LPA(struct MsgPort *, port, D1),
         AROS_LPA(struct DosPacket *, pkt, D2),
         LIBBASETYPEPTR, DOSBase, 44, Dos
);
AROS_LP5(BOOL, LockRecord,
         AROS_LPA(BPTR, fh, D1),
         AROS_LPA(ULONG, offset, D2),
         AROS_LPA(ULONG, length, D3),
         AROS_LPA(ULONG, mode, D4),
         AROS_LPA(ULONG, timeout, D5),
         LIBBASETYPEPTR, DOSBase, 45, Dos
);
AROS_LP2(BOOL, LockRecords,
         AROS_LPA(struct RecordLock *, recArray, D1),
         AROS_LPA(ULONG, timeout, D2),
         LIBBASETYPEPTR, DOSBase, 46, Dos
);
AROS_LP3(BOOL, UnLockRecord,
         AROS_LPA(BPTR, fh, D1),
         AROS_LPA(ULONG, offset, D2),
         AROS_LPA(ULONG, length, D3),
         LIBBASETYPEPTR, DOSBase, 47, Dos
);
AROS_LP1(BOOL, UnLockRecords,
         AROS_LPA(struct RecordLock *, recArray, D1),
         LIBBASETYPEPTR, DOSBase, 48, Dos
);
AROS_LP1(BPTR, SelectInput,
         AROS_LPA(BPTR, fh, D1),
         LIBBASETYPEPTR, DOSBase, 49, Dos
);
AROS_LP1(BPTR, SelectOutput,
         AROS_LPA(BPTR, fh, D1),
         LIBBASETYPEPTR, DOSBase, 50, Dos
);
AROS_LP1(LONG, FGetC,
         AROS_LPA(BPTR, file, D1),
         LIBBASETYPEPTR, DOSBase, 51, Dos
);
AROS_LP2(LONG, FPutC,
         AROS_LPA(BPTR, file, D1),
         AROS_LPA(LONG, character, D2),
         LIBBASETYPEPTR, DOSBase, 52, Dos
);
AROS_LP2(LONG, UnGetC,
         AROS_LPA(BPTR, file, D1),
         AROS_LPA(LONG, character, D2),
         LIBBASETYPEPTR, DOSBase, 53, Dos
);
AROS_LP4(LONG, FRead,
         AROS_LPA(BPTR, fh, D1),
         AROS_LPA(APTR, block, D2),
         AROS_LPA(ULONG, blocklen, D3),
         AROS_LPA(ULONG, number, D4),
         LIBBASETYPEPTR, DOSBase, 54, Dos
);
AROS_LP4(LONG, FWrite,
         AROS_LPA(BPTR, fh, D1),
         AROS_LPA(CONST_APTR, block, D2),
         AROS_LPA(ULONG, blocklen, D3),
         AROS_LPA(ULONG, numblocks, D4),
         LIBBASETYPEPTR, DOSBase, 55, Dos
);
AROS_LP3(STRPTR, FGets,
         AROS_LPA(BPTR, fh, D1),
         AROS_LPA(STRPTR, buf, D2),
         AROS_LPA(ULONG, buflen, D3),
         LIBBASETYPEPTR, DOSBase, 56, Dos
);
AROS_LP2(LONG, FPuts,
         AROS_LPA(BPTR, file, D1),
         AROS_LPA(CONST_STRPTR, string, D2),
         LIBBASETYPEPTR, DOSBase, 57, Dos
);
AROS_LP3(LONG, VFWritef,
         AROS_LPA(BPTR, fh, D1),
         AROS_LPA(CONST_STRPTR, fmt, D2),
         AROS_LPA(const IPTR *, argarray, D3),
         LIBBASETYPEPTR, DOSBase, 58, Dos
);
AROS_LP3(LONG, VFPrintf,
         AROS_LPA(BPTR, file, D1),
         AROS_LPA(CONST_STRPTR, format, D2),
         AROS_LPA(const IPTR *, argarray, D3),
         LIBBASETYPEPTR, DOSBase, 59, Dos
);
AROS_LP1(LONG, Flush,
         AROS_LPA(BPTR, file, D1),
         LIBBASETYPEPTR, DOSBase, 60, Dos
);
AROS_LP4(LONG, SetVBuf,
         AROS_LPA(BPTR, file, D1),
         AROS_LPA(STRPTR, buff, D2),
         AROS_LPA(LONG, type, D3),
         AROS_LPA(LONG, size, D4),
         LIBBASETYPEPTR, DOSBase, 61, Dos
);
AROS_LP1(BPTR, OpenFromLock,
         AROS_LPA(BPTR, lock, D1),
         LIBBASETYPEPTR, DOSBase, 63, Dos
);
AROS_LP1(BPTR, ParentOfFH,
         AROS_LPA(BPTR, fh, D1),
         LIBBASETYPEPTR, DOSBase, 64, Dos
);
AROS_LP2(BOOL, SetFileDate,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(const struct DateStamp *, date, D2),
         LIBBASETYPEPTR, DOSBase, 66, Dos
);
AROS_LP3(BOOL, NameFromLock,
         AROS_LPA(BPTR, lock, D1),
         AROS_LPA(STRPTR, buffer, D2),
         AROS_LPA(LONG, length, D3),
         LIBBASETYPEPTR, DOSBase, 67, Dos
);
AROS_LP5(LONG, SplitName,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(ULONG, separator, D2),
         AROS_LPA(STRPTR, buf, D3),
         AROS_LPA(LONG, oldpos, D4),
         AROS_LPA(LONG, size, D5),
         LIBBASETYPEPTR, DOSBase, 69, Dos
);
AROS_LP2(LONG, SameLock,
         AROS_LPA(BPTR, lock1, D1),
         AROS_LPA(BPTR, lock2, D2),
         LIBBASETYPEPTR, DOSBase, 70, Dos
);
AROS_LP2(LONG, SetMode,
         AROS_LPA(BPTR, fh, D1),
         AROS_LPA(LONG, mode, D2),
         LIBBASETYPEPTR, DOSBase, 71, Dos
);
AROS_LP5(BOOL, ExAll,
         AROS_LPA(BPTR, lock, D1),
         AROS_LPA(struct ExAllData *, buffer, D2),
         AROS_LPA(LONG, size, D3),
         AROS_LPA(LONG, data, D4),
         AROS_LPA(struct ExAllControl *, control, D5),
         LIBBASETYPEPTR, DOSBase, 72, Dos
);
AROS_LP5(LONG, ReadLink,
         AROS_LPA(struct MsgPort *, port, D1),
         AROS_LPA(BPTR, lock, D2),
         AROS_LPA(CONST_STRPTR, path, D3),
         AROS_LPA(STRPTR, buffer, D4),
         AROS_LPA(ULONG, size, D5),
         LIBBASETYPEPTR, DOSBase, 73, Dos
);
AROS_LP3(LONG, MakeLink,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(APTR, dest, D2),
         AROS_LPA(LONG, soft, D3),
         LIBBASETYPEPTR, DOSBase, 74, Dos
);
AROS_LP3(BOOL, ChangeMode,
         AROS_LPA(ULONG, type, D1),
         AROS_LPA(BPTR, object, D2),
         AROS_LPA(ULONG, newmode, D3),
         LIBBASETYPEPTR, DOSBase, 75, Dos
);
AROS_LP3(LONG, SetFileSize,
         AROS_LPA(BPTR, file, D1),
         AROS_LPA(LONG, offset, D2),
         AROS_LPA(LONG, mode, D3),
         LIBBASETYPEPTR, DOSBase, 76, Dos
);
AROS_LP1(LONG, SetIoErr,
         AROS_LPA(LONG, result, D1),
         LIBBASETYPEPTR, DOSBase, 77, Dos
);
AROS_LP4(BOOL, Fault,
         AROS_LPA(LONG, code, D1),
         AROS_LPA(CONST_STRPTR, header, D2),
         AROS_LPA(STRPTR, buffer, D3),
         AROS_LPA(LONG, len, D4),
         LIBBASETYPEPTR, DOSBase, 78, Dos
);
AROS_LP2(BOOL, PrintFault,
         AROS_LPA(LONG, code, D1),
         AROS_LPA(CONST_STRPTR, header, D2),
         LIBBASETYPEPTR, DOSBase, 79, Dos
);
AROS_LP4(BOOL, ErrorReport,
         AROS_LPA(LONG, code, D1),
         AROS_LPA(LONG, type, D2),
         AROS_LPA(IPTR, arg1, D3),
         AROS_LPA(struct MsgPort *, device, D4),
         LIBBASETYPEPTR, DOSBase, 80, Dos
);
AROS_LP3(LONG, DisplayError,
         AROS_LPA(CONST_STRPTR, formatStr, A0),
         AROS_LPA(ULONG, flags, D0),
         AROS_LPA(APTR, args, A1),
         LIBBASETYPEPTR, DOSBase, 81, Dos
);
AROS_LP0(struct CommandLineInterface *, Cli,
         LIBBASETYPEPTR, DOSBase, 82, Dos
);
AROS_LP1(struct Process *, CreateNewProc,
         AROS_LPA(const struct TagItem *, tags, D1),
         LIBBASETYPEPTR, DOSBase, 83, Dos
);
AROS_LP4(LONG, RunCommand,
         AROS_LPA(BPTR, segList, D1),
         AROS_LPA(ULONG, stacksize, D2),
         AROS_LPA(STRPTR, argptr, D3),
         AROS_LPA(ULONG, argsize, D4),
         LIBBASETYPEPTR, DOSBase, 84, Dos
);
AROS_LP0(struct MsgPort *, GetConsoleTask,
         LIBBASETYPEPTR, DOSBase, 85, Dos
);
AROS_LP1(struct MsgPort *, SetConsoleTask,
         AROS_LPA(struct MsgPort *, handler, D1),
         LIBBASETYPEPTR, DOSBase, 86, Dos
);
AROS_LP0(struct MsgPort *, GetFileSysTask,
         LIBBASETYPEPTR, DOSBase, 87, Dos
);
AROS_LP1(struct MsgPort *, SetFileSysTask,
         AROS_LPA(struct MsgPort *, task, D1),
         LIBBASETYPEPTR, DOSBase, 88, Dos
);
AROS_LP0(STRPTR, GetArgStr,
         LIBBASETYPEPTR, DOSBase, 89, Dos
);
AROS_LP1(STRPTR, SetArgStr,
         AROS_LPA(CONST_STRPTR, string, D1),
         LIBBASETYPEPTR, DOSBase, 90, Dos
);
AROS_LP1(struct Process *, FindCliProc,
         AROS_LPA(ULONG, num, D1),
         LIBBASETYPEPTR, DOSBase, 91, Dos
);
AROS_LP0(ULONG, MaxCli,
         LIBBASETYPEPTR, DOSBase, 92, Dos
);
AROS_LP1(BOOL, SetCurrentDirName,
         AROS_LPA(CONST_STRPTR, name, D1),
         LIBBASETYPEPTR, DOSBase, 93, Dos
);
AROS_LP2(BOOL, GetCurrentDirName,
         AROS_LPA(STRPTR, buf, D1),
         AROS_LPA(LONG, len, D2),
         LIBBASETYPEPTR, DOSBase, 94, Dos
);
AROS_LP1(BOOL, SetProgramName,
         AROS_LPA(CONST_STRPTR, name, D1),
         LIBBASETYPEPTR, DOSBase, 95, Dos
);
AROS_LP2(BOOL, GetProgramName,
         AROS_LPA(STRPTR, buf, D1),
         AROS_LPA(LONG, len, D2),
         LIBBASETYPEPTR, DOSBase, 96, Dos
);
AROS_LP1(BOOL, SetPrompt,
         AROS_LPA(CONST_STRPTR, name, D1),
         LIBBASETYPEPTR, DOSBase, 97, Dos
);
AROS_LP2(BOOL, GetPrompt,
         AROS_LPA(STRPTR, buf, D1),
         AROS_LPA(LONG, len, D2),
         LIBBASETYPEPTR, DOSBase, 98, Dos
);
AROS_LP1(BPTR, SetProgramDir,
         AROS_LPA(BPTR, lock, D1),
         LIBBASETYPEPTR, DOSBase, 99, Dos
);
AROS_LP0(BPTR, GetProgramDir,
         LIBBASETYPEPTR, DOSBase, 100, Dos
);
AROS_LP2(LONG, SystemTagList,
         AROS_LPA(CONST_STRPTR, command, D1),
         AROS_LPA(const struct TagItem *, tags, D2),
         LIBBASETYPEPTR, DOSBase, 101, Dos
);
AROS_LP2(BOOL, AssignLock,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(BPTR, lock, D2),
         LIBBASETYPEPTR, DOSBase, 102, Dos
);
AROS_LP2(BOOL, AssignLate,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(CONST_STRPTR, path, D2),
         LIBBASETYPEPTR, DOSBase, 103, Dos
);
AROS_LP2(BOOL, AssignPath,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(CONST_STRPTR, path, D2),
         LIBBASETYPEPTR, DOSBase, 104, Dos
);
AROS_LP2(BOOL, AssignAdd,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(BPTR, lock, D2),
         LIBBASETYPEPTR, DOSBase, 105, Dos
);
AROS_LP2(LONG, RemAssignList,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(BPTR, lock, D2),
         LIBBASETYPEPTR, DOSBase, 106, Dos
);
AROS_LP2(struct DevProc *, GetDeviceProc,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(struct DevProc *, dp, D2),
         LIBBASETYPEPTR, DOSBase, 107, Dos
);
AROS_LP1(void, FreeDeviceProc,
         AROS_LPA(struct DevProc *, dp, D1),
         LIBBASETYPEPTR, DOSBase, 108, Dos
);
AROS_LP1(struct DosList *, LockDosList,
         AROS_LPA(ULONG, flags, D1),
         LIBBASETYPEPTR, DOSBase, 109, Dos
);
AROS_LP1(void, UnLockDosList,
         AROS_LPA(ULONG, flags, D1),
         LIBBASETYPEPTR, DOSBase, 110, Dos
);
AROS_LP1(struct DosList *, AttemptLockDosList,
         AROS_LPA(ULONG, flags, D1),
         LIBBASETYPEPTR, DOSBase, 111, Dos
);
AROS_LP1(LONG, RemDosEntry,
         AROS_LPA(struct DosList *, dlist, D1),
         LIBBASETYPEPTR, DOSBase, 112, Dos
);
AROS_LP1(LONG, AddDosEntry,
         AROS_LPA(struct DosList *, dlist, D1),
         LIBBASETYPEPTR, DOSBase, 113, Dos
);
AROS_LP3(struct DosList *, FindDosEntry,
         AROS_LPA(struct DosList *, dlist, D1),
         AROS_LPA(CONST_STRPTR, name, D2),
         AROS_LPA(ULONG, flags, D3),
         LIBBASETYPEPTR, DOSBase, 114, Dos
);
AROS_LP2(struct DosList *, NextDosEntry,
         AROS_LPA(struct DosList *, dlist, D1),
         AROS_LPA(ULONG, flags, D2),
         LIBBASETYPEPTR, DOSBase, 115, Dos
);
AROS_LP2(struct DosList *, MakeDosEntry,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(LONG, type, D2),
         LIBBASETYPEPTR, DOSBase, 116, Dos
);
AROS_LP1(void, FreeDosEntry,
         AROS_LPA(struct DosList *, dlist, D1),
         LIBBASETYPEPTR, DOSBase, 117, Dos
);
AROS_LP1(BOOL, IsFileSystem,
         AROS_LPA(CONST_STRPTR, devicename, D1),
         LIBBASETYPEPTR, DOSBase, 118, Dos
);
AROS_LP3(BOOL, Format,
         AROS_LPA(CONST_STRPTR, devicename, D1),
         AROS_LPA(CONST_STRPTR, volumename, D2),
         AROS_LPA(ULONG, dostype, D3),
         LIBBASETYPEPTR, DOSBase, 119, Dos
);
AROS_LP2(LONG, Relabel,
         AROS_LPA(CONST_STRPTR, drive, D1),
         AROS_LPA(CONST_STRPTR, newname, D2),
         LIBBASETYPEPTR, DOSBase, 120, Dos
);
AROS_LP2(LONG, Inhibit,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(LONG, onoff, D2),
         LIBBASETYPEPTR, DOSBase, 121, Dos
);
AROS_LP2(BOOL, AddBuffers,
         AROS_LPA(CONST_STRPTR, devicename, D1),
         AROS_LPA(LONG, numbuffers, D2),
         LIBBASETYPEPTR, DOSBase, 122, Dos
);
AROS_LP2(LONG, CompareDates,
         AROS_LPA(const struct DateStamp *, date1, D1),
         AROS_LPA(const struct DateStamp *, date2, D2),
         LIBBASETYPEPTR, DOSBase, 123, Dos
);
AROS_LP1(BOOL, DateToStr,
         AROS_LPA(struct DateTime *, datetime, D1),
         LIBBASETYPEPTR, DOSBase, 124, Dos
);
AROS_LP1(BOOL, StrToDate,
         AROS_LPA(struct DateTime *, datetime, D1),
         LIBBASETYPEPTR, DOSBase, 125, Dos
);
AROS_LP4(BPTR, InternalLoadSeg,
         AROS_LPA(BPTR, fh, D0),
         AROS_LPA(BPTR, table, A0),
         AROS_LPA(LONG_FUNC, functionarray, A1),
         AROS_LPA(LONG *, stack, A2),
         LIBBASETYPEPTR, DOSBase, 126, Dos
);
AROS_LP2(BOOL, InternalUnLoadSeg,
         AROS_LPA(BPTR, seglist, D1),
         AROS_LPA(VOID_FUNC, freefunc, A1),
         LIBBASETYPEPTR, DOSBase, 127, Dos
);
AROS_LP2(BPTR, NewLoadSeg,
         AROS_LPA(CONST_STRPTR, file, D1),
         AROS_LPA(const struct TagItem *, tags, D2),
         LIBBASETYPEPTR, DOSBase, 128, Dos
);
AROS_LP3(BOOL, AddSegment,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(BPTR, seg, D2),
         AROS_LPA(LONG, type, D3),
         LIBBASETYPEPTR, DOSBase, 129, Dos
);
AROS_LP3(struct Segment *, FindSegment,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(struct Segment *, seg, D2),
         AROS_LPA(BOOL, system, D3),
         LIBBASETYPEPTR, DOSBase, 130, Dos
);
AROS_LP1(LONG, RemSegment,
         AROS_LPA(struct Segment *, seg, D1),
         LIBBASETYPEPTR, DOSBase, 131, Dos
);
AROS_LP1(LONG, CheckSignal,
         AROS_LPA(LONG, mask, D1),
         LIBBASETYPEPTR, DOSBase, 132, Dos
);
AROS_LP3(struct RDArgs *, ReadArgs,
         AROS_LPA(CONST_STRPTR, template, D1),
         AROS_LPA(IPTR *, array, D2),
         AROS_LPA(struct RDArgs *, rdargs, D3),
         LIBBASETYPEPTR, DOSBase, 133, Dos
);
AROS_LP2(LONG, FindArg,
         AROS_LPA(CONST_STRPTR, template, D1),
         AROS_LPA(CONST_STRPTR, keyword, D2),
         LIBBASETYPEPTR, DOSBase, 134, Dos
);
AROS_LP3(LONG, ReadItem,
         AROS_LPA(STRPTR, buffer, D1),
         AROS_LPA(LONG, maxchars, D2),
         AROS_LPA(struct CSource *, input, D3),
         LIBBASETYPEPTR, DOSBase, 135, Dos
);
AROS_LP2(LONG, StrToLong,
         AROS_LPA(CONST_STRPTR, string, D1),
         AROS_LPA(LONG *, value, D2),
         LIBBASETYPEPTR, DOSBase, 136, Dos
);
AROS_LP2(LONG, MatchFirst,
         AROS_LPA(CONST_STRPTR, pat, D1),
         AROS_LPA(struct AnchorPath *, AP, D2),
         LIBBASETYPEPTR, DOSBase, 137, Dos
);
AROS_LP1(LONG, MatchNext,
         AROS_LPA(struct AnchorPath *, AP, D1),
         LIBBASETYPEPTR, DOSBase, 138, Dos
);
AROS_LP1(void, MatchEnd,
         AROS_LPA(struct AnchorPath *, AP, D1),
         LIBBASETYPEPTR, DOSBase, 139, Dos
);
AROS_LP3(LONG, ParsePattern,
         AROS_LPA(CONST_STRPTR, Source, D1),
         AROS_LPA(STRPTR, Dest, D2),
         AROS_LPA(LONG, DestLength, D3),
         LIBBASETYPEPTR, DOSBase, 140, Dos
);
AROS_LP2(BOOL, MatchPattern,
         AROS_LPA(CONST_STRPTR, pat, D1),
         AROS_LPA(CONST_STRPTR, str, D2),
         LIBBASETYPEPTR, DOSBase, 141, Dos
);
AROS_LP0(BPTR, Error,
         LIBBASETYPEPTR, DOSBase, 142, Dos
);
AROS_LP1(void, FreeArgs,
         AROS_LPA(struct RDArgs *, args, D1),
         LIBBASETYPEPTR, DOSBase, 143, Dos
);
AROS_LP1(BPTR, SelectError,
         AROS_LPA(BPTR, fh, D1),
         LIBBASETYPEPTR, DOSBase, 144, Dos
);
AROS_LP1(STRPTR, FilePart,
         AROS_LPA(CONST_STRPTR, path, D1),
         LIBBASETYPEPTR, DOSBase, 145, Dos
);
AROS_LP1(STRPTR, PathPart,
         AROS_LPA(CONST_STRPTR, path, D1),
         LIBBASETYPEPTR, DOSBase, 146, Dos
);
AROS_LP3(BOOL, AddPart,
         AROS_LPA(STRPTR, dirname, D1),
         AROS_LPA(CONST_STRPTR, filename, D2),
         AROS_LPA(ULONG, size, D3),
         LIBBASETYPEPTR, DOSBase, 147, Dos
);
AROS_LP1(BOOL, StartNotify,
         AROS_LPA(struct NotifyRequest *, notify, D1),
         LIBBASETYPEPTR, DOSBase, 148, Dos
);
AROS_LP1(void, EndNotify,
         AROS_LPA(struct NotifyRequest *, notify, D1),
         LIBBASETYPEPTR, DOSBase, 149, Dos
);
AROS_LP4(BOOL, SetVar,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(CONST_STRPTR, buffer, D2),
         AROS_LPA(LONG, size, D3),
         AROS_LPA(LONG, flags, D4),
         LIBBASETYPEPTR, DOSBase, 150, Dos
);
AROS_LP4(LONG, GetVar,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(STRPTR, buffer, D2),
         AROS_LPA(LONG, size, D3),
         AROS_LPA(LONG, flags, D4),
         LIBBASETYPEPTR, DOSBase, 151, Dos
);
AROS_LP2(LONG, DeleteVar,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(ULONG, flags, D2),
         LIBBASETYPEPTR, DOSBase, 152, Dos
);
AROS_LP2(struct LocalVar *, FindVar,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(ULONG, type, D2),
         LIBBASETYPEPTR, DOSBase, 153, Dos
);
AROS_LP1(STRPTR, DosGetLocalizedString,
         AROS_LPA(LONG, stringNum, D1),
         LIBBASETYPEPTR, DOSBase, 154, Dos
);
AROS_LP1(IPTR, CliInitNewcli,
         AROS_LPA(struct DosPacket *, dp, A0),
         LIBBASETYPEPTR, DOSBase, 155, Dos
);
AROS_LP1(IPTR, CliInitRun,
         AROS_LPA(struct DosPacket *, dp, A0),
         LIBBASETYPEPTR, DOSBase, 156, Dos
);
AROS_LP2(LONG, WriteChars,
         AROS_LPA(CONST_STRPTR, buf, D1),
         AROS_LPA(ULONG, buflen, D2),
         LIBBASETYPEPTR, DOSBase, 157, Dos
);
AROS_LP1(LONG, PutStr,
         AROS_LPA(CONST_STRPTR, string, D1),
         LIBBASETYPEPTR, DOSBase, 158, Dos
);
AROS_LP2(LONG, VPrintf,
         AROS_LPA(CONST_STRPTR, format, D1),
         AROS_LPA(IPTR *, argarray, D2),
         LIBBASETYPEPTR, DOSBase, 159, Dos
);
AROS_LP3(LONG, Pipe,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(BPTR *, reader, D2),
         AROS_LPA(BPTR *, writer, D3),
         LIBBASETYPEPTR, DOSBase, 160, Dos
);
AROS_LP3(LONG, ParsePatternNoCase,
         AROS_LPA(CONST_STRPTR, Source, D1),
         AROS_LPA(STRPTR, Dest, D2),
         AROS_LPA(LONG, DestLength, D3),
         LIBBASETYPEPTR, DOSBase, 161, Dos
);
AROS_LP2(BOOL, MatchPatternNoCase,
         AROS_LPA(CONST_STRPTR, pat, D1),
         AROS_LPA(CONST_STRPTR, str, D2),
         LIBBASETYPEPTR, DOSBase, 162, Dos
);
AROS_LP1(STRPTR, DosGetString,
         AROS_LPA(LONG, stringNu, D0),
         LIBBASETYPEPTR, DOSBase, 163, Dos
);
AROS_LP2(BOOL, SameDevice,
         AROS_LPA(BPTR, lock1, D1),
         AROS_LPA(BPTR, lock2, D2),
         LIBBASETYPEPTR, DOSBase, 164, Dos
);
AROS_LP5(void, ExAllEnd,
         AROS_LPA(BPTR, lock, D1),
         AROS_LPA(struct ExAllData *, buffer, D2),
         AROS_LPA(LONG, size, D3),
         AROS_LPA(LONG, data, D4),
         AROS_LPA(struct ExAllControl *, control, D5),
         LIBBASETYPEPTR, DOSBase, 165, Dos
);
AROS_LP2(BOOL, SetOwner,
         AROS_LPA(CONST_STRPTR, name, D1),
         AROS_LPA(ULONG, owner_info, D2),
         LIBBASETYPEPTR, DOSBase, 166, Dos
);
AROS_LP3(LONG, ScanVars,
         AROS_LPA(struct Hook *, hook, D1),
         AROS_LPA(ULONG, flags, D2),
         AROS_LPA(APTR, userdata, D3),
         LIBBASETYPEPTR, DOSBase, 167, Dos
);

#endif /* CLIB_DOS_PROTOS_H */
