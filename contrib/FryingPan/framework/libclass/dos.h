/*
 * Amiga Generic Set - set of libraries and includes to ease sw development for all Amiga platforms
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _DOSIFACE_H_
#define _DOSIFACE_H_

#if defined (_PROTO_DOS_H) || defined (PROTO_DOS_H)
#error "Please remove proto/dos includes first!"
#endif

#define _PROTO_DOS_H
#define PROTO_DOS_H

#include <Generic/Types.h>
#define MOS_DIRECT_OS
#include <GenericLib/Calls.h>
#undef MOS_DIRECT_OS

#include <libclass/exec.h>
#include <dos/dos.h>

extern class DOSIFace *DOS;

struct FileInfoBlock;
struct InfoData;
struct MsgPort;
struct TagItem;
struct DateStamp;
struct DosPacket;
struct RecordLock;
struct ExAllData;
struct ExAllControl;
struct CommandLineInterface;
struct Process;
struct DevProc;
struct DateTime;
struct Segment;
struct RDArgs;
struct CSource;
struct AnchorPath;
struct LocalVar;
struct NotifyRequest;
struct CapturedExpression;
struct Hook;
struct AddressAndSize;
struct PathNode;

   BEGINDECL(DOS, "dos.library")

   FUNC2(BPTR,             Open,             5, const char*,      name,    d1, int32,           access,  d2);
   FUNC1(int32,            Close,            6, BPTR,             file,    d1);
   FUNC3(int32,            Read,             7, BPTR,             file,    d1, void*,           buffer,  d2, int32,           length,  d3);
   FUNC3(int32,            Write,            8, BPTR,             file,    d1, void*,           buffer,  d2, int32,           length,  d3);
   FUNC0(BPTR,             Input,            9);
   FUNC0(BPTR,             Output,          10);
   FUNC3(int32,            Seek,            11, BPTR,             file,    d1, int32,           pos,     d2, int32,           offset,  d3);
   FUNC1(int32,            DeleteFile,      12, const char*,      name,    d1);
   FUNC2(int32,            Rename,          13, const char*,      oldname, d1, const char*,     newname, d2);
   FUNC2(BPTR,             Lock,            14, const char*,      name,    d1, int32,           type,    d2);
   PROC1(                  UnLock,          15, BPTR,             lock,    d1);
   FUNC1(BPTR,             DupLock,         16, BPTR,             lock,    d1);
   FUNC2(int32,            Examine,         17, BPTR,             lock,    d1, FileInfoBlock*,  fib,     d2);
   FUNC2(int32,            ExNext,          18, BPTR,             lock,    d1, FileInfoBlock*,  fib,     d2);
   FUNC2(int32,            Info,            19, BPTR,             lock,    d1, InfoData*,       info,    d2);
   FUNC1(BPTR,             CreateDir,       20, const char*,      name,    d1);
   FUNC1(BPTR,             CurrentDir,      21, BPTR,             lock,    d1);
   FUNC0(int32,            IoErr,           22);
   FUNC4(MsgPort*,         CreateProc,      23, const char*,      name,    d1, int32,           pri,     d2, BPTR,            seglist, d3, int32,           stksize, d4);
   PROC1(                  Exit,            24, int32,            retcode, d1);
   FUNC1(BPTR,             LoadSeg,         25, const char*,      name,    d1);
   FUNC1(int32,            UnLoadSeg,       26, BPTR,             seglist, d1);
#ifdef __AMIGAOS4__
   PROC0(                  dummy1,          27);
   FUNC1(BPTR,             NoReqLoadSeg,    28, const char*,      name,    d1);
#endif
   FUNC1(MsgPort*,         DeviceProc,      29, const char*,      name,    d1);
   FUNC2(int32,            SetCommend,      30, const char*,      name,    d1, const char*,     comment, d2);
   FUNC2(int32,            SetProtection,   31, const char*,      name,    d1, int32,           protect, d2);
   FUNC1(struct DateStamp*, DateStamp,      32, struct DateStamp*, date,   d1);
   PROC1(                  Delay,           33, int32,            timeout, d1);
   FUNC2(int32,            WaitForChar,     34, BPTR,             file,    d1, int32,           timeout, d2);
   FUNC1(BPTR,             ParentDir,       35, BPTR,             lock,    d1);
   FUNC1(int32,            IsInteractive,   36, BPTR,             file,    d1);
   FUNC3(int32,            Execute,         37, const char*,      command, d1, BPTR,            input,   d2, BPTR,            output,  d3);
   FUNC2(void*,            AllocDosObject,  38, uint32,           type,    d1, const TagItem*,  tags,    d2);
#ifdef __AMIGAOS4__
   PROC0(                  dummy2,           0);
   PROC0(                  dummy3,           0);
#endif
   PROC2(                  FreeDosObject,   39, uint32,           type,    d1, void*,           ptr,     d2);

   FUNC7(int32,            DoPkt,           40, MsgPort*, port, d1, int32, action, d2, int32, arg1, d3, int32, arg2, d4, int32, arg3, d5, int32, arg4, d6, int32, arg5, d7);
   FUNC2(int32,            DoPkt0,          40, MsgPort*, port, d1, int32, action, d2);
   FUNC3(int32,            DoPkt1,          40, MsgPort*, port, d1, int32, action, d2, int32, arg1, d3);
   FUNC4(int32,            DoPkt2,          40, MsgPort*, port, d1, int32, action, d2, int32, arg1, d3, int32, arg2, d4);
   FUNC5(int32,            DoPkt3,          40, MsgPort*, port, d1, int32, action, d2, int32, arg1, d3, int32, arg2, d4, int32, arg3, d5);
   FUNC6(int32,            DoPkt4,          40, MsgPort*, port, d1, int32, action, d2, int32, arg1, d3, int32, arg2, d4, int32, arg3, d5, int32, arg4, d6);

   PROC3(                  SendPkt,         41, DosPacket*,       dp,      d1, MsgPort*,        port,    d2, MsgPort*,        repport, d3);
   FUNC1(DosPacket*,       WaitPkt,         42, MsgPort*,         port,    d1);
   FUNC3(int32,            ReplyPkt,        43, DosPacket*,       packet,  d1, int32,           res1,    d2, int32,           res2,    d3);
   PROC0(                  dummy4,          44);   // abort pkt! we don't want that.
   FUNC5(int32,            LockRecord,      45, BPTR, file, d1, uint32, offset, d2, uint32, length, d3, uint32, mode, d4, uint32, timeout, d5);
   FUNC2(int32,            LockRecords,     46, RecordLock*,      array,   d1, uint32,          timeout, d2);
   FUNC3(int32,            UnLockRecord,    47, BPTR,             file,    d1, uint32,          offset,  d2, uint32,          length,  d3);
   FUNC1(int32,            UnLockRecords,   48, RecordLock*,      array,   d1);
   FUNC1(BPTR,             SelectInput,     49, BPTR,             input,   d1);
   FUNC1(BPTR,             SelectOutput,    50, BPTR,             output,  d1);
   FUNC1(int32,            FGetC,           51, BPTR,             file,    d1);
   FUNC2(int32,            FPutC,           52, BPTR,             file,    d1, int32,           chrctr,  d2);
   FUNC2(int32,            UnGetC,          53, BPTR,             file,    d1, int32,           chrctr,  d2);
   FUNC4(int32,            FRead,           54, BPTR, fh, d1, void*, block, d2, uint32, blklen, d3, uint32, blkcnt, d4);
   FUNC4(int32,            FWrite,          55, BPTR, fh, d1, void*, block, d2, uint32, blklen, d3, uint32, blkcnt, d4);
   FUNC3(char*,            FGets,           56, BPTR,             file,    d1, char*,           buffer,  d2, uint32,          buflen,  d3);
   FUNC2(int32,            FPuts,           57, BPTR,             file,    d1, const char*,     string,  d2);
   
   PROC0(                  dummy5,          58);   // vfwritef
   PROC0(                  dummy6,          58);   // os4 dummy function for fwritef 
 
   FUNC3(int32,            VFPrintf,        59, BPTR,             file,    d1, const char*,     string,  d2, void*,           array,   d3);
   PROC0(                  dummy7,          59);   // os4 dummy for FPrintf
   FUNC1(int32,            Flush,           60, BPTR,             file,    d1);

   FUNC4(int32,            SetVBuf,         61, BPTR,             file,    d1, void*,           buff,    d2, int32,           type,    d3, int32,              size,    d4);
   FUNC1(BPTR,             DupLockFromFH,   62, BPTR,             file,    d1);
   FUNC1(BPTR,             OpenFromLock,    63, BPTR,             lock,    d1);
   FUNC1(BPTR,             ParentOfFH,      64, BPTR,             file,    d1);
   FUNC2(int32,            ExamineFH,       65, BPTR,             file,    d1, FileInfoBlock*,  fib,     d2);
   FUNC2(int32,            SetFileDate,     66, const char*,      name,    d1, struct DateStamp*, date,  d2);
   FUNC3(int32,            NameFromLock,    67, BPTR,             lock,    d1, char*,           buffer,  d2, int32,           length,  d3);
   FUNC3(int32,            NameFromFH,      68, BPTR,             file,    d1, char*,           buffer,  d2, int32,           length,  d3);
   FUNC5(int32,            SplitName,       69, const char*, name, d1, char, separator, d2, char*, buffer, d3, int32, oldpos, d4, int32, size, d5);
   FUNC2(int32,            SameLock,        70, BPTR,             lock1,   d1, BPTR,            lock2,   d2);
   FUNC2(int32,            SetMode,         71, BPTR,             fh,      d1, int32,           mode,    d2);
   FUNC5(int32,            ExAll,           72, BPTR, lock, d1, ExAllData*, buffer, d2, int32, size, d3, int32, data, d4, ExAllControl*, control, d5);
   FUNC5(int32,            ReadLink,        73, MsgPort*, port, d1, BPTR, lock, d2, const char*, path, d3, char*, buffer, d4, int32, buflen, d5);
   FUNC3(int32,            MakeLink,        74, const char*,      srcpath, d1, const char*,     dstpath, d2, int32,           soft,    d3);
   FUNC3(int32,            ChangeMode,      75, int32,            type,    d1, BPTR,            file,    d2, int32,           newmode, d3);
   FUNC3(int32,            SetFileSize,     76, BPTR,             file,    d1, int32,           pos,     d2, int32,           mode,    d3);
   FUNC1(int32,            SetIoErr,        77, int32,            result,  d1);
   FUNC4(int32,            Fault,           78, int32,            code,    d1, const char*,     header,  d2, char*,           buffer,  d3, int32,              length,  d4);
   FUNC2(int32,            PrintFault,      79, int32,            code,    d1, const char*,     header,  d2);
   FUNC4(int32,            ErrorReport,     80, int32,            code,    d1, int32,           type,    d2, uint32,          arg1,    d3, MsgPort*,           device,  d4);

   PROC0(                  dummy8,          81);

   FUNC0(CommandLineInterface*, Cli,        82);
   FUNC1(Process*,         CreateNewProc,   83, const TagItem*,   tags,    d1);
   FUNC1(Process*,         CreateNewProcTagList, 83, const TagItem*,tags,    d1);
   PROC0(                  dummy9,          0);    // os4 create new proc tags

   FUNC4(int32,            RunCommand,      84, BPTR,             seglist, d1, uint32,          stack,   d2, const char*,     params,  d3, int32,              parmlen, d4);
   FUNC0(MsgPort*,         GetConsoleTask,  85);
   FUNC1(MsgPort*,         SetConsoleTask,  86, MsgPort*,         port,    d1);
   FUNC0(MsgPort*,         GetFileSysTask,  87);
   FUNC1(MsgPort*,         SetFileSysTask,  88, MsgPort*,         port,    d1);
   FUNC0(const char*,      GetArgStr,       89);
   FUNC1(const char*,      SetArgStr,       90, const char*,      string,  d1);
   FUNC1(Process*,         FindCliProc,     91, uint32,           number,  d1);
   FUNC0(uint32,           MaxCli,          92);

   FUNC1(int32,            SetCurrentDirName, 93, const char*,    name,    d1);
   FUNC2(int32,            GetCurrentDirName, 94, char*,          buffer,  d1, uint32,          buflen,  d2);
   FUNC1(int32,            SetProgramName,  95, const char*,      name,    d1);
   FUNC2(int32,            GetProgramName,  96, char*,            buffer,  d1, uint32,          buflen,  d2);
   FUNC1(int32,            SetPrompt,       97, const char*,      prompt,  d1);
   FUNC2(int32,            GetPrompt,       98, char*,            buffer,  d1, uint32,          buflen,  d2);
   FUNC1(BPTR,             SetProgramDir,   99, BPTR,             lock,    d1);
   FUNC0(BPTR,             GetProgramDir,  100);

   FUNC2(int32,            SystemTagList,  101, const char*,      command, d1, const TagItem*,  tags,    d2);
   FUNC2(int32,            System,         101, const char*,      command, d1, const TagItem*,  tags,    d2);
   PROC0(                  dummy10,          0);

   FUNC2(int32,            AssignLock,     102, const char*,      name,    d1, BPTR,            lock,    d2);
   FUNC2(int32,            AssignLate,     103, const char*,      name,    d1, const char*,     path,    d2);
   FUNC2(int32,            AssignPath,     104, const char*,      name,    d1, const char*,     path,    d2);
   FUNC2(int32,            AssignAdd,      105, const char*,      name,    d1, BPTR,            lock,    d2);
   FUNC2(int32,            RemAssignList,  106, const char*,      name,    d1, BPTR,            lock,    d2);

   FUNC2(DevProc*,         GetDeviceProc,  107, const char*,      name,    d1, DevProc*,        dp,      d2);
   PROC1(                  FreeDeviceProc, 108, DevProc*,         dp,      d1);

   FUNC1(struct DosList*,  LockDosList,    109, uint32,           flags,   d1);
   PROC1(                  UnLockDosList,  110, uint32,           flags,   d1);
   FUNC1(DosList*,         AttemptLockDosList, 111, uint32,       flags,   d1);
   FUNC1(int32,            RemDosEntry,    112, DosList*,         dlist,   d1);
   FUNC1(int32,            AddDosEntry,    113, DosList*,         dlist,   d1);
   FUNC3(DosList*,         FindDosEntry,   114, DosList*,         dlist,   d1, const char*,     name,    d2, uint32,          flags,   d3);
   FUNC2(DosList*,         NextDosEntry,   115, DosList*,         dlist,   d1, uint32,          flags,   d2);
   FUNC2(DosList*,         MakeDosEntry,   116, const char*,      name,    d1, int32,           type,    d2);
   PROC1(                  FreeDosEntry,   117, DosList*,         dlist,   d1);

   FUNC1(int32,            IsFileSystem,   118, const char*,      name,    d1);
   FUNC3(int32,            Format,         119, const char*,      filesys, d1, const char*,     name,    d2, uint32,          type,    d3);
   FUNC2(int32,            Relabel,        120, const char*,      drive,   d1, const char*,     newname, d2);
   FUNC2(int32,            Inhibit,        121, const char*,      name,    d1, int32,           onoff,   d2);
   FUNC2(int32,            AddBuffers,     122, const char*,      name,    d1, int32,           number,  d2);
   
   FUNC2(int32,            CompareDates,   123, const struct DateStamp*, date1,   d1, const struct DateStamp*,date2,   d2);
   FUNC1(int32,            DateToStr,      124, DateTime*,        datetme, d1);
   FUNC1(int32,            StrToDate,      125, DateTime*,        datetme, d1);

   PROC0(                  dummy11,        126);      //internal load seg
   PROC0(                  dummy12,        127);      //internal unload seg
   PROC0(                  dummy13,          0);      // os4 something
   PROC0(                  dummy14,          0);      // os4 something
   PROC0(                  dummy15,          0);      // os4 something

   FUNC3(int32,            AddSegment,     129, const char*,      name,    d1, BPTR,            segment, d2, int32,           type,    d3);
   FUNC3(Segment*,         FindSegment,    130, const char*,      name,    d1, Segment*,        segment, d2, int32,           sys,     d3);
   FUNC1(int32,            RemSegment,     131, Segment*,         segment, d1);
   FUNC1(int32,            CheckSignal,    132, int32,            mask,    d1);
   FUNC3(RDArgs*,          ReadArgs,       133, const char*,      tmplate, d1, IPTR *,          array,   d2, RDArgs*,         args,    d3);
   FUNC2(int32,            FindArg,        134, const char*,      tmplate, d1, const char*,     keyword, d2);
   FUNC3(int32,            ReadItem,       135, const char*,      name,    d1, int32,           maxchrs, d2, CSource*,        csource, d3);
   FUNC2(int32,            StrToLong,      136, const char*,      string,  d1, int32*,          value,   d2);

   FUNC2(int32,            MatchFirst,     137, const char*,      pattern, d1, AnchorPath*,     anchor,  d2);
   FUNC1(int32,            MatchNext,      138, AnchorPath*,      anchor,  d1);
   PROC1(                  MatchEnd,       139, AnchorPath*,      anchor,  d1);
   FUNC3(int32,            ParsePattern,   140, const char*,      pattern, d1, char*,           buffer,  d2, int32,           length,  d3);
   FUNC2(int32,            MatchPattern,   141, const char*,      pattern, d1, const char*,     string,  d2);

   PROC0(                  dummy16,        142);
   PROC1(                  FreeArgs,       143, RDArgs*,          args,    d1);
   PROC0(                  dummy17,        144);
   FUNC1(const char*,      FilePart,       145, const char*,      path,    d1);
   FUNC1(const char*,      PathPart,       146, const char*,      path,    d1);
   FUNC3(int32,            AddPart,        147, char*,            path,    d1, const char*,     file,    d2, int32,           buflen,  d3);
   FUNC1(int32,            StartNotify,    148, NotifyRequest*,   notify,  d1);
   FUNC1(int32,            EndNotify,      149, NotifyRequest*,   notify,  d1);
   FUNC4(int32,            SetVar,         150, const char*,      name,    d1, const char*,     buffer,  d2, int32,           size,    d3, uint32,             flags,   d4);
   FUNC4(int32,            GetVar,         151, const char*,      name,    d1, char*,           buffer,  d2, int32,           size,    d3, uint32,             flags,   d4);
   FUNC2(int32,            DeleteVar,      152, const char*,      name,    d1, uint32,          flags,   d2);
   FUNC2(LocalVar*,        FindVar,        153, const char*,      name,    d1, uint32,          type,    d2);

   PROC0(                  dummy18,        154);

   FUNC1(int32,            CliInitNewcli,  155, DosPacket*,       dp,      d1);
   FUNC1(int32,            CliInitRun,     156, DosPacket*,       dp,      d1);
   FUNC2(int32,            WriteChars,     157, const char*,      buffer,  d1, uint32,          buflen,  d2);
   FUNC1(int32,            PutStr,         158, const char*,      buffer,  d1);
   FUNC2(int32,            VPrintf,        159, const char*,      format,  d1, IPTR,           argarray,d2);

   PROC0(                  dummy19,          0);      // amigaos4 Printf function
   PROC0(                  dummy20,        160);      // reserved call

   FUNC3(int32,            ParsePatternNoCase, 161, const char*,  pattern, d1, char*,           buffer,  d2, int32,           buflen,  d3);
   FUNC2(int32,            MatchPatternNoCase, 162, const char*,  pattern, d1, const char*,     string,  d2);

   PROC0(                  dummy21,        163);

   FUNC2(int32,            SameDevice,     164, BPTR,             lock1,   d1, BPTR,            lock2,   d2);
   PROC5(                  ExAllEnd,       165, BPTR,             lock,    d1, ExAllData*,      ead,     d2, int32,           eadsize, d3, int32,              data,    d4, ExAllControl*,   eac,     d5);
   FUNC2(int32,            SetOwner,       166, const char*,      name,    d1, int32,           ownernfo,d2);

   PROC0(                  dummy22,        167);
   PROC0(                  dummy23,        168);
   PROC0(                  dummy24,        169);
   PROC0(                  dummy25,        170);

// OS4 SPECIFIC

#ifdef __AMIGAOS4__
   FUNC0(BPTR,             GetCurrentDir,  171);
   FUNC4(int32,            ReadItemZ,      172, const char*,      name,    d1, int32,           maxchrs, d2, void*,           input,   d3, uint32,             flags,   d4);
   FUNC2(struct DateStamp*,SecondsToDateStamp, 173, uint32,       seconds, d1, struct DateStamp*,ds,      d2);
   FUNC1(uint32,           DateStampToSeconds, 174, struct DateStamp*, ds, d1);
   FUNC1(int32,            FixDateStamp,   175, struct DateStamp*,ds,      d1);
   FUNC2(int32,            AddDates,       176, struct DateStamp*,dest,    d1, const struct DateStamp*, src,    d2);
   FUNC2(int32,            SubtractDates,  177, struct DateStamp*,dest,    d1, const struct DateStamp*, src,    d2);
   FUNC3(int32,            AddSegmentTagList, 178, const char*,   name,    d1, int32,           type,    d2, const TagItem*,  tags,    d3);
   FUNC4(int32,            ParseCapturePattern, 179, const char*, pattern, d1, char*,           dest,    d2, int32,           length,  d3, int32,              casesen, d4);
   FUNC4(int32,            CapturePattern, 180, const char*,      pattern, d1, char*,           string,  d2, int32,           casesen, d3, CapturedExpression**, cap,   d4);

   PROC1(                  ReleaseCapturedExpression, 181, struct CaptureExpression*, first, d1);
   PROC2(                  FindTrackedAddress, 182, void*, address, d1, const Hook*, hook, d2);
   FUNC6(int32,            TrackAddressList, 183, const char*, name, d1, BPTR, segment, d2, void*, extinfo, d3, int32, extsize, d4, AddressAndSize*, aaa, d5, int32, numpairs, d6);
   FUNC4(int32,            TrackSegmentList, 184, const char*, name, d1, BPTR, segment, d2, void*, extinfo, d3, int32, extsize, d4);
   PROC1(                  UnTrackAddress, 185, void*, address, d1);
   PROC1(                  UnTrackSegmentList, 186, BPTR, seglist, d1);

   FUNC0(int32,            GetExitData,    187);
   FUNC1(int32,            PutErrStr,      188, const char*,      string,  d1);
   FUNC0(BPTR,             ErrorOutput,    189);
   FUNC1(BPTR,             SelectErrorOutput, 190, BPTR,          output,  d1);

   FUNC3(int32,            MountDevice,    191, const char*,      name,    d1, int32,           type,    d2, const TagItem*,  tags,    d3);
   PROC0(                  dummy26,          0);   // mount device tags os4
   FUNC1(void*,            SetProcWindow,  192, void*,            window,  d1);

   FUNC1(uint32,           FindSegmentStackSize, 193, BPTR, segment, d1);
   FUNC1(uint32,           CalculateSegmentChecksum, 194, BPTR, segment, d1);
   FUNC4(BPTR,             AllocSegList,   195, void*,            entry,   d1, void*,           data,    d2, uint32,          datalen, d3, uint32,             idkey,   d4);
   FUNC2(int32,            GetSegListInfo, 196, BPTR,             seglist, d1, const TagItem*,  tags,    d2);
   PROC0(                  dummy27,          0);   // get seg list info tags os4
   FUNC2(int32,            AddSegListTail, 197, BPTR,             seghead, d1, BPTR,            segnew,  d2);
   FUNC3(int32,            DevNameFromLock,198, BPTR,             lock,    d1, char*,           buffer,  d2, int32,           buflen,  d3);
   FUNC1(MsgPort*,         GetProcMsgPort, 199, Process*,         proc,    d1);
   FUNC3(int32,            WaitForData,    200, BPTR,             stream,  d1, int32,           datadir, d2, int32,           timeout, d3);
   FUNC2(int32,            SetBlockingMode,201, BPTR,             stream,  d1, int32,           newmode, d2);
   FUNC1(PathNode*,        SetCurrentCmdPathList, 202, PathNode*, newlist, d1);
   FUNC1(PathNode*,        AllocateCmdPathList, 203, BPTR,        frstlck, d1);
   PROC1(                  FreeCmdPathList,204, PathNode*,        node,    d1);
   FUNC2(PathNode*,        RemoveCmdPathNode, 205, PathNode*,     node,    d1, BPTR,            lock,    d2);
   FUNC3(PathNode*,        AddCmdPathNode, 206, PathNode*,        node,    d1, BPTR,            lock,    d2, int32,           where,   d3);
   FUNC4(int32,            SearchCmdPathList, 207, PathNode*,     node,    d1, const Hook*,     hook,    d2, const char*,     name,    d3, const TagItem*,     tags,    d4);
   PROC0(                  dummy28,          0);   // search cmd path list tags os4
   FUNC3(int32,            ScanVars,       208, const Hook*,      hook,    d1, uint32,          flags,   d2, void*,           usrdata, d3);

   FUNC1(BPTR,             GetProcSegList, 209, Process*,         proc,    d1);
   FUNC2(int32,            HexToLong,      210, const char*,      string,  d1, uint32*,         valptr,  d2);
   FUNC3(DevProc*,         GetDeviceProcFlags, 211, const char*,  name,    d1, DevProc*,        odp,     d2, uint32,          flags,   d3);
   FUNC1(uint32,           DosControl,     212, const TagItem*,   tags,    d1);
   PROC0(                  dummy29,          0);   // dos control tags os4
   FUNC1(BPTR,             CreateDirTree,  213, const char*,      tree,    d1);
   FUNC4(int32,            NotifyVar,      214, const char*,      name,    d1, const Hook*,     hook,    d2, uint32,          flags,   d3, void*,              usrdata, d4);

#endif /* __AMIGAOS4__ */
   
   ENDDECL

#endif /* DOS_INTERFACE_DEF_H */
