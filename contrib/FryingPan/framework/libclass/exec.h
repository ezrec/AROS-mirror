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

#ifndef _EXECIFACE_H_
#define _EXECIFACE_H_

#if defined (_PROTO_EXEC_H) || defined (PROTO_EXEC_H)
#error "Please remove proto/exec includes first!"
#endif

#define _PROTO_EXEC_H
#define PROTO_EXEC_H


#define MOS_DIRECT_OS
#include <GenericLib/Calls.h>
#undef MOS_DIRECT_OS
#include <exec/execbase.h>
#include <Generic/Types.h>

struct List;
struct Node;
struct MemList;
struct MemHeader;
struct Interrupt;
struct Interface;
struct Library;
struct Task;
struct MinList;
struct TagItem;
struct MsgPort;
struct Message;
struct IORequest;
struct SignalSemaphore;
struct SemaphoreMessage;
struct Device;
struct Resident;
struct AVLNode;
struct Trackable;
struct DMAEntry;
struct Hook;
struct StackSwapStruct;

typedef void* AVLKey;

   BEGINSHORTDECL(Exec)
#ifndef __AMIGAOS4__
      static ExecIFace* GetInstance(Library* libbase) 
      {
         if (libbase == 0)
            return 0;

         Exec = new ExecIFace;
         Exec->LibBase = libbase;
         return Exec;
      }

      void FreeInstance()
      {
         delete this;
      }
#else
      static ExecIFace* GetInstance(Library* libbase) 
      {
         Exec    = (struct ExecIFace*)((struct ExecBase*)libbase)->MainInterface;
         return Exec;
      }

      void FreeInstance()
      {
      }
#endif

      PROC2(                  AddHead, 40, List *, list, a0, Node *, node, a1);
      PROC1(                  AddMemHandler, 129, Interrupt *, memHandler, a1);
      PROC5(                  AddMemList, 103, uint32, size, d0, uint32, attributes, d1, int32, pri, d2, void *, base, a0, const char *, name, a1);
      PROC2(                  AddTail, 41, List *, list, a0, Node *, node, a1);
      FUNC2(void *,           AllocAbs, 34, uint32, byteSize, d0, void *, location, a1);
      FUNC2(void *,           Allocate, 31, MemHeader *, memHeader, a0, uint32, byteSize, d0);
      FUNC1(MemList *,        AllocEntry, 37, MemList *, memList, a0);
      FUNC2(void *,           AllocMem, 33, uint32, byteSize, d0, uint32, attributes, d1);
      FUNC2(void *,           AllocPooled, 118, void *, poolHeader, a0, uint32, memSize, d0);
      FUNC2(void *,           AllocVec, 114, uint32, byteSize, d0, uint32, attributes, d1);
      FUNC2(void *,           AllocVecPooled, 0, void *, poolHeader, a0, uint32, size, a1);
      FUNC1(uint32,           AvailMem, 36, uint32, attributes, d1);
      PROC3(                  CopyMem, 104, void *, source, a0, void *, dest, a1, uint32, size, d0);
      PROC3(                  CopyMemQuick, 105, uint32 *, source, a0, uint32 *, dest, a1, uint32, size, d0);
      FUNC3(void *,           CreatePool, 116, uint32, memFlags, d0, uint32, puddleSize, d1, uint32, threshSize, d2);
      PROC3(                  Deallocate, 32, MemHeader *, memHeader, a0, void *, memoryBlock, a1, uint32, byteSize, d0);
      PROC1(                  DeletePool, 117, void *, poolHeader, a0);
      PROC2(                  Enqueue, 45, List *, list, a0, Node *, node, a1);
      FUNC2(Node *,           FindName, 46, List *, start, a0, const char *, name, a1);
      FUNC2(Node *,           FindIName, 0, List *, start, a0, const char *, name, a1);
      PROC0(                  Forbid, 22);
      PROC1(                  FreeEntry, 38, MemList *, memList, a0);
      PROC2(                  FreeMem, 35, void *, memoryBlock, a1, uint32, byteSize, d0);
      PROC3(                  FreePooled, 119, void *, poolHeader, a0, void *, memory, a1, uint32, memSize, d0);
      PROC1(                  FreeVec, 115, void *, memoryBlock, a1);
      PROC2(                  FreeVecPooled, 0, void *, poolHeader, a0, void *, memory, a1);
      PROC3(                  InitData, 0, void *, initTab, a0, void *, memory, a1, uint32, size, a2);
      PROC3(                  InitStruct, 13, void *, initTable, a1, void *, memory, a2, uint32, size, d0);
      PROC3(                  Insert, 39, List *, list, a0, Node *, node, a1, Node *, listNode, a2);
      FUNC2(Interface *,      MakeInterface, 0, Library *, library, a0, const TagItem *, taglist, a1);
      PROC0(                  dummy1, 0);
      PROC0(                  Permit, 23);
      FUNC4(void *,           RawDoFmt, 87, const char *, formatString, a0, void *, dataStream, a1, void *, PutChProc, a2, void *, PutChData, a3);
      FUNC1(Node *,           RemHead, 43, List *, list, a0);
      PROC1(                  RemMemHandler, 130, Interrupt *, memHandler, a1);
      PROC1(                  Remove, 42, Node *, node, a1);
      FUNC1(Node *,           RemTail, 44, List *, list, a0);
      FUNC1(uint32,           TypeOfMem, 89, void *, address, a1);
      FUNC2(void *,           InitResident, 17, Resident *, resident, a1, uint32, segList, d1);
      PROC2(                  InitCode, 12, uint32, startClass, d0, uint32, version, d1);
      FUNC0(uint32,           SumKickData, 102);
      FUNC4(void *,           AddTask, 47, Task *, task, a1, void *, initialPC, a2, void *, finalPC, a3, const TagItem *, tagList, a4);
      PROC0(                  dummy2, 0);
      PROC0(                  Disable, 20);
      PROC0(                  Enable, 21);
      PROC0(                  dummy3, 0);
      FUNC1(Task *,           FindTask, 49, const char *, name, a1);
      PROC1(                  RemTask, 48, Task *, task, a1);
      FUNC2(int8,             SetTaskPri, 50, Task *, task, a1, int32, priority, d0);
      PROC1(                  StackSwap, 122, StackSwapStruct *, newstack, a0);
      FUNC1(int8,             AllocSignal, 55, int8, signalNum, d0);
      PROC1(                  FreeSignal, 56, int8, signalNum, d0);
      FUNC2(uint32,           SetExcept, 52, uint32, newSignals, d0, uint32, signalMask, d1);
      FUNC2(uint32,           SetSignal, 51, uint32, newSignals, d0, uint32, signalMask, d1);
      PROC2(                  Signal, 54, Task *, task, a1, uint32, signals, d0);
      FUNC1(uint32,           Wait, 53, uint32, signalSet, d0);
      PROC1(                  AddPort, 59, MsgPort *, port, a1);
      FUNC2(MsgPort *,        CreatePort, 111, const char *, name, a0, int8, pri, a1);
      FUNC0(MsgPort *,        CreateMsgPort, 111);
      PROC1(                  DeletePort, 112, MsgPort *, port, a0);
      PROC1(                  DeleteMsgPort, 112, MsgPort *, port, a0);
      FUNC1(MsgPort *,        FindPort, 65, const char *, name, a1);
      FUNC1(Message *,        GetMsg, 62, MsgPort *, port, a0);
      PROC2(                  PutMsg, 61, MsgPort *, port, a0, Message *, message, a1);
      PROC1(                  RemPort, 60, MsgPort *, port, a1);
      PROC1(                  ReplyMsg, 63, Message *, message, a1);
      FUNC1(Message *,        WaitPort, 64, MsgPort *, port, a0);
      PROC1(                  Cause, 30, Interrupt *, interrupt, a1);
      PROC1(                  AddSemaphore, 100, SignalSemaphore *, semaphore, a1);
      FUNC1(int32,            AttemptSemaphore, 96, SignalSemaphore *, semaphore, a0);
      FUNC1(int32,            AttemptSemaphoreShared, 120, SignalSemaphore *, semaphore, a0);
      FUNC1(SignalSemaphore *, FindSemaphore, 99, const char *, name, a1);
      PROC1(                  InitSemaphore, 93, SignalSemaphore *, semaphore, a0);
      PROC1(                  ObtainSemaphore, 94, SignalSemaphore *, semaphore, a0);
      PROC1(                  ObtainSemaphoreList, 97, List *, list, a0);
      PROC1(                  ObtainSemaphoreShared, 113, SignalSemaphore *, semaphore, a0);
      PROC2(                  Procure, 90, SignalSemaphore *, semaphore, a0, SemaphoreMessage *, bidMessage, a1);
      PROC1(                  ReleaseSemaphore, 95, SignalSemaphore *, semaphore, a0);
      PROC1(                  ReleaseSemaphoreList, 98, List *, list, a0);
      PROC1(                  RemSemaphore, 101, SignalSemaphore *, semaphore, a1);
      PROC2(                  Vacate, 91, SignalSemaphore *, semaphore, a0, SemaphoreMessage *, bidMessage, a1);
      FUNC5(Task *,           CreateTask, 0, const char *, name, a0, int32, pri, a1, void *, initPC, a2, uint32, stackSize, a3, const TagItem *, tagList, a4);
      PROC0(                  dummy4, 0);
      PROC1(                  DeleteTask, 0, Task *, task, a0);
      PROC1(                  SumLibrary, 71, Library *, library, a1);
      FUNC1(Library *,        CreateLibrary, 0, const TagItem *, taglist, a0);
      PROC0(                  dummy5, 0);
      FUNC2(Library *,        OpenLibrary, 92, const char *, name, a1, uint32, version, d0);
      FUNC1(void *,           CloseLibrary, 69, Library *, library, a1);
      PROC1(                  AddLibrary, 66, Library *, library, a1);
      FUNC1(uint32,           RemLibrary, 67, Library *, library, a1);
      PROC1(                  AddDevice, 72, Device *, device, a1);
      FUNC1(uint32,           RemDevice, 73, Device *, device, a1);
      FUNC4(Interface *,      GetInterface, 0, Library *, library, a0, const char *, name, a1, uint32, version, a2, const TagItem *, taglist, a3);
      PROC0(                  dummy6, 0);
      PROC1(                  DropInterface, 0, Interface *, interface, a0);
      PROC2(                  AddInterface, 0, Library *, library, a0, Interface *, interface, a1);
      PROC1(                  RemInterface, 0, Interface *, interface, a0);
      PROC1(                  SumInterface, 0, Interface *, interface, a0);
      FUNC1(Resident *,       FindResident, 16, const char *, name, a1);
      FUNC3(void *,           SetMethod, 0, Interface *, interface, a0, int32, funcOffset, a1, void *, newFunc, a2);
      PROC1(                  DeleteInterface, 0, Interface *, interface, a0);
      PROC1(                  DeleteLibrary, 0, Library *, library, a0);
      FUNC3(void *,           SetFunction, 70, Library *, library, a1, int32, funcOffset, a0, void *, newFunc, d0);
      PROC3(                  CacheClearE, 107, void *, address, a0, uint32, length, d0, uint32, caches, d1);
      PROC0(                  CacheClearU, 106);
      FUNC3(uint32,           MakeFunctions, 15, void *, target, a0, void *, functionArray, a1, void *, funcDispBase, a2);
      FUNC4(int32,            OpenDevice, 74, const char *, devName, a0, uint32, unitNumber, d0, IORequest *, ioRequest, a1, uint32, flags, d1);
      FUNC1(void *,           CloseDevice, 75, IORequest *, ioRequest, a1);
      FUNC2(IORequest *,      CreateIORequest, 109, MsgPort *, ioReplyPort, a0, uint32, size, d0);
      PROC1(                  DeleteIORequest, 110, IORequest *, ioRequest, a0);
      PROC1(                  AbortIO, 80, IORequest *, ioRequest, a1);
      FUNC1(IORequest *,      CheckIO, 78, IORequest *, ioRequest, a1);
      FUNC1(int8,             DoIO, 76, IORequest *, ioRequest, a1);
      PROC1(                  SendIO, 77, IORequest *, ioRequest, a1);
      PROC1(                  BeginIO, 0, IORequest *, ioRequest, a0);
      FUNC1(int8,             WaitIO, 79, IORequest *, ioRequest, a1);
      PROC1(                  AddResource, 81, void *, resource, a1);
      PROC1(                  RemResource, 82, void *, resource, a1);
      FUNC1(void *,           OpenResource, 83, const char *, resName, a1);
      FUNC2(bool,             AddIntServer, 28, uint32, intNum, d0, Interrupt *, interrupt, a1);
      PROC2(                  RemIntServer, 29, uint32, intNum, d0, Interrupt *, interrupt, a1);
      FUNC2(Interrupt *,      SetIntVector, 27, uint32, intNum, d0, Interrupt *, interrupt, a1);
      FUNC1(uint32,           ObtainQuickVector, 131, void *, interruptCode, a0);
      PROC1(                  Alert, 18, uint32, alertNum, d7);
      FUNC0(void *,           SuperState, 25);
      PROC1(                  UserState, 26, void *, sysStack, d0);
      FUNC1(uint32,           Supervisor, 5, void *, userFunc, d7);
      FUNC3(bool,             SetTaskTrap, 0, uint32, trapNum, a0, void *, trapCode, a1, void *, trapData, a2);
      FUNC1(int32,            AllocTrap, 57, uint32, trapNum, d0);
      PROC1(                  FreeTrap, 58, uint32, trapNum, d0);
      FUNC0(uint16,           GetCC, 88);
      FUNC2(uint32,           SetSR, 24, uint32, newSR, d0, uint32, mask, d1);
      FUNC3(AVLNode *,        AVL_AddNode, 142, AVLNode **, root, a0, AVLNode *, node, a1, void*, nodecompfunc, a2);
      FUNC1(AVLNode *,        AVL_FindFirstNode, 150, const AVLNode *, root, a0);
      FUNC1(AVLNode *,        AVL_FindLastNode, 151, const AVLNode *, root, a0);
      FUNC1(AVLNode *,        AVL_FindNextNodeByAddress, 148, const AVLNode *, root, a0);
      FUNC3(AVLNode *,        AVL_FindNextNodeByKey, 149, const AVLNode *, root, a0, AVLKey, key, a1, void*, keycompfunc, a2);
      FUNC3(AVLNode *,        AVL_FindNode, 145, const AVLNode *, root, a0, AVLKey, key, a1, void*, keycompfunc, a2);
      FUNC1(AVLNode *,        AVL_FindPrevNodeByAddress, 146, const AVLNode *, root, a0);
      FUNC3(AVLNode *,        AVL_FindPrevNodeByKey, 147, const AVLNode *, root, a0, AVLKey, key, a1, void*, keycompfunc, a2);
      FUNC2(AVLNode *,        AVL_RemNodeByAddress, 143, AVLNode **, root, a0, AVLNode *, node, a1);
      FUNC3(AVLNode *,        AVL_RemNodeByKey, 144, AVLNode **, root, a0, AVLKey, key, a1, void*, keycompfunc, a2);
      FUNC2(uint32,           CacheControl, 108, uint32, cacheBits, d0, uint32, cacheMask, d1);
      FUNC2(bool,             LockMem, 0, void *, baseAddress, a0, uint32, size, a1);
      PROC2(                  UnlockMem, 0, void *, baseAddress, a0, uint32, size, a1);
      FUNC3(uint32,           ReallocVec, 0, void *, memBlock, a0, uint32, newSize, a1, uint32, flags, a2);
      FUNC3(void *,           CachePreDMA, 127, void *, vaddr, a0, uint32 *, length, a1, uint32, flags, d0);
      PROC3(                  CachePostDMA, 128, void *, vaddr, a0, uint32 *, length, a1, uint32, flags, d0);
      FUNC3(uint32,           StartDMA, 0, void *, startAddr, a0, uint32, blockSize, a1, uint32, flags, a2);
      PROC3(                  EndDMA, 0, void *, startAddr, a0, uint32, blockSize, a1, uint32, flags, a2);
      PROC4(                  GetDMAList, 0, void *, startAddr, a0, uint32, blockSize, a1, uint32, flags, a2, DMAEntry *, dmaList, a3);
      FUNC3(Trackable *,      AddTrackable, 0, Task *, usingTask, a0, void *, object, a1, const Hook *, destFunc, a2);
      FUNC2(Trackable *,      FindTrackable, 0, Task *, usingTask, a0, void *, object, a1);
      FUNC2(Trackable *,      RemTrackable, 0, Task *, usingTask, a0, Trackable *, trackable, a1);
      PROC1(                  DeleteTrackable, 0, Trackable *, trackable, a0);
      FUNC2(void *,           AllocSysObject, 0, uint32, type, a0, const TagItem *, tags, a1);
      PROC0(                  dummy7, 0);
      PROC2(                  FreeSysObject, 0, uint32, type, a0, void *, object, a1);
      PROC2(                  SuspendTask, 0, Task *, whichTask, a0, uint32, flags, a1);
      PROC2(                  RestartTask, 0, Task *, whichTask, a0, uint32, flags, a1);
      PROC2(                  MoveList, 0, List *, destinationList, a0, List *, sourceList, a1);
      PROC1(                  NewList, 0, List *, list, a0);
      PROC1(                  NewMinList, 138, MinList *, list, a0);
      PROC0(                  ColdReboot, 121);
      FUNC5(Library *,        MakeLibrary, 14, void *, vectors, a0, void *, structure, a1, void *, init, a2, uint32, dataSize, d0, void *, segList, d1);
      FUNC2(uint32,           Emulate, 0, void *, InitPC, a0, const TagItem *, tagList, a1);
      PROC0(                  dummy8, 0);
      PROC0(                  dummy9, 0);
      FUNC1(bool,             IsNative, 0, void *, code, a0);
      PROC0(                  dummy10, 0);
      PROC0(                  dummy11, 0);
      PROC1(                  GetCPUInfo, 0, const TagItem *, TagList, a0);
      PROC0(                  dummy12, 0);
      FUNC1(Task *,           OwnerOfMem, 0, void *, Address, a0);
      FUNC1(bool,             AddResetCallback, 0, Interrupt *, resetCallback, a0);
      PROC1(                  RemResetCallback, 0, Interrupt *, resetCallback, a0);
      FUNC1(void *,           ItemPoolAlloc, 0, void *, itemPool, a0);
      PROC2(                  ItemPoolFree, 0, void *, itemPool, a0, void *, item, a1);
      PROC1(                  ItemPoolGC, 0, void *, itemPool, a0);
      FUNC2(uint32,           ItemPoolControl, 0, void *, itemPool, a0, const TagItem *, tagList, a1);
      PROC0(                  dummy13, 0);
// some morphos enhancements 
      FUNC4(void*,            NewSetFunction, 132, Library*, lib, a0, void*, call, a1, sint, offset, d0, const TagItem*, tags, a2);
   ENDDECL

#endif /*_EXECIFACE_H_*/
