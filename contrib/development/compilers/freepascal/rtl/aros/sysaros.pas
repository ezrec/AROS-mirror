{
    $Id$
    This file is part of the Free Pascal run time library.
    Copyright (c) 1999-2000 by Carl Eric Codere
    Some parts taken from
       Marcel Timmermans - Modula 2 Compiler
       Nils Sjoholm - Amiga porter
       Matthew Dillon - Dice C (with his kind permission)
          dillon@backplane.com

    See the file COPYING.FPC, included in this distribution,
    for details about the copyright.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 **********************************************************************}
unit sysaros;

{--------------------------------------------------------------------}
{ LEFT TO DO:                                                        }
{--------------------------------------------------------------------}
{ o GetDir with different drive numbers                              }
{--------------------------------------------------------------------}

{$I os.inc}

{ AROS uses character #10 as eoln only }
{$DEFINE SHORT_LINEBREAK}

  interface

    { used for single computations }
    const BIAS4 = $7f-1;

    {$I systemh.inc}
    {$I heaph.inc}

       
const
  UnusedHandle    : longint = -1;
  StdInputHandle  : longint = 0;
  StdOutputHandle : longint = 0;
  StdErrorHandle  : longint = 0;

 _WorkbenchMsg  : longint = 0;
 _ExecBase      : pointer = nil;
 _DosBase       : pointer = nil;       { DOS library pointer       }
 _IntuitionBase : pointer = nil;       { intuition library pointer }
 _UtilityBase   : pointer = nil;       { utiity library pointer    }

 { Required for crt unit }
  function do_read(h,addr,len : longint) : longint;
  function do_write(h,addr,len : longint) : longint;





  implementation

 const

   intuitionname : pchar   = 'intuition.library';
   dosname       : pchar   = 'dos.library';
   utilityname   : pchar   = 'utility.library';
   argc          : longint = 0;

   { AROS does not automtically deallocate memory on program termination     }
   { therefore we have to handle this manually. This is a list of allocated  }
   { pointers from the OS, we cannot use a linked list, because the linked   }
   { list itself uses the HEAP!                                              }
   pointerlist : array[1..8] of longint = (0,0,0,0,0,0,0,0);


    {$I exec.inc}

  TYPE
    TDateStamp = packed record
        ds_Days         : Longint;      { Number of days since Jan. 1, 1978 }
        ds_Minute       : Longint;      { Number of minutes past midnight }
        ds_Tick         : Longint;      { Number of ticks past minute }
    end;
    PDateStamp = ^TDateStamp;


    PFileInfoBlock = ^TfileInfoBlock;
    TFileInfoBlock = packed record
        fib_DiskKey     : Longint;
        fib_DirEntryType : Longint;
                        { Type of Directory. If < 0, then a plain file.
                          If > 0 a directory }
        fib_FileName    : Array [0..107] of Char;
                        { Null terminated. Max 30 chars used for now }
        fib_Protection  : Longint;
                        { bit mask of protection, rwxd are 3-0. }
        fib_EntryType   : Longint;
        fib_Size        : Longint;      { Number of bytes in file }
        fib_NumBlocks   : Longint;      { Number of blocks in file }
        fib_Date        : TDateStamp; { Date file last changed }
        fib_Comment     : Array [0..79] of Char;
                        { Null terminated comment associated with file }
        fib_Reserved    : Array [0..35] of Char;
    end;


    TProcess = packed record
        pr_Task        : TTask;
        pr_MsgPort     : TMsgPort; { This is BPTR address from DOS functions  }
{126}   pr_Pad         : Word;     { Remaining variables on 4 byte boundaries }
{128}   pr_SegList     : Pointer;  { Array of seg lists used by this process  }
{132}   pr_StackSize   : Longint;  { Size of process stack in bytes           }
{136}   pr_GlobVec     : Pointer;  { Global vector for this process (BCPL)    }
{140}   pr_TaskNum     : Longint;  { CLI task number of zero if not a CLI     }
{144}   pr_StackBase   : BPTR;     { Ptr to high memory end of process stack  }
{148}   pr_Result2     : Longint;  { Value of secondary result from last call }
{152}   pr_CurrentDir  : BPTR;     { Lock associated with current directory   }
{156}   pr_CIS         : BPTR;     { Current CLI Input Stream                 }
{160}   pr_COS         : BPTR;     { Current CLI Output Stream                }
{164}   pr_ConsoleTask : Pointer;  { Console handler process for curr window  }
{168}   pr_FileSystemTask : Pointer;  { File handler process for curr drive   }
{172}   pr_CLI         : BPTR;     { pointer to ConsoleLineInterpreter        }
        pr_ReturnAddr  : Pointer;  { pointer to previous stack frame          }
        pr_PktWait     : Pointer;  { Function to be called when awaiting msg  }
        pr_WindowPtr   : Pointer;  { Window for error printing }
       
        { following definitions are new with 2.0 }
        pr_HomeDir     : BPTR;     { Home directory of executing program      }
        pr_Flags       : Longint;  { flags telling dos about process          }
        pr_ExitCode    : Pointer;  { code to call on exit of program OR NULL  }
        pr_ExitData    : Longint;  { Passed as an argument to pr_ExitCode.    }
        pr_Arguments   : PChar;    { Arguments passed to the process at start }
        pr_LocalVars   : TMinList; { Local environment variables              }
        pr_ShellPrivate: Longint;  { for the use of the current shell         }
        pr_CES         : BPTR;     { Error stream - IF NULL, use pr_COS       }
    end;
    PProcess = ^TProcess;

  
  { AROS does not automatically close opened files on exit back to  }
  { the operating system, therefore as a precuation we close all files }
  { manually on exit.                                                  }
  PFileList = ^TFileList;
  TFileList = record { no packed, must be correctly aligned }
    Handle: longint;      { Handle to file    }
    next: pfilelist;      { Next file in list }
    closed: boolean;      { TRUE=file already closed }
  end;



  Const
     CTRL_C               = 20;      { Error code on CTRL-C press }
     SIGBREAKF_CTRL_C     = $1000;   { CTRL-C signal flags }

      { Errors from IoErr(), etc. }
      ERROR_NO_FREE_STORE              = 103;
      ERROR_TASK_TABLE_FULL            = 105;
      ERROR_BAD_TEMPLATE               = 114;
      ERROR_BAD_NUMBER                 = 115;
      ERROR_REQUIRED_ARG_MISSING       = 116;
      ERROR_KEY_NEEDS_ARG              = 117;
      ERROR_TOO_MANY_ARGS              = 118;
      ERROR_UNMATCHED_QUOTES           = 119;
      ERROR_LINE_TOO_LONG              = 120;
      ERROR_FILE_NOT_OBJECT            = 121;
      ERROR_INVALID_RESIDENT_LIBRARY   = 122;
      ERROR_NO_DEFAULT_DIR             = 201;
      ERROR_OBJECT_IN_USE              = 202;
      ERROR_OBJECT_EXISTS              = 203;
      ERROR_DIR_NOT_FOUND              = 204;
      ERROR_OBJECT_NOT_FOUND           = 205;
      ERROR_BAD_STREAM_NAME            = 206;
      ERROR_OBJECT_TOO_LARGE           = 207;
      ERROR_ACTION_NOT_KNOWN           = 209;
      ERROR_INVALID_COMPONENT_NAME     = 210;
      ERROR_INVALID_LOCK               = 211;
      ERROR_OBJECT_WRONG_TYPE          = 212;
      ERROR_DISK_NOT_VALIDATED         = 213;
      ERROR_DISK_WRITE_PROTECTED       = 214;
      ERROR_RENAME_ACROSS_DEVICES      = 215;
      ERROR_DIRECTORY_NOT_EMPTY        = 216;
      ERROR_TOO_MANY_LEVELS            = 217;
      ERROR_DEVICE_NOT_MOUNTED         = 218;
      ERROR_SEEK_ERROR                 = 219;
      ERROR_COMMENT_TOO_BIG            = 220;
      ERROR_DISK_FULL                  = 221;
      ERROR_DELETE_PROTECTED           = 222;
      ERROR_WRITE_PROTECTED            = 223;
      ERROR_READ_PROTECTED             = 224;
      ERROR_NOT_A_DOS_DISK             = 225;
      ERROR_NO_DISK                    = 226;
      ERROR_NO_MORE_ENTRIES            = 232;
      { added for 1.4 }
      ERROR_IS_SOFT_LINK               = 233;
      ERROR_OBJECT_LINKED              = 234;
      ERROR_BAD_HUNK                   = 235;
      ERROR_NOT_IMPLEMENTED            = 236;
      ERROR_RECORD_NOT_LOCKED          = 240;
      ERROR_LOCK_COLLISION             = 241;
      ERROR_LOCK_TIMEOUT               = 242;
      ERROR_UNLOCK_ERROR               = 243;



    var
      Initial  : boolean;         { Have successfully opened Std I/O   }
      errno    : word;            { arosOS IO Error number            }
//      FileList : pFileList;       { Linked list of opened files        }
      FromHalt : boolean;
      OrigDir  : Longint;   { Current lock on original startup directory }

    {$I system.inc}

    {.$I lowmath.inc}

{$ifdef i386}
    {$I sysaros386.inc}
{$endif i386}
{$ifdef m68k}  
    {$I sysaros86k.inc}
{$endif m68k}



{ Converts an AROS error code to a TP compatible error code }
Procedure Error2InOut;
Begin
  case errno of
    ERROR_BAD_NUMBER,
    ERROR_ACTION_NOT_KNOWN,
    ERROR_NOT_IMPLEMENTED : InOutRes := 1;

    ERROR_OBJECT_NOT_FOUND : InOutRes := 2;

    ERROR_DIR_NOT_FOUND :  InOutRes := 3;

    ERROR_DISK_WRITE_PROTECTED : InOutRes := 150;

    ERROR_OBJECT_WRONG_TYPE : InOutRes := 151;

    ERROR_OBJECT_EXISTS,
    ERROR_DELETE_PROTECTED,
    ERROR_WRITE_PROTECTED,
    ERROR_READ_PROTECTED,
    ERROR_OBJECT_IN_USE,
    ERROR_DIRECTORY_NOT_EMPTY : InOutRes := 5;

    ERROR_NO_MORE_ENTRIES : InOutRes := 18;

    ERROR_RENAME_ACROSS_DEVICES : InOutRes := 17;

    ERROR_DISK_FULL : InOutRes := 101;

    ERROR_INVALID_RESIDENT_LIBRARY : InoutRes := 153;

    ERROR_BAD_HUNK : InOutRes := 153;

    ERROR_NOT_A_DOS_DISK : InOutRes := 157;

    ERROR_NO_DISK,
    ERROR_DISK_NOT_VALIDATED,
    ERROR_DEVICE_NOT_MOUNTED : InOutRes := 152;

    ERROR_SEEK_ERROR : InOutRes := 156;

    ERROR_LOCK_COLLISION,
    ERROR_LOCK_TIMEOUT,
    ERROR_UNLOCK_ERROR,
    ERROR_INVALID_LOCK,
    ERROR_INVALID_COMPONENT_NAME,
    ERROR_BAD_STREAM_NAME,
    ERROR_FILE_NOT_OBJECT : InOutRes := 6;
  else
    InOutres := errno;
  end;
  errno:=0;
end;

  { ************************ AROS SUPP ROUTINES ************************* }

{ This routine from EXEC determines if the Ctrl-C key has }
{ been used since the last call to I/O routines.          }
{ Use to halt the program.                                }
{ Returns the state of the old signals.                   }
Function SetSignal(newSignal: longint; SignalMask: longint): longint;
Begin
  Result := ExecSetSignal(newSignal, SignalMask);
End;

Function OpenLibrary(libname : pchar; libver: Longint):pointer;
Begin
   Result := pointer(ExecOpenLibrary(libname, libver));
End;

{  Close the library pointed to in lib }
Procedure CloseLibrary(lib : pointer);
Begin
  ExecCloseLibrary(lib);
End;

Procedure FreeVec(p: longint);
Begin
  ExecFreeVec(p);
End;

Function AllocVec(bytesize: longint; attributes: longint):longint;
Begin
  Result := ExecAllocVec(bytesize, attributes);
End;

Procedure DateStamp(var ds : tDateStamp);
Begin
  DosDateStamp(ds);
End;

{ UNLOCK the BPTR pointed to in alock }
Procedure Unlock(alock: longint);
Begin
  DosUnLock(aLock);
End;

Function FindTask(p : PChar): PProcess;
Begin
  Result := PProcess(ExecFindTask(p));
End;

Function NameFromLock(alock : Longint):string;
Var s : shortstring;
    i : integer;
Begin
  DosNameFromLock(alock, @s[1], 255);

  { now check out the length of the string }
  i := 1;
  while s[i] <> #0 do begin
    Inc(i);
  end;
  SetLength(s, i-1);
  Result := s;
End;

{ Returns the parent directory of a lock }
Function ParentDir(alock : longint): longint;
Begin
  Result := DosParentDir(alock);
End;

Function Examine(alock :longint; var fib: TFileInfoBlock) : Boolean;
Begin
  If DosExamine(alock, fib) = 0 Then Begin
    Result := False;
  End
  Else Begin
    Result := True;
  End;
End;

{ Returns a lock on the directory the program was loaded from }
Function GetProgramLock: longint;
Begin
  Result := DosGetProgramDir;
End;

{ Duplicate a lock }
Function DupLock(alock: longint): Longint;
Begin
  Result := DosDupLock(alock);
End;

{ Change to the directory pointed to in the lock }
Function CurrentDir(alock : longint) : longint;
Begin
  Result := DosCurrentDir(alock);
End;


{*****************************************************************************
                         System Dependent Exit code
*****************************************************************************}
Procedure system_exit;
Var i: byte;
Begin
  { We must remove the CTRL-C FLAG here because halt }
  { may call I/O routines, which in turn might call  }
  { halt, so a recursive stack crash                 }
  IF (SetSignal(0,0) AND SIGBREAKF_CTRL_C) <> 0 THEN
    SetSignal(0,SIGBREAKF_CTRL_C);

  If (OrigDir <> 0) Then Begin
    Unlock(CurrentDir(OrigDir));
    OrigDir := 0;
  End;

  { Is this a normal exit - YES, close libs }
  If NOT FromHalt Then Begin
    { close the libraries }
    If _UtilityBase   <> nil Then CloseLibrary(_UtilityBase);
    If _DosBase       <> nil Then CloseLibrary(_DosBase);
    If _IntuitionBase <> nil Then CloseLibrary(_IntuitionBase);
    _UtilityBase := nil;
    _DosBase := nil;
    _IntuitionBase := nil;
  End;

  { Dispose of extraneous allocated pointers }
  For i := 1 to 8 Do Begin
    If pointerlist[i] <> 0 then
      FreeVec(pointerlist[i]);
  End;
End;


  { ************************ PARAMCOUNT/PARAMSTR *************************** }

Function paramcount : longint;
Begin
  paramcount := argc;
End;

Function GetParamCount(const p: pchar): longint;
Var i    : word;
    count: word;
Begin
  i := 0;
  count := 0;
  While p[count] <> #0 Do Begin
    If (p[count] <> ' ') And (p[count] <> #9) And (p[count] <> #0) Then Begin
      i := i+1;
      While (p[count] <> ' ') And (p[count] <> #9) And (p[count] <> #0) Do
        count := count+1;
    End;
    If p[count] = #0 Then
      Break;
    count := count+1;
  End;
  GetParamCount := longint(i);
End;


{ On Entry: index = string index to correct parameter  }
{ On exit:  = correct character index into pchar array }
{ Returns correct index to command line argument }
Function GetParam(index: word; const p : pchar): string;
Var count     : word;
    localindex: word;
    l         : byte;
    temp      : shortstring;
Begin
  temp := '';
  count := 0;

  { first index is one }
  localindex := 1;
  l := 0;
  While p[count] <> #0 Do Begin
    If (p[count] <> ' ') And (p[count] <> #9) Then Begin
      If localindex = index Then Begin
        While (p[count] <>  #0) And (p[count] <> ' ')
	  And (p[count] <>  #9) And (l < 256        ) Do Begin
          temp := temp+p[count];
          l := l+1;
          count := count+1;
        End;
        temp[0] := char(l);
        GetParam := temp;
        exit;
      End;

      { Point to next argument in list }
      While (p[count] <> #0)
	And (p[count] <> ' ') And (p[count] <> #9) Do Begin
        count := count+1;
      End;
      localindex := localindex+1;
    End;
    if p[count] = #0 Then
      Break;
    count := count+1;
  End;
  GetParam := temp;
End;


Function GetProgramDir : String;
var alock : longint;
Begin
  { GetLock of program directory }
  alock := GetProgramLock;
  If alock <> 0 Then Begin
    { Get the name from the lock! }
    Result := NameFromLock(alock);
  End
  Else Begin
    Result := '';
  End;
End;


{ Returns ONLY the program name }
{ There seems to be a bug in v39 since if the program is not }
{ called from its home directory the program name will also  }
{ contain the path!                                          }
Function GetProgramName : string;
var s: shortstring;
    i: byte;
Begin

  FillChar(s, 255, #0);
  DosGetProgramName(@s[1], 255);

  { now check out and assign the length of the string }
  i := 1;
  while s[i] <> #0 do begin
    Inc(i);
  end;

  SetLength(s, i-1);

  { now remove any component path which should not be there }
  for i := length(s) downto 1 do
    if (s[i] = '/') or (s[i] = ':') then break;

  { readajust i to point to character }
  if i <> 1 then
    Inc(i);

  Result := copy(s, i, length(s));
End;


Function paramstr(l : longint) : string;
Var p : pchar;
    s : string;
Begin

  if l = 0 then Begin
     
    s := GetProgramDir;
     
    { If this is a root, then simply don't add '/' }
    if s[length(s)] = ':' then begin
      Result := s+GetProgramName;
    end
    else begin
      { add directory separator }
      Result := s+'/'+GetProgramName;
    end;
  end
  else if (l > 0) and (l <= paramcount) then begin
    p := args;
    Result := GetParam(word(l), p);
  end
  else begin
    Result := '';
  end;
End;

  { ************************************************************************ }

Procedure randomize;
Var time : TDateStamp;
begin
  DateStamp(time);
  randseed := time.ds_tick;
end;


{ This routine is used to grow the heap.  }
{ But here we do a trick, we say that the }
{ heap cannot be regrown!                 }
{ return -1 = if failure.                 }
function sbrk( size: longint): longint;

var p: longint;
    i: byte;

Begin

  p := 0;

  { Is the pointer list full }
  if pointerlist[8] <> 0 then begin
    { yes, then don't allocate and simply exit }
    Result := -1;
    exit;
  end;

  { Allocate best available memory }
  p := AllocVec(size, 0);
  if p = 0 then begin
    Result := -1;
  end
  else Begin
    i := 1;

    { add it to the list of allocated pointers }
    { first find the last pointer in the list  }
    while (i < 8) and (pointerlist[i] <> 0) do
      i := i+1;

    pointerlist[i]:=p;
    Result := p;
  end;
end;



{$I heap.inc}


{****************************************************************************
                          Low Level File Routines
 ****************************************************************************}
Const
   OFFSET_END	    =  1;
   OFFSET_CURRENT   =  0;
   OFFSET_BEGINNING = -1;

   ACCESS_READ      = -2;


{ We cannot check for CTRL-C because this routine will be called }
{ on HALT to close all remaining opened files. Therefore no      }
{ CTRL-C checking otherwise a recursive call might result!       }
Procedure do_close(h : longint);
Begin
  DosClose(h);
End;

Procedure do_erase(p : pchar);
Var err : Longint;
Begin
  If (Setsignal(0,0) AND SIGBREAKF_CTRL_C) <> 0 Then Begin
    SetSignal(0,SIGBREAKF_CTRL_C);
    Halt(CTRL_C);
  End;
  err := DosDeleteFile(p);
  If err = 0 Then Begin
    DosIoErr;
  End;
  If errno <> 0 Then Begin
    Error2InOut;
  End;
End;

Procedure do_rename(p1,p2 : pchar);
Var err : Longint;
Begin
  If (Setsignal(0,0) AND SIGBREAKF_CTRL_C) <> 0 Then Begin
    SetSignal(0,SIGBREAKF_CTRL_C);
    Halt(CTRL_C);
  End;
  err := DosRename(p1, p2);
  If err = 0 Then Begin
    DosIoErr;
  End;
  If errno <> 0 Then Begin
    Error2InOut;
  End;
End;

Function do_write(h,addr,len : longint) : longint;
Var num : Longint;
Begin
  If (Setsignal(0,0) AND SIGBREAKF_CTRL_C) <> 0 Then Begin
    SetSignal(0,SIGBREAKF_CTRL_C);
    Halt(CTRL_C);
  End;
  If len <= 0 Then Begin
    Result := 0;
    exit;
  End;
  num := DosWrite(h, addr, len);
  If num = -1 Then Begin
    DosIoErr;
  End;
  If errno <> 0 Then Begin
    Error2InOut;
  End;
  Result := num;
End;

Function do_read(h,addr,len : longint) : longint;
Var num : Longint;
Begin
  If (Setsignal(0,0) AND SIGBREAKF_CTRL_C) <> 0 Then Begin
    SetSignal(0,SIGBREAKF_CTRL_C);
    Halt(CTRL_C);
  End;
  If len <= 0 Then Begin
    Result := 0;
    exit;
  End;
  num := DosRead(h, addr, len);
  If num = -1 Then Begin
    DosIoErr;
  End;
  If errno <> 0 Then Begin
    Error2InOut;
  End;
  Result := num;
End;


Function do_filepos(handle : longint) : longint;
Var filepos : Longint;
Begin
  If (Setsignal(0,0) AND SIGBREAKF_CTRL_C) <> 0 Then Begin
    { Clear CTRL-C signal }
    SetSignal(0,SIGBREAKF_CTRL_C);
    Halt(CTRL_C);
  End;
  filepos := DosSeek(handle, 0, OFFSET_CURRENT);
  If filepos = -1 Then Begin
    DosIoErr;
  End;
  If errno <> 0 Then Begin
    Error2InOut;
  End;
  Result := filepos;
End;

Procedure do_seek(handle, pos : longint);
Var oldpos : Longint;
Begin
  If (Setsignal(0,0) AND SIGBREAKF_CTRL_C) <> 0 Then Begin
    { Clear CTRL-C signal }
    SetSignal(0,SIGBREAKF_CTRL_C);
    Halt(CTRL_C);
  end;
  oldpos := DosSeek(handle, pos, OFFSET_BEGINNING);
  If oldpos = -1 Then Begin
    DosIoErr;
  End;
  If errno <> 0 Then Begin
    Error2InOut;
  End;
End;

{ seek from end of file }
Function do_seekend(handle:longint):longint;
Var filepos : Longint;
Begin
  If (Setsignal(0,0) AND SIGBREAKF_CTRL_C) <> 0 Then Begin
    { Clear CTRL-C signal }
    SetSignal(0,SIGBREAKF_CTRL_C);
    Halt(CTRL_C);
  End;
  filepos := DosSeek(handle, 0, OFFSET_END);
  If filepos = -1 Then Begin
    DosIoErr;
  End;
  If Errno <> 0 Then Begin
    Error2InOut;
  End;
  Result := filepos;
End;

{ truncate file 'handle' at position 'pos' }
Procedure do_truncate (handle,pos:longint);
Var size : Longint;
Begin
  size := DosSetFileSize(handle, pos, -1 { OFFSET_BEGINNING });
  If size = -1 Then Begin
    DosIoErr;
  End;
  If Errno <> 0 Then Begin
    Error2InOut;
  End;
End;


Function Open(Name: pchar; AccessMode: Longint):Longint;
Var aLock: Longint;
Begin
  aLock := DosOpen(Name, AccessMode);
  If (aLock = 0) Then Begin
    aLock := DosIoErr;
  End;
  Result := aLock;
End;

Function CreateDir(Name: pchar):Longint;
Var aLock: Longint;
Begin
  aLock := DosCreateDir(Name);
  If (aLock = 0) Then Begin
    Result := DosIoErr;
  End
  Else Begin
    Result := DosUnLock(aLock);
  End;
End;

function do_isdevice(handle:longint):boolean;
begin
  if (handle = stdoutputhandle)
  or (handle = stdinputhandle )
  or (handle = stderrorhandle ) then
    do_isdevice := TRUE
  else
    do_isdevice := FALSE;
end;


Function do_filesize(handle : longint) : longint;
Var aktfilepos : longint;
Begin
  If (Setsignal(0,0) AND SIGBREAKF_CTRL_C) <> 0 Then Begin
    { Clear CTRL-C signal }
    SetSignal(0,SIGBREAKF_CTRL_C);
    Halt(CTRL_C);
  End;
  aktfilepos := do_filepos(handle);

 { We have to do this two times, because seek returns the }
 { OLD position                                           }
 do_filesize := do_seekend(handle);
 do_filesize := do_seekend(handle);
 do_seek(handle, aktfilepos);
End;


{
  filerec and textrec have both handle and mode as the first items so
  they could use the same routine for opening/creating.
  when (flags and $100)   the file will be append
  when (flags and $1000)  the file will be truncate/rewritten
  when (flags and $10000) there is no check for close (needed for textfiles)
}
Procedure do_open(var f; p:pchar; flags:longint);

Var i      : longint;
    oflags : longint;
    path   : string;
    buffer : array[0..255] of char;
    s      : string;

Begin

  path := strpas(p);
  For i := 1 to length(path) Do Begin
    If path[i] = '\' Then Begin
      path[i] := '/';
    End;
  End;
   
  { remove any dot characters and replace by their current }
  { directory equivalent.                                  }
  If pos('../', path) = 1 Then Begin
    { look for parent directory }
    delete(path,1,3);
    getdir(0,s);
    i := length(s);
    While (s[i] <> '/') And (s[i] <> ':') And (i > 0 ) Do Begin
      dec(i);
    End;
    If i > 0 Then Begin
      s := copy(s,1,i);
    End;
    path := s+path;
  End
  Else If pos('./',path) = 1 Then Begin
    { look for current directory }
    delete(path,1,2);
    getdir(0,s);
    If (s[length(s)] <> '/') And (s[length(s)] <> ':') Then Begin
      s := s+'/';
    End;
    path := s+path;
  End;

  move(path[1], buffer, length(path));
  buffer[length(path)] := #0;

  { close first if opened }
  If ((flags and $10000)=0) Then Begin
    Case filerec(f).mode Of
      fminput,fmoutput,fminout : Do_Close(filerec(f).handle);
      fmclosed : ;
      Else Begin
        inoutres := 102; {not assigned}
        exit;
      End;
    End;
  End;
   
  { reset file handle }
  filerec(f).handle := UnusedHandle;
   
  { convert filemode to filerec modes }
  { READ/WRITE on existing file }
  { RESET/APPEND                }
  oflags := 1005;
  Case (flags And 3) Of
    0 : filerec(f).mode := fminput;
    1 : filerec(f).mode := fmoutput;
    2 : filerec(f).mode := fminout;
  End; { case }
   
  { READ/WRITE mode, create file in all cases }
  { REWRITE                                   }
  If (flags and $1000) <> 0 Then Begin
    filerec(f).mode := fmoutput;
    oflags := 1006;
  End
  Else If (flags and $100)<>0 Then Begin
    { READ/WRITE mode on existing file APPEND }
    filerec(f).mode := fmoutput;
    oflags := 1005;
  End;

  { empty name is special }
  If p[0] = #0 Then Begin
    Case filerec(f).mode Of
      fminput : filerec(f).handle := StdInputHandle;
      fmappend,
      fmoutput : begin
                   filerec(f).handle := StdOutputHandle;
                   filerec(f).mode := fmoutput; {fool fmappend}
                 end;
    end;
    exit;
  end;

  filerec(f).handle := Open(buffer, oflags);
  If Errno <> 0 Then Begin
    Error2InOut;
  End;
   
  If (flags and $100) <> 0 Then Begin
    do_seekend(filerec(f).handle);
  End;
End;

{*****************************************************************************
                           UnTyped File Handling
*****************************************************************************}

{$i file.inc}

{*****************************************************************************
                           Typed File Handling
*****************************************************************************}

{$i typefile.inc}

{*****************************************************************************
                           Text File Handling
*****************************************************************************}

{$i text.inc}

{*****************************************************************************
                           Directory Handling
*****************************************************************************}

Procedure mkdir(const s : string);[IOCheck];
Var buffer : Array [0..255] Of Char;
    j      : Integer;
    temp   : string;
Begin
  { We must check the Ctrl-C before IOChecking of course! }
  If (Setsignal(0,0) AND SIGBREAKF_CTRL_C) <> 0 Then Begin
    { Clear CTRL-C signal }
    SetSignal(0,SIGBREAKF_CTRL_C);
    Halt(CTRL_C);
  End;
  If InOutRes <> 0 Then Begin
    exit;
  End;
  temp := s;
  For j := 1 To length(temp) Do Begin
    If temp[j] = '\' Then Begin
      temp[j] := '/';
    End;
  End;
  move(temp[1], buffer, length(temp));
  buffer[length(temp)] := #0;
  CreateDir(buffer);
  If errno <> 0 Then Begin
    Error2InOut;
  End;
End;


Procedure rmdir(const s : string);[IOCheck];
Var buffer : Array[0..255] Of Char;
    j      : Integer;
    temp   : String;
Begin
  { We must check the Ctrl-C before IOChecking of course! }
  If (Setsignal(0,0) AND SIGBREAKF_CTRL_C) <> 0 Then Begin
    { Clear CTRL-C signal }
    SetSignal(0,SIGBREAKF_CTRL_C);
    Halt(CTRL_C);
  end;
  If InOutRes <> 0 Then Begin
    exit;
  End;
  temp := s;
  For j := 1 to length(temp) Do Begin
    If temp[j] = '\' Then Begin
      temp[j] := '/';
    End;
  End;
  move(temp[1], buffer, length(temp));
  buffer[length(temp)] := #0;
  do_erase(buffer);
End;


Procedure chdir(const s : string);[IOCheck];
Var buffer : Array [0..255] Of Char;
    alock  : Longint;
    FIB    : pFileInfoBlock;
    j      : Integer;
    temp   : String;
Begin
  If (Setsignal(0,0) AND SIGBREAKF_CTRL_C) <> 0 Then Begin
    { Clear CTRL-C signal }
    SetSignal(0,SIGBREAKF_CTRL_C);
    Halt(CTRL_C);
  End;
  If InOutRes <> 0 Then Begin
    exit;
  End;
  temp := s;
  For j := 1 to length(temp) Do Begin
    If temp[j] = '\' Then Begin
      temp[j] := '/';
    End;
  End;
  { Return parent directory }
  If s = '..' Then Begin
    getdir(0, temp);
    j := length(temp);
    { Look through the previous paths }
    While (temp[j] <> '/') And (temp[j] <> ':') And (j > 0 ) Do Begin
      dec(j);
    End;
    If j > 0 Then Begin
      temp := copy(temp,1,j);
    End;
  End;
  alock := 0;
  fib   := nil;
  new(fib);

  move(temp[1], buffer, length(temp));
  buffer[length(temp)] := #0;

  { Changing the directory is a pretty complicated affair }
  {   1) Obtain a lock on the directory                   }
  {   2) CurrentDir the lock                              }
  alock := DosLock(buffer, ACCESS_READ);
  If (alock = 0) Then Begin
    DosIoErr;
  End;
  If errno <> 0 then Begin
     Error2InOut;
     exit;
  End;
  If (Examine(alock, fib^) = TRUE) And (fib^.fib_DirEntryType > 0) Then Begin
    alock := CurrentDir(alock);
    If OrigDir = 0 Then Begin
      OrigDir := alock;
      alock := 0;
    End;
  End;
  If alock <> 0 Then Begin
    Unlock(alock);
  End;
  If assigned(fib) Then Begin
    dispose(fib);
  End;
End;




Procedure GetCwd(var path: string);
Var lock    : Longint;
    fib     : PfileInfoBlock;
    len     : Integer;
    newlock : Longint;
    elen    : Integer;
    Process : PProcess;
Begin
  len  := 0;
  path := '';
  fib  := nil;
   
  { By using a pointer instead of a local variable}
  { we are assured that the pointer is aligned on }
  { a dword boundary.                             }
  new(fib);
  Process := FindTask(nil);
  If (process^.pr_Task.tc_Node.ln_Type = NT_TASK) Then Begin
    path := '';
    exit;
  End;

  lock := DupLock(process^.pr_CurrentDir);
  If (Lock = 0) Then Begin
    path := '';
    exit;
  End;

  While (lock <> 0) and (Examine(lock,FIB^) = TRUE) Do Begin
    elen := strlen(fib^.fib_FileName);
    If (len + elen + 2 > 255) Then Begin
      break;
    End;
    newlock := ParentDir(lock);
    If (len <> 0) Then Begin
      If (newlock <> 0) Then Begin
        path := '/'+path
      End
      Else Begin
        path := ':'+path;
      End;
      path := strpas(fib^.fib_FileName)+path;
      Inc(len);
    End
    Else Begin
      path := strpas(fib^.fib_Filename);
      If (newlock = 0) Then Begin
        path := path+':';
      End;
    End;
    len := len + elen;
    UnLock(lock);
    lock := newlock;
  End;
  If (lock <> 0) Then Begin
    UnLock(lock);
    path := '';
  End;
  If assigned(fib) Then Begin
    dispose(fib);
  End;
End;


Procedure getdir(drivenr : byte;var dir : shortstring);
Begin
  If (Setsignal(0,0) AND SIGBREAKF_CTRL_C) <> 0 Then Begin
    { Clear CTRL-C signal }
    SetSignal(0,SIGBREAKF_CTRL_C);
    Halt(CTRL_C);
  End;
  GetCwd(dir);
  If errno <> 0 Then Begin
    Error2InOut;
  End;
End;


{*****************************************************************************
                         SystemUnit Initialization
*****************************************************************************}

{*****************************************************************************
                         SystemUnit Initialization
*****************************************************************************}

Procedure Startup;
Var this_task : PProcess;
Begin
  this_task := FindTask(0);
  If this_task^.pr_CLI = 0 Then Begin
    { we do not support Workbench yet ..       }
    Halt(1);
  End;
   
  {  Open the following libraries:            }
  {   Intuition.library                       }
  {   dos.library                             }
  _IntuitionBase := OpenLibrary(intuitionname, 0);
  If _IntuitionBase = nil Then Begin
    Halt(219);
  End;
   
  _UtilityBase := OpenLibrary(utilityname, 0);
  If _UtilityBase = nil Then Begin
    Halt(219);
  End;

  _DOSBase := OpenLibrary(dosname, 0);
  If _DosBase = nil Then Begin
    Halt(219);
  End;
   
  { Find standard input and output for CLI }
  StdInputHandle := DosInput;
  StdOutputHandle := DosOutput;
End;


begin
  errno    := 0;
  FromHalt := FALSE;
   
  {  Initial state is on }
  {    -- in case of RunErrors before the i/o handles are ok. }
  Initial := TRUE;
   
  { Initialize ExitProc }
  ExitProc := Nil;
  Startup;
   
  { to test stack depth }
  loweststack := maxlongint;
   
  { Setup heap }
  InitHeap;
   
  { Setup stdin, stdout and stderr }
  OpenStdIO(Input,fmInput,StdInputHandle);
  OpenStdIO(Output,fmOutput,StdOutputHandle);
  OpenStdIO(StdOut,fmOutput,StdOutputHandle);
   
  { The aros does not seem to have a StdError }
  { handle, therefore make the StdError handle }
  { equal to the StdOutputHandle.              }
  StdErrorHandle := StdOutputHandle;
  OpenStdIO(StdErr,fmOutput,StdErrorHandle);
   
  { Now Handles and function handlers are setup correctly. }
  Initial := FALSE;
   
  { Reset IO Error }
  InOutRes := 0;
   
  { Startup - Only arosOS v2.04 or greater is supported }
  If KickVersion < 36 then Begin
    WriteLn('v36 or greater of Kickstart required.');
    Halt(1);
  end; { Startup }

  argc := GetParamCount(args);
  OrigDir := 0;
//  FileList := nil;
end.


{
  $Log$
  Revision 1.2  2002/02/27 08:45:39  dgs
  Work in progress.

  Revision 1.1.1.1  2002/02/19 08:24:57  sasu
  Initial import. See readme.aros in freepascal/rtl/aros.

  Compiler have target AROS now. Left to do is hundreds of m68k asm parts of system includes, mainly libcall wrappers.
  To find places where to make AROS specifig version, seek [AROS VERSION HERE] string and write i386 version of function or
  procedure.

  Revision 1.1  2000/07/13 06:30:29  michael
  + Initial import

  Revision 1.15  2000/01/07 16:41:29  daniel
    * copyright 2000

  Revision 1.14  2000/01/07 16:32:22  daniel
    * copyright 2000 added

  Revision 1.13  1999/09/10 15:40:32  peter
    * fixed do_open flags to be > $100, becuase filemode can be upto 255

  Revision 1.12  1999/01/18 10:05:47  pierre
   + system_exit procedure added

  Revision 1.11  1998/12/28 15:50:42  peter
    + stdout, which is needed when you write something in the system unit
      to the screen. Like the runtime error

  Revision 1.10  1998/09/14 10:48:00  peter
    * FPC_ names
    * Heap manager is now system independent

  Revision 1.9  1998/08/17 12:34:22  carl
    * chdir accepts .. characters
    + added ctrl-c checking
    + implemented sbrk
    * exit code was never called if no error was found on exit!
    * register was not saved in do_open

  Revision 1.8  1998/07/13 12:32:18  carl
    * do_truncate works, some cleanup

  Revision 1.6  1998/07/02 12:37:52  carl
    * IOCheck for chdir,rmdir and mkdir as in TP

  Revision 1.5  1998/07/01 14:30:56  carl
    * forgot that includes are case sensitive

  Revision 1.4  1998/07/01 14:13:50  carl
    * do_open bugfix
    * correct conversion of aros error codes to TP error codes
    * InoutRes word bugfix
    * parameter counting fixed
    * new stack checking implemented
    + IOCheck for chdir,rmdir,getdir and rmdir
    * do_filepos was wrong
    + chdir correctly implemented
    * getdir correctly implemented

  Revision 1.1.1.1  1998/03/25 11:18:47  root
  * Restored version

  Revision 1.14  1998/03/21 04:20:09  carl
    * correct ExecBase pointer (from Nils Sjoholm)
    * correct OpenLibrary vector (from Nils Sjoholm)

  Revision 1.13  1998/03/14 21:34:32  carl
    * forgot to save a6 in Startup routine

  Revision 1.12  1998/02/24 21:19:42  carl
  *** empty log message ***

  Revision 1.11  1998/02/23 02:22:49  carl
    * bugfix if linking problems

  Revision 1.9  1998/02/06 16:34:32  carl
    + do_open is now standard with other platforms

  Revision 1.8  1998/02/02 15:01:45  carl
    * fixed bug with opening library versions (from Nils Sjoholm)

  Revision 1.7  1998/01/31 19:35:19  carl
    + added opening of utility.library

  Revision 1.6  1998/01/29 23:20:54  peter
    - Removed Backslash convert

  Revision 1.5  1998/01/27 10:55:04  peter
    * aros uses / not \, so change AllowSlash -> AllowBackSlash

  Revision 1.4  1998/01/25 21:53:20  peter
    + Universal Handles support for StdIn/StdOut/StdErr
    * Updated layout of sysaros.pas

  Revision 1.3  1998/01/24 21:09:53  carl
    + added missing input/output function pointers

  Revision 1.2  1998/01/24 14:08:25  carl
    * RunError 217 --> RunError 219 (cannot open lib)
    + Standard Handle names implemented

  Revision 1.1  1998/01/24 05:12:15  carl
    + initial revision, some stuff still missing though.
      (and as you might imagine ... untested :))
}
