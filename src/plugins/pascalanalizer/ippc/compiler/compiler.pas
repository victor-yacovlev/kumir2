{
    This unit is the interface of the compiler which can be used by
    external programs to link in the compiler

    Copyright (c) 1998-2005 by Florian Klaempfl

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

 ****************************************************************************}

unit compiler;

{$i fpcdefs.inc}

interface

uses
{$ifdef GO32V2}
  emu387,
{$endif GO32V2}
{$ifdef WATCOM}
    emu387,
{$endif WATCOM}
{$IFNDEF USE_FAKE_SYSUTILS}
  sysutils,math,
{$ELSE}
  fksysutl,
{$ENDIF}
  verbose,comphook,systems,
  cutils,cfileutl,cclasses,globals,options,fmodule,parser,symtable,
  assemble,link,dbgbase,import,export,tokens,pass_1,wpobase,wpo
  { cpu parameter handling }
  ,cpupara
  { procinfo stuff }
  ,cpupi
  { cpu codegenerator }
  ,cgcpu
{$ifndef NOPASS2}
  ,cpunode
{$endif}
  { cpu targets }
  ,cputarg
  { system information for source system }
  { the information about the target os  }
  { are pulled in by the t_* units       }
{$ifdef amiga}
  ,i_amiga
{$endif amiga}
{$ifdef atari}
  ,i_atari
{$endif atari}
{$ifdef beos}
  ,i_beos
{$endif beos}
{$ifdef fbsd}
  ,i_fbsd
{$endif fbsd}
{$ifdef gba}
  ,i_gba
{$endif gba}
{$ifdef go32v2}
  ,i_go32v2
{$endif go32v2}
{$ifdef linux}
  ,i_linux
{$endif linux}
{$ifdef macos}
  ,i_macos
{$endif macos}
{$ifdef nds}
  ,i_nds
{$endif nds}
{$ifdef nwm}
  ,i_nwm
{$endif nwm}
{$ifdef nwl}
  ,i_nwl
{$endif nwm}
{$ifdef os2}
 {$ifdef emx}
  ,i_emx
 {$else emx}
  ,i_os2
 {$endif emx}
{$endif os2}
{$ifdef palmos}
  ,i_palmos
{$endif palmos}
{$ifdef solaris}
  ,i_sunos
{$endif solaris}
{$ifdef wdosx}
  ,i_wdosx
{$endif wdosx}
{$ifdef win32}
  ,i_win
{$endif win32}
{$ifdef symbian}
  ,i_symbian
{$endif symbian}
  ,globtype;

function Compile(const cmd:string; const source: pchar):longint;
procedure InitCompiler(const cmd:string);

implementation

uses
  aasmcpu;

{$if defined(EXTDEBUG) or defined(MEMDEBUG)}
  {$define SHOWUSEDMEM}
{$endif}

var
  CompilerInitedAfterArgs,
  CompilerInited : boolean;


{****************************************************************************
                                Compiler
****************************************************************************}

procedure DoneCompiler;
begin
  if not CompilerInited then
   exit;
{ Free compiler if args are read }
  if CompilerInitedAfterArgs then
   begin
     CompilerInitedAfterArgs:=false;
     DoneParser;
     DoneImport;
     DoneExport;
     DoneLinker;
     DoneAsm;
     DoneWpo;
   end;
{ Free memory for the others }
  CompilerInited:=false;
  do_doneSymbolInfo;
  DoneSymtable;
  DoneGlobals;
  DoneFileUtils;
  donetokens;
end;


procedure InitCompiler(const cmd:string);
begin
  if CompilerInited then
   DoneCompiler;
{ inits which need to be done before the arguments are parsed }
  InitSystems;
  { fileutils depends on source_info so it must be after systems }
  InitFileUtils;
  { globals depends on source_info so it must be after systems }
  InitGlobals;
  { verbose depends on exe_path and must be after globals }
  InitVerbose;
  inittokens;
  IniTSymtable; {Must come before read_arguments, to enable macrosymstack}
  do_initSymbolInfo;
  CompilerInited:=true;
{ this is needed here for the IDE
  in case of compilation failure
  at the previous compile }
  set_current_module(nil);
{ read the arguments }
  read_arguments(cmd);
{ inits which depend on arguments }
  InitParser;
  InitImport;
  InitExport;
  InitLinker;
  InitAsm;
  InitWpo;

  CompilerInitedAfterArgs:=true;
end;


function Compile(const cmd:string; const source: pchar):longint;

{$maxfpuregisters 0}

  procedure writepathlist(w:longint;l:TSearchPathList);
  var
    hp : TCmdStrListItem;
  begin
    hp:=TCmdStrListItem(l.first);
    while assigned(hp) do
     begin
       Message1(w,hp.str);
       hp:=TCmdStrListItem(hp.next);
     end;
  end;

var
  timestr    : string[20];
  linkstr    : string[64];
{$ifdef SHOWUSEDMEM}
  hstatus : TFPCHeapStatus;
{$endif SHOWUSEDMEM}
  ExceptionMask : TFPUExceptionMask;
  totaltime : real;
begin

       ExceptionMask:=GetExceptionMask;
       SetExceptionMask([exInvalidOp, exDenormalized, exZeroDivide,
                         exOverflow, exUnderflow, exPrecision]);

       starttime:=getrealtime;

       { Initialize the compiler }
       InitCompiler(cmd);

       { Compile the program }
  {$ifdef PREPROCWRITE}
       if parapreprocess then
        parser.preprocess(inputfilepath+inputfilename)
       else
  {$endif PREPROCWRITE}
        parser.compile(inputfilepath+inputfilename, source);


  { Set the return value if an error has occurred }
  if status.errorcount=0 then
    result:=0
  else
    result:=1;
end;

end.
