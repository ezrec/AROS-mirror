{
    $Id$
    Copyright (c) 1998-2000 by Peter Vreman

    This unit handles the compilerhooks for output to external programs

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 ****************************************************************************
}
unit comphook;
interface

uses files;

Const
{ <$10000 will show file and line }
  V_None         = $0;
  V_Fatal        = $1;
  V_Error        = $2;
  V_Normal       = $4; { doesn't show a text like Error: }
  V_Warning      = $8;
  V_Note         = $10;
  V_Hint         = $20;
  V_Macro        = $100;
  V_Procedure    = $200;
  V_Conditional  = $400;
  V_Assem        = $800;
  V_Declarations = $1000;
  V_Info         = $10000;
  V_Status       = $20000;
  V_Used         = $40000;
  V_Tried        = $80000;
  V_Debug        = $100000;
  V_Executable   = $200000;
  V_ShowFile     = $ffff;
  V_All          = $ffffffff;
  V_Default      = V_Fatal + V_Error + V_Normal;

const
  { RHIDE expect gcc like error output }
  fatalstr      : string[20] = 'Fatal:';
  errorstr      : string[20] = 'Error:';
  warningstr    : string[20] = 'Warning:';
  notestr       : string[20] = 'Note:';
  hintstr       : string[20] = 'Hint:';

type
  PCompilerStatus = ^TCompilerStatus;
  TCompilerStatus = record
  { Current status }
    currentmodule,
    currentsourcepath,
    currentsource : string;   { filename }
    currentline,
    currentcolumn : longint;  { current line and column }
  { Total Status }
    compiledlines : longint;  { the number of lines which are compiled }
    errorcount    : longint;  { number of generated errors }
  { Settings for the output }
    verbosity     : longint;
    maxerrorcount : longint;
    errorwarning,
    errornote,
    errorhint,
    skip_error,
    use_stderr,
    use_redir,
    use_gccoutput,
    compiling_current : boolean;
  { Redirection support }
    redirfile : text;
  end;
var
  status : tcompilerstatus;

{ Default Functions }
procedure def_stop;
procedure def_halt(i : longint);
Function  def_status:boolean;
Function  def_comment(Level:Longint;const s:string):boolean;
function  def_internalerror(i:longint):boolean;
procedure def_initsymbolinfo;
procedure def_donesymbolinfo;
procedure def_extractsymbolinfo;
function  def_openinputfile(const filename: string): pinputfile;
Function  def_getnamedfiletime(Const F : String) : Longint;
{$ifdef DEBUG}
{ allow easy stopping in GDB
  using
  b DEF_GDB_STOP
  cond 1 LEVEL <= 8 }
procedure def_gdb_stop(level : longint);
{$endif DEBUG}
{ Function redirecting for IDE support }
type
  tstopprocedure         = procedure;
  thaltprocedure         = procedure(i : longint);
  tstatusfunction        = function:boolean;
  tcommentfunction       = function(Level:Longint;const s:string):boolean;
  tinternalerrorfunction = function(i:longint):boolean;

  tinitsymbolinfoproc = procedure;
  tdonesymbolinfoproc = procedure;
  textractsymbolinfoproc = procedure;
  topeninputfilefunc = function(const filename: string): pinputfile;
  tgetnamedfiletimefunc = function(const filename: string): longint;

const
  do_stop          : tstopprocedure   = def_stop;
  do_halt          : thaltprocedure   = def_halt;
  do_status        : tstatusfunction  = def_status;
  do_comment       : tcommentfunction = def_comment;
  do_internalerror : tinternalerrorfunction = def_internalerror;

  do_initsymbolinfo : tinitsymbolinfoproc = def_initsymbolinfo;
  do_donesymbolinfo : tdonesymbolinfoproc = def_donesymbolinfo;
  do_extractsymbolinfo : textractsymbolinfoproc = def_extractsymbolinfo;

  do_openinputfile : topeninputfilefunc = def_openinputfile;
  do_getnamedfiletime : tgetnamedfiletimefunc = def_getnamedfiletime;

implementation

  uses
{$ifdef USEEXCEPT}
   tpexcept,
{$endif USEEXCEPT}
{$ifdef Linux}
   linux,
{$endif}
   dos;

{****************************************************************************
                          Helper Routines
****************************************************************************}

function gccfilename(const s : string) : string;
var
  i : longint;
begin
  for i:=1to length(s) do
   begin
     case s[i] of
      '\' : gccfilename[i]:='/';
 'A'..'Z' : gccfilename[i]:=chr(ord(s[i])+32);
     else
      gccfilename[i]:=s[i];
     end;
   end;
  {$ifndef TP}
    {$ifopt H+}
      setlength(gccfilename,length(s));
    {$else}
      gccfilename[0]:=s[0];
    {$endif}
  {$else}
    gccfilename[0]:=s[0];
  {$endif}
end;


function tostr(i : longint) : string;
var
  hs : string;
begin
  str(i,hs);
  tostr:=hs;
end;


{****************************************************************************
                         Predefined default Handlers
****************************************************************************}

{ predefined handler when then compiler stops }
procedure def_stop;
begin
{$ifndef USEEXCEPT}
  Halt(1);
{$else USEEXCEPT}
  Halt(1);
{$endif USEEXCEPT}
end;

{$ifdef DEBUG}
{ allow easy stopping in GDB
  using
  b DEF_GDB_STOP
  cond 1 LEVEL <= 8 }
procedure def_gdb_stop(level : longint);
begin
  { Its only a dummy for GDB }
end;
{$endif DEBUG}

procedure def_halt(i : longint);
begin
  halt(i);
end;

function def_status:boolean;
begin
  def_status:=false; { never stop }
{ Status info?, Called every line }
  if ((status.verbosity and V_Status)<>0) then
   begin
{$ifndef Delphi}
     if (status.compiledlines=1) then
       WriteLn(memavail shr 10,' Kb Free');
{$endif Delphi}
     if (status.currentline>0) and (status.currentline mod 100=0) then
{$ifdef FPC}
       WriteLn(status.currentline,' ',memavail shr 10,'/',system.heapsize shr 10,' Kb Free');
{$else}
{$ifndef Delphi}
       WriteLn(status.currentline,' ',memavail shr 10,' Kb Free');
{$endif Delphi}
{$endif}
   end
end;


Function def_comment(Level:Longint;const s:string):boolean;
const
  rh_errorstr   = 'error:';
  rh_warningstr = 'warning:';
var
  hs : string;
begin
  def_comment:=false; { never stop }
  if (status.verbosity and Level)=Level then
   begin
     hs:='';
     if not(status.use_gccoutput) then
       begin
         if (status.verbosity and Level)=V_Hint then
           hs:=hintstr;
         if (status.verbosity and Level)=V_Note then
           hs:=notestr;
         if (status.verbosity and Level)=V_Warning then
           hs:=warningstr;
         if (status.verbosity and Level)=V_Error then
           hs:=errorstr;
         if (status.verbosity and Level)=V_Fatal then
           hs:=fatalstr;
       end
     else
       begin
         if (status.verbosity and Level)=V_Hint then
           hs:=rh_warningstr;
         if (status.verbosity and Level)=V_Note then
           hs:=rh_warningstr;
         if (status.verbosity and Level)=V_Warning then
           hs:=rh_warningstr;
         if (status.verbosity and Level)=V_Error then
           hs:=rh_errorstr;
         if (status.verbosity and Level)=V_Fatal then
           hs:=rh_errorstr;
       end;
     if (Level<=V_ShowFile) and (status.currentsource<>'') and (status.currentline>0) then
      begin
        { Adding the column should not confuse RHIDE,
        even if it does not yet use it PM
        but only if it is after error or warning !! PM }
        if status.currentcolumn>0 then
         begin
           if status.use_gccoutput then
             hs:=gccfilename(status.currentsource)+':'+tostr(status.currentline)+': '+hs+' '+
                 tostr(status.currentcolumn)+': '+s
           else
             hs:=status.currentsource+'('+tostr(status.currentline)+
                 ','+tostr(status.currentcolumn)+') '+hs+' '+s;
         end
        else
         begin
           if status.use_gccoutput then
             hs:=gccfilename(status.currentsource)+': '+hs+' '+tostr(status.currentline)+': '+s
           else
             hs:=status.currentsource+'('+tostr(status.currentline)+') '+hs+' '+s;
         end;
      end
     else
      begin
        if hs<>'' then
         hs:=hs+' '+s
        else
         hs:=s;
      end;
{$ifdef FPC}
     if status.use_stderr then
      begin
        writeln(stderr,hs);
        flush(stderr);
      end
     else
{$endif}
      begin
        if status.use_redir then
         writeln(status.redirfile,hs)
        else
         writeln(hs);
      end;
{$ifdef DEBUG}
     def_gdb_stop(level);
{$endif DEBUG}
   end;
end;


function def_internalerror(i : longint) : boolean;
begin
  do_comment(V_Fatal,'Internal error '+tostr(i));
  def_internalerror:=true;
end;

procedure def_initsymbolinfo;
begin
end;

procedure def_donesymbolinfo;
begin
end;

procedure def_extractsymbolinfo;
begin
end;

function  def_openinputfile(const filename: string): pinputfile;
begin
  def_openinputfile:=new(pdosinputfile, init(filename));
end;

Function def_GetNamedFileTime (Const F : String) : Longint;
   var
     L : Longint;
   {$ifndef linux}
     info : SearchRec;
   {$else}
     info : stat;
   {$endif}
   begin
     l:=-1;
   {$ifdef linux}
     if FStat (F,Info) then
      L:=info.mtime;
   {$else}
{$ifdef delphi}
     dmisc.FindFirst (F,archive+readonly+hidden,info);
{$else delphi}
     FindFirst (F,archive+readonly+hidden,info);
{$endif delphi}
     if DosError=0 then
      l:=info.time;
{$ifdef delphi}
     dmisc.FindClose (info);
{$else delphi}
     FindClose (info);
{$endif delphi}
   {$endif}
     def_GetNamedFileTime:=l;
   end;


end.
{
  $Log$
  Revision 1.1  2002/02/19 08:21:57  sasu
  Initial revision

  Revision 1.1.2.6  2000/11/23 12:51:11  pierre
   + added compiling_current to status record

  Revision 1.1.2.5  2000/10/16 19:42:04  pierre
   * findclose cleanup

  Revision 1.1.2.4  2000/09/30 16:07:03  peter
    * fix prefix without filepos

  Revision 1.1.2.3  2000/09/24 21:40:18  peter
    * error messages updated
    * if messages not available in message file fallback to the internal
      messages
    * message prefixes (like Note:) can now also be set in the msg file

  Revision 1.1.2.2  2000/08/13 13:03:19  peter
    * -vb update

  Revision 1.1.2.1  2000/08/12 15:29:52  peter
    * patch from Gabor for IDE to support memory stream reading

  Revision 1.1  2000/07/13 06:29:48  michael
  + Initial import

  Revision 1.23  2000/05/29 10:04:40  pierre
    * New bunch of Gabor changes

  Revision 1.22  2000/05/10 13:40:19  peter
    * -Se<x> option extended to increase errorcount for
      warning,notes or hints

  Revision 1.21  2000/02/09 13:22:50  peter
    * log truncated

  Revision 1.20  2000/01/07 01:14:23  peter
    * updated copyright to 2000

  Revision 1.19  1999/11/18 15:34:45  pierre
    * Notes/Hints for local syms changed to
      Set_varstate function

  Revision 1.18  1999/09/07 14:03:48  pierre
   + added do_halt procedure

  Revision 1.17  1999/08/05 16:52:53  peter
    * V_Fatal=1, all other V_ are also increased
    * Check for local procedure when assigning procvar
    * fixed comment parsing because directives
    * oldtp mode directives better supported
    * added some messages to errore.msg

}
