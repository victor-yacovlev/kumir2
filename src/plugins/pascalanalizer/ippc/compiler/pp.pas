{
    Copyright (c) 1998-2002 by Florian Klaempfl

    Commandline compiler for Free Pascal

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
program pp;

{
  possible compiler switches:
  -----------------------------------------------------------------
  CMEM                use cmem unit for better memory debugging
  I386                generate a compiler for the Intel i386+
  x86_64              generate a compiler for the AMD x86-64 architecture
  M68K                generate a compiler for the M68000
  SPARC               generate a compiler for SPARC
  POWERPC             generate a compiler for the PowerPC
  POWERPC64           generate a compiler for the PowerPC64 architecture
  VIS                 generate a compile for the VIS
  DEBUG               version with debug code is generated
  EXTDEBUG            some extra debug code is executed
  SUPPORT_MMX         only i386: releases the compiler switch
                      MMX which allows the compiler to generate
                      MMX instructions
  EXTERN_MSG          Don't compile the msgfiles in the compiler, always
                      use external messagefiles, default for TP
  FPC_ARMEL           create an arm eabi compiler
  FPC_ARMEB           create an arm big endian compiler
  FPC_OARM            create an arm oabi compiler, only needed when the host
                      compiler is ARMEL or ARMEB
  -----------------------------------------------------------------
  cpuflags            The target processor has status flags (on by default)
  cpufpemu            The target compiler will also support emitting software
                       floating point operations
  cpu64bitaddr        Generate code for a 64-bit address space
  cpu64bitalu         The target cpu has 64-bit registers and a 64 bit alu
                      (required for cpu64bitaddr; optional with 32 bit addr space)
  -----------------------------------------------------------------
}

{$i fpcdefs.inc}

{ Require at least 2.0.2 }
{$ifdef VER2_0}
  {$if FPC_PATCH<2}
    {$fatal At least FPC 2.0.2 is required to compile the compiler}
  {$endif}
{$endif VER2_0}

{ exactly one target CPU must be defined }
{$ifdef I386}
  {$ifdef CPUDEFINED}
    {$fatal ONLY one of the switches for the CPU type must be defined}
  {$endif CPUDEFINED}
  {$define CPUDEFINED}
{$endif I386}
{$ifdef x86_64}
  {$ifdef CPUDEFINED}
    {$fatal ONLY one of the switches for the CPU type must be defined}
  {$endif CPUDEFINED}
  {$define CPUDEFINED}
{$endif x86_64}
{$ifdef M68K}
  {$ifdef CPUDEFINED}
    {$fatal ONLY one of the switches for the CPU type must be defined}
  {$endif CPUDEFINED}
  {$define CPUDEFINED}
{$endif M68K}
{$ifdef vis}
  {$ifdef CPUDEFINED}
    {$fatal ONLY one of the switches for the CPU type must be defined}
  {$endif CPUDEFINED}
  {$define CPUDEFINED}
{$endif}
{$ifdef iA64}
  {$ifdef CPUDEFINED}
    {$fatal ONLY one of the switches for the CPU type must be defined}
  {$endif CPUDEFINED}
  {$define CPUDEFINED}
{$endif iA64}
{$ifdef POWERPC}
  {$ifdef CPUDEFINED}
    {$fatal ONLY one of the switches for the CPU type must be defined}
  {$endif CPUDEFINED}
  {$define CPUDEFINED}
{$endif POWERPC}
{$ifdef POWERPC64}
  {$ifdef CPUDEFINED}
    {$fatal ONLY one of the switches for the CPU type must be defined}
  {$endif CPUDEFINED}
  {$define CPUDEFINED}
{$endif POWERPC64}
{$ifdef ALPHA}
  {$ifdef CPUDEFINED}
    {$fatal ONLY one of the switches for the CPU type must be defined}
  {$endif CPUDEFINED}
  {$define CPUDEFINED}
{$endif ALPHA}
{$ifdef SPARC}
  {$ifdef CPUDEFINED}
    {$fatal ONLY one of the switches for the CPU type must be defined}
  {$endif CPUDEFINED}
  {$define CPUDEFINED}
{$endif SPARC}
{$ifdef ARM}
  {$ifdef CPUDEFINED}
    {$fatal ONLY one of the switches for the CPU type must be defined}
  {$endif CPUDEFINED}
  {$define CPUDEFINED}
{$endif ARM}
{$ifdef MIPS}
  {$ifdef CPUDEFINED}
    {$fatal ONLY one of the switches for the CPU type must be defined}
  {$endif CPUDEFINED}
  {$define CPUDEFINED}
{$endif MIPS}
{$ifdef AVR}
  {$ifdef CPUDEFINED}
    {$fatal ONLY one of the switches for the CPU type must be defined}
  {$endif CPUDEFINED}
  {$define CPUDEFINED}
{$endif AVR}
{$ifndef CPUDEFINED}
  {$fatal A CPU type switch must be defined}
{$endif CPUDEFINED}
{$ifdef support_mmx}
  {$ifndef i386}
    {$fatal I386 switch must be on for MMX support}
  {$endif i386}
{$endif support_mmx}

{$ifdef win32}
  { 256 MB stack }
  { under windows the stack can't grow }
  {$MAXSTACKSIZE 256000000}
{$else win32}
  {$ifdef win64}
    { 512 MB stack }
    { under windows the stack can't grow }
    {$MAXSTACKSIZE 512000000}
  {$else win64}
    { 1 MB stack }
    {$MINSTACKSIZE 1000000}
  {$endif win64}
{$endif win32}

uses
{$ifdef cmem}
  cmem,
{$endif cmem}
{$ifdef profile}
  profile,
{$endif profile}
{$ifndef NOCATCH}
  {$if defined(Unix) or defined(Go32v2) or defined(Watcom)}
    catch,
  {$endif}
{$endif NOCATCH}
  globals,compiler,ipputils,comphook,strings,pbase,sysutils;


procedure DoSyntaxCheck(const name: string; const data: pchar);
begin

end;

procedure DoLineLexerCheck(const name: string; const source: pchar);
var
    pscanner: PScannerFile;
    pmodule: PPpuModule;
begin
    pscanner:=nil;
    pmodule:=nil;
    DoLexicalAnalisys(name, source, pscanner, pmodule);
end;

const
    v0: string = '-v0 ';

var
  command: string;
  name: string;
  source: pchar;
  arguments: string;
  unitpaths: array[0..32] of string;
  unitpathscount: integer;
  i: integer;
begin
  do_comment:=@WriteError;
  parse_only:=true;
  InitCompiler(concat(v0,'dummy.pas'));
    
  unitpathscount:=0;
  inputsourcebuffer:=stralloc(1048576);
{$ifdef extheaptrc}
  keepreleased:=true;
  showonlylocalsymbols:=true;
{$endif extheaptrc}
{ Call the compiler with empty command, so it will take the parameters }
{  Halt(compiler.Compile('')); }
 repeat
        status.errorcount:=0;
        write(ErrOutput, 'ippc> ');
        ReadToken(command);
        if (command='check') or (command='checkandgettables') then
        begin
            showonlylocalsymbols:=command='check';
            source:=readSourceFromStdIn;
            arguments:='';
            name:='dummy.pas';
            for i:=0 to unitpathscount-1 do
            begin
                arguments:=Concat(arguments,'-Fu',unitpaths[i],' ');
            end;
            arguments:=Concat(v0, arguments, name);
            try
                DoLineLexerCheck('dummy.pas', source);

            except
              on Exception do
              begin

              end;
            end;
            try
                compiler.Compile(arguments, source);
            except
              on Exception do
              begin

              end;
            end;
            StrDispose(source);
            writeln('end');
            
        end
        else if command='linesyntax' then
        begin
            source:=readSourceFromStdIn;
            try
                DoLineLexerCheck('dummy.pas', source);

            except
              on Exception do
              begin

              end;
            end;
            StrDispose(source);
            writeln('end');
        end
        else if command='addunitpath' then
        begin
             source:=readSourceFromStdIn;
             unitpaths[unitpathscount]:=StrPas(source);
             StrDispose(source);
             unitpathscount:=unitpathscount+1;
        end
        else if command='exit' then
        begin
            writeln(ErrOutput, 'Bye');
            break;
        end
        else if command='' then
        begin
            writeln(ErrOutput);
        end
        else
        begin
            writeln(ErrOutput,'Wrong command: ',command,'(',length(command),')');
        end;
        flush(Output);
        flush(ErrOutput);
    until command='exit';
end.
