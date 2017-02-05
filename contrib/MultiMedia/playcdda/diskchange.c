/* Copyright 2010-2011 Fredrik Wikstrom. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
*/

#include <exec/exec.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include "diskchange.h"
#include <SDI_compiler.h>

struct DiskChangeHandler {
	struct ExecBase *exec;
	struct Task *task;
	ULONG sigmask;
	struct IOStdReq *io;
	struct Interrupt *interrupt;
};

AROS_UFH3(void, DiskChangeFunction,
	AROS_UFHA(APTR, data, A1),
	AROS_UFHA(APTR, code, A5),
	AROS_UFHA(struct ExecBase *, SysBase, A6))
{
	AROS_USERFUNC_INIT
	struct DiskChangeHandler *handler = data;
	Signal(handler->task, handler->sigmask);
	AROS_USERFUNC_EXIT
}

struct IORequest *DuplicateIORequest (struct IORequest *old) {
	struct IORequest *new;
	new = CreateIORequest(old->io_Message.mn_ReplyPort, old->io_Message.mn_Length);
	if (new) {
		CopyMem(old, new, old->io_Message.mn_Length);
	}
	return new;
}

APTR AddDiskChangeHandler (struct IOStdReq *ref_io, ULONG sigmask) {
	struct DiskChangeHandler *handler;
	struct IOStdReq *io;
	struct Interrupt *interrupt;
	handler = AllocVec(sizeof(struct DiskChangeHandler), MEMF_PUBLIC);
	io = (struct IOStdReq *)DuplicateIORequest((struct IORequest *)ref_io);
	interrupt = AllocVec(sizeof(struct Interrupt), MEMF_PUBLIC|MEMF_CLEAR);
	if (handler && io && interrupt) {
		handler->exec = SysBase;
		handler->task = FindTask(NULL);
		handler->sigmask = sigmask;
		handler->io = io;
		handler->interrupt = interrupt;
		
		interrupt->is_Node.ln_Succ = NULL;
		interrupt->is_Node.ln_Pred = NULL;
		interrupt->is_Node.ln_Type = NT_INTERRUPT;
		interrupt->is_Node.ln_Pri = 0;
		interrupt->is_Node.ln_Name = "PlayCDDA disk change interrupt";
		interrupt->is_Data = handler;
		interrupt->is_Code = (APTR)DiskChangeFunction;
		
		io->io_Command = TD_ADDCHANGEINT;
		io->io_Data = interrupt;
		io->io_Length = sizeof(struct Interrupt);
		SendIO((struct IORequest *)io);
		
		return handler;
	} else {
		FreeVec(interrupt);
		DeleteIORequest((struct IORequest *)io);
		FreeVec(handler);
	}
	return NULL;
}

void RemDiskChangeHandler (APTR handler_ptr) {
	struct DiskChangeHandler *handler = handler_ptr;
	if (handler) {
		struct IOStdReq *io = handler->io;
		struct Interrupt *interrupt = handler->interrupt;
		
		io->io_Command = TD_REMCHANGEINT;
		DoIO((struct IORequest *)io);
		
		FreeVec(interrupt);
		DeleteIORequest((struct IORequest *)io);
		FreeVec(handler);
	}
}

