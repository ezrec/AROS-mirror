Const

    { DGS -- AROS function vectors }

       LIBVECTSIZE = 4;
  
       _LVOExec_FindTask         = -(49 * LIBVECTSIZE);
       _LVOExec_WaitPort         = -(64 * LIBVECTSIZE);
       _LVOExec_GetMsg           = -(62 * LIBVECTSIZE);
       _LVOExec_OpenLibrary      = -(92 * LIBVECTSIZE);
       _LVOExec_CloseLibrary     = -(69 * LIBVECTSIZE);
       _LVOExec_SetSignal        = -(51 * LIBVECTSIZE);
       _LVOExec_AllocVec         = -(114 * LIBVECTSIZE);
       _LVOExec_FreeVec          = -(115 * LIBVECTSIZE);

       _LVODos_Close             = -(6 * LIBVECTSIZE);
       _LVODos_Open              = -(5 * LIBVECTSIZE);
       _LVODos_DateStamp         = -(32 * LIBVECTSIZE);
       _LVODos_IoErr             = -(22 * LIBVECTSIZE);
       _LVODos_Seek              = -(11 * LIBVECTSIZE);
       _LVODos_DeleteFile        = -(12 * LIBVECTSIZE);
       _LVODos_Rename            = -(13 * LIBVECTSIZE);
       _LVODos_Write             = -(8 * LIBVECTSIZE);
       _LVODos_Read              = -(7 * LIBVECTSIZE);
       _LVODos_CreateDir         = -(20 * LIBVECTSIZE);
       _LVODos_SetCurrentDirName = -(93 * LIBVECTSIZE);
       _LVODos_GetCurrentDirName = -(94 * LIBVECTSIZE);
       _LVODos_Input             = -(9 * LIBVECTSIZE);
       _LVODos_Output            = -(10 * LIBVECTSIZE);
       _LVODos_UnLock            = -(15 * LIBVECTSIZE);
       _LVODos_Lock              = -(14 * LIBVECTSIZE);
       _LVODos_CurrentDir        = -(21 * LIBVECTSIZE);
       _LVODos_NameFromLock      = -(67 * LIBVECTSIZE);
       _LVODos_NameFromFH        = -(68 * LIBVECTSIZE);
       _LVODos_GetProgramName    = -(96 * LIBVECTSIZE);
       _LVODos_GetProgramDir     = -(100 * LIBVECTSIZE);
       _LVODos_DupLock           = -(16 * LIBVECTSIZE);
       _LVODos_Examine           = -(17 * LIBVECTSIZE);
       _LVODos_ParentDir         = -(35 * LIBVECTSIZE);
       _LVODos_SetFileSize       = -(76 * LIBVECTSIZE);


{ ************************ AROS STUB ROUTINES ************************* }

Function DosDateStamp(var ds : tDateStamp):Longint; Assembler;
Asm
  movl  _DOSBase,%eax
  pushl %eax
  pushl ds
  addl  $_LVODos_DateStamp,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $8,%esp
End ['eax'];


Function DosCurrentDir(alock: Longint):Longint; Assembler;
Asm
  movl  _DOSBase,%eax
  pushl %eax
  pushl alock
  addl  $_LVODos_CurrentDir,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $8,%esp
End ['eax'];

Function DosDupLock(alock: Longint):Longint; Assembler;
Asm
  movl  _DOSBase,%eax
  pushl %eax
  pushl alock
  addl  $_LVODos_DupLock,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $8,%esp
End ['eax'];

Function DosGetProgramDir:Longint; Assembler;
Asm
  movl  _DOSBase,%eax
  pushl %eax
  addl  $_LVODos_GetProgramDir,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $4,%esp
End ['eax'];

Function DosExamine(alock:Longint; var fib:TFileInfoBlock):Longint; Assembler;
Asm
  movl  _DOSBase,%eax
  pushl %eax
  pushl fib
  pushl alock
  addl  $_LVODos_Examine,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $12,%esp
End ['eax'];

Function DosParentDir(alock: Longint):Longint; Assembler;
Asm
  movl  _DOSBase,%eax
  pushl %eax
  pushl alock
  addl  $_LVODos_ParentDir,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $8,%esp
End ['eax'];

Function DosNameFromLock(alock: Longint; buffer: pchar; len: Longint):Longint; Assembler;
Asm
  movl  _DOSBase,%eax
  pushl %eax
  pushl len
  pushl buffer
  pushl alock
  addl  $_LVODos_NameFromLock,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $16,%esp
End ['eax'];

Function DosInput:Longint; Assembler;
Asm
  movl  _DOSBase,%eax
  pushl %eax
  addl  $_LVODos_Input,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $4,%esp
End ['eax'];

Function DosOutput:Longint; Assembler;
Asm
  movl  _DOSBase,%eax
  pushl %eax
  addl  $_LVODos_Output,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $4,%esp
End ['eax'];

{$S-}
{$ifdef i386}
  Procedure stack_check; assembler;
  asm
{AROS VERSION HERE}
  end;
{$endif i386}

{$ifdef m68k}
    Procedure stack_check; assembler;
    { Check for local variable allocation }
    { On Entry -> d0 : size of local stack we are trying to allocate }
     asm
      XDEF STACKCHECK
        move.l  sp,d1            { get value of stack pointer            }

        { We must add some security, because Writing the RunError strings }
        { requires a LOT of stack space (at least 1030 bytes!)            }
        add.l   #2048,d0
        sub.l   d0,d1            {  sp - stack_size                      }

        move.l  _ExecBase,a0
        move.l  276(A0),A0       { ExecBase.thisTask }
        { if allocated stack_pointer - splower <= 0 then stack_ovf       }
        cmp.l   58(A0),D1        { Task.SpLower      }
        bgt     @Ok
        move.l  #202,d0
        jsr     HALT_ERROR       { stack overflow    }
    @Ok:
   end;
{$endif m68k}

Function ExecFindTask(p: pchar):Longint; Assembler;
Asm
  movl  _ExecBase,%eax
  pushl %eax
  pushl p
  addl  $_LVOExec_FindTask,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $8,%esp
End ['eax'];


Function ExecSetSignal(newSignal, SignalMask: Longint):Longint; Assembler;
Asm
  movl  _ExecBase,%eax
  pushl %eax
  pushl SignalMask
  pushl newSignal
  addl  $_LVOExec_SetSignal,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $12,%esp
End ['eax'];

Function ExecAllocVec(size, attrib:Longint):Longint; Assembler;
Asm
  movl  _ExecBase,%eax
  pushl %eax
  pushl attrib
  pushl size
  addl  $_LVOExec_AllocVec,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $12,%esp
End ['eax'];

Function ExecFreeVec(vec: Longint):Longint; Assembler;
Asm
  movl  _ExecBase,%eax
  pushl %eax
  pushl vec
  addl  $_LVOExec_FreeVec,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $8,%esp
End ['eax'];

Function ExecCloseLibrary(lib: pointer):Longint; Assembler;
Asm
  movl  _ExecBase,%eax
  pushl %eax
  pushl lib
  addl  $_LVOExec_CloseLibrary,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $8,%esp
End ['eax'];

Function ExecOpenLibrary(libname: pchar; libver: Longint):Longint; Assembler;
Asm
  movl  _ExecBase,%eax
  pushl %eax
  pushl libver
  pushl libname
  addl  $_LVOExec_OpenLibrary,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $12,%esp
End ['eax'];

Function KickVersion: word; assembler;
Asm
  movl  _ExecBase,%eax
  xorl  %ebx,%ebx
  movw  20(%eax),%bx       { Return version - version at this offset }
  movl  %ebx,%eax
end ['eax', 'ebx'];


{*****************************************************************************
                         System Dependent Exit code
*****************************************************************************}

Procedure _Halt(errnum : byte);
Begin
  { Indicate to the SYSTEM EXIT procedure that we are calling it }
  { from halt, and that its library will be closed HERE and not  }
  { in the exit procedure.                                       }
  FromHalt := TRUE;

  { We must remove the CTRL-C FLAG here because halt }
  { may call I/O routines, which in turn might call  }
  { halt, so a recursive stack crash                 }
  If (ExecSetSignal(0,0) And SIGBREAKF_CTRL_C) <> 0 Then Begin
    ExecSetSignal(0,SIGBREAKF_CTRL_C);
  End;

  { WE can only FLUSH the stdio   }
  { if the handles have correctly }
  { been set.                     }
  { No exit procedures exist      }
  { if in initial state           }
  If NOT Initial Then Begin
    do_exit;
    flush(stderr);
  End;

  { close the libraries }
  If _UtilityBase <> nil then begin
    ExecCloseLibrary(_UtilityBase);
    _UtilityBase := nil;
  end;

  If _DosBase <> nil then begin
    ExecCloseLibrary(_DosBase);
    _DosBase := nil;
  end;

  If _IntuitionBase <> nil then begin
    ExecCloseLibrary(_IntuitionBase);
    _IntuitionBase := nil;
  end;

  Asm
    xorl  %eax,%eax    { clear register }
    movb  errnum,%al
    movl  STKPTR,%esp  { restore stack to program entry state }
    ret                { and back to operating system }
  End;
  { This point is never reached }
End;

  { ************************ PARAMCOUNT/PARAMSTR *************************** }

Function args : pointer; assembler;
Asm
  movl __ARGS,%eax
End;

Function getheapstart:pointer;assembler;
Asm
  leal HEAP,%eax
End ['eax'];

Function getheapsize:longint;assembler;
Asm
  movl HEAP_SIZE,%eax
End ['eax'];

{****************************************************************************
                          Low Level File Routines
 ****************************************************************************}

Function DosGetProgramName(buffer: pchar; length: Longint):Longint; Assembler;
Asm
  movl  _DOSBase,%eax
  pushl %eax
  pushl length
  pushl buffer
  addl  $_LVODos_GetProgramName,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $12,%esp
End ['eax'];

Function DosClose(handle : Longint):Longint; Assembler;
Asm
  movl  _DOSBase,%eax
  pushl %eax
  pushl handle
  addl  $_LVODos_Close,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $8,%esp
End ['eax'];

Function DosDeleteFile(Name: pchar):Longint; Assembler;
Asm
  movl  _DOSBase,%eax
  pushl %eax
  pushl Name
  addl  $_LVODos_DeleteFile,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $8,%esp
End ['eax'];

Function DosRename(NewName, OldName: pchar):Longint; Assembler;
Asm
  movl  _DOSBase,%eax
  pushl %eax
  pushl OldName
  pushl NewName
  addl  $_LVODos_Rename,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $12,%esp
End ['eax'];

Function DosWrite(handle, buffer, len: Longint):Longint; Assembler;
Asm
  movl  _DOSBase,%eax
  pushl %eax
  pushl len              { length }
  pushl buffer           { buffer }
  pushl handle           { file   }
  addl  $_LVODos_Write,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $16,%esp
End ['eax'];

Function DosRead(handle, addr, len: Longint):Longint; Assembler;
Asm
  movl  _DOSBase,%eax
  pushl %eax
  pushl len              { length }
  pushl addr             { buffer }
  pushl handle           { file   }
  addl  $_LVODos_Read,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $16,%esp
End ['eax'];

Function DosSeek(handle, offset, mode: Longint):Longint; Assembler;
Asm
  movl  _DOSBase,%eax
  pushl %eax
  pushl mode
  pushl offset
  pushl handle
  addl  $_LVODos_Seek,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $16,%esp
End ['eax'];

Function DosSetFileSize(handle,offset,mode:longint):Longint; Assembler;
Asm
  movl  _DOSBase,%eax
  pushl %eax
  pushl mode
  pushl offset
  pushl handle
  addl  $_LVODos_SetFileSize,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $16,%esp
End ['eax'];


Function DosOpen(Name: pchar; AccessMode: Longint):Longint; Assembler;
Asm
  movl  _DOSBase,%eax
  pushl %eax
  pushl AccessMode
  pushl Name
  addl  $_LVODos_Open,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $12,%esp
End ['eax'];

Function DosIoErr:Longint; Assembler;
Asm
  movl  _DOSBase,%eax
  pushl %eax
  addl  $_LVODos_IoErr,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $4,%esp
  movw  %ax,errno
End ['eax'];

Function DosLock(Name: pchar; AccessMode: Longint):Longint; Assembler;
Asm
    movl  _DOSBase,%eax
    pushl %eax
    pushl AccessMode
    pushl Name
    addl  $_LVODos_Lock,%eax
    movl  (%eax),%eax
    call  *%eax
    addl  $12,%esp
End ['eax'];

Function DosUnLock(aLock: Longint):Longint; Assembler;
Asm
  movl  _DOSBase,%eax
  pushl %eax
  pushl aLock
  addl  $_LVODos_UnLock,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $8,%esp
End ['eax'];

Function DosCreateDir(Name: pchar):Longint; Assembler;
Asm
  movl  _DOSBase,%eax
  pushl %eax
  pushl Name
  addl  $_LVODos_CreateDir,%eax
  movl  (%eax),%eax
  call  *%eax
  addl  $8,%esp
End ['eax'];

(*----------------------------------------------------*)

