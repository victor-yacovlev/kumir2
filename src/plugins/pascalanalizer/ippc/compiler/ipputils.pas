unit ipputils;

{$i fpcdefs.inc}

interface

uses parser, strings, fppu,
  finput, cutils, sysutils, scanner, fmodule,
  tokens, globals, pmodules, verbose, comphook,
  symbase, symtable, cclasses, cfileutl, options, systems, symconst;

type

    TInputBuffer = class(TInputFile)
        public
            constructor create(const fn:string; const data: pchar);
            destructor destroy; override;
        protected
            function fileopen(const filename: string): boolean; override;
            function fileseek(pos: longint): boolean; override;
            function fileread(var databuf; maxsize: longint): longint; override;
            function fileeof: boolean; override;
            function fileclose: boolean; override;
            procedure filegettime; override;

        private
            buffer  : pchar;
            bufferPosition : longint;
    end;

    PScannerFile = ^TScannerFile;
    PPpuModule = ^TPpuModule;

    function descreen(const data: pchar): pchar;
    function readSourceFromStdIn: pchar;
    function WriteError(level: longint; const s: ansistring): boolean;
    procedure DoLexicalAnalisys(
        const name: string;
        const source: pchar;
        var pscanner: PScannerFile;
        var pmodule: PPpuModule
        );
    procedure ReadToken(out token: string);
    procedure PrintSymbolTable(const table: TSymtable; const local: boolean);

var
    inputsourcebuffer: pchar;
    showonlylocalsymbols: boolean;

implementation

{$i fpcdefs.inc}


const
    maxbuffersize: integer = 65536;

{ class TInputBuffer }

constructor TInputBuffer.create(const fn: string; const data: pchar);
begin
name:=stringdup(ExtractFileName(fn));
    path:=stringdup(ExtractFilePath(fn));
    next:=nil;
    filetime:=-1;
  { file info }
    is_macro:=false;
    endoffile:=false;
    closed:=true;
    buf:=nil;
    bufstart:=0;
    bufsize:=0;
    maxbufsize:=InputFileBufSize;
  { save fields }
    saveinputpointer:=nil;
    saveline_no:=0;
    savelastlinepos:=0;
  { indexing refs }
    ref_next:=nil;
    ref_index:=0;
  { line buffer }
    linebuf:=nil;
    maxlinebuf:=0;
    buffer:=data;
    c:=data^;
end;


destructor TInputBuffer.destroy;
begin
end;

function TInputBuffer.fileOpen(const filename: string): boolean;
begin
    closed:=false;
    result:=true;
    bufferPosition:=0;
end;

function TInputBuffer.fileseek(pos: longint): boolean;
begin
    if (pos>=0) and (pos<strlen(buffer)) then
    begin
        bufferPosition:=pos;
        result:=true;
    end else
    begin
        result:=false;
    end;
end;

function TInputBuffer.fileread(var databuf; maxsize: longint): longint;
var
    cnt: longint;
    fragment: pchar;
begin
    cnt:=strlen(buffer)-bufferPosition;
    if cnt>maxsize then cnt:=maxsize;
    result:=cnt;
    fragment:=buffer+bufferPosition;
    strlcopy(buf, fragment, cnt);
    bufferPosition:=bufferPosition+cnt;
end;

function TInputBuffer.fileEOF: boolean;
begin
    result:=bufferPosition=strlen(buffer);
end;

function TInputBuffer.fileClose: boolean;
begin
    closed:=true;
    result:=true;
    bufferPosition:=0;
end;

procedure TInputBuffer.fileGetTime;
begin

end;

{ end class TInputBuffer }

procedure PrintSymbolTable(const table: TSymtable; const local: boolean);
var
   i, cnt: integer;
   name: String;
   item: TSymEntry;
   scope: String;
begin
    cnt:=table.SymList.Count;
    if local then begin scope:='local'; end else begin scope:='uses'; end;
    for i:=0 to cnt-1 do
    begin
         name:=table.SymList.NameOfIndex(i);
         item:=TSymEntry(table.SymList.Find(name));
         if (item.typ=procsym) and (pos('fpc_',name)<>1) then
         begin
            writeln('procedure ', scope, ' ', name)
         end;
         if (item.typ=typesym) then
         begin
            writeln('class ', scope, ' ', name)
         end;
         if (item.typ=unitsym) then
         begin
            writeln('unitname ', scope, ' ', name);
         end;
         flush(output);
    end;
end;

function descreen(const data: pchar): pchar;
var
    c: char;
    i, j: integer;
    maxlen: integer;
    prevslash: boolean;
begin
    prevslash:=false;
    maxlen:=strlen(data);
    descreen:=stralloc(maxlen+1);
    j:=0;
    for i:=0 to maxlen-1 do
    begin
        c:=data[i];
        if prevslash then
        begin
            if c='\' then
            begin
                descreen[j]:='\';
            end;
            if c='n' then
            begin
                descreen[j]:=#10;
            end;
            prevslash:=false;
            j:=j+1;
        end else
            if c='\' then
            begin
                prevslash:=true;
            end else
            begin
                descreen[j]:=c;
                j:=j+1;
            end;
        begin
        end;
    end;
    descreen[j]:=#0;
end;


function readSourceFromStdIn: pchar;
var
    c: char;
    buffersize: integer;
begin
    buffersize:=0;
    repeat
        read(c);
        inputsourcebuffer[buffersize]:=c;
        buffersize:=buffersize+1;
    until (ord(c)=10) or (ord(c)=13) or (buffersize>=maxbuffersize);
    inputsourcebuffer[buffersize-1]:=chr(0);
    readSourceFromStdIn:=descreen(inputsourcebuffer);
    {$ifdef win32}
{    read(c); }
    {$endif}
end;


function WriteError(level: longint; const s: ansistring): boolean;
var
    message: ansistring;
    i, chopindex: integer;
    foundExtra: boolean;
begin
    WriteError:=false; { do not raise an exception }
    if (pos('Free Pascal Compiler', s)<>0) then
    begin
        exit;
    end;
    if (pos('Copyright', s)<>0) then
    begin
        exit;
    end;
    if (status.currentline<>0) then
    begin
        foundExtra:=false;
        chopindex:=length(s);
        for i:=length(s) downto 1 do
        begin
            if s[i]='(' then
            begin
                chopindex:=i;
                foundExtra:=true;
                break;
            end;
        end;
        if foundExtra then    begin
                message:=copy(s,1,chopindex-1);
        end
        else                       begin
            message:=s;
        end;
        WriteLn('error ', status.currentline-1, ' ', status.currentcolumn-1,' ', message);
        Flush(Output);
    end;
end;

procedure ReadToken(out token: string);
var
    c: char;
    pos: integer;
begin
    token:='';
    repeat
        read(c);
        if (c<>' ') and (c<>#10) and (c<>#13) then
        begin
            token:=concat(token, stringofchar(c, 1));
        end;
    until (c=' ') or (c=#10) or (c=#13);
end;

procedure DoLexicalAnalisys(
    const name: string;
    const source: pchar;
    var pscanner: PScannerFile;
    var pmodule: PPpuModule
    );
var
    scanner: TScannerFile;
    module: TPpuModule;
    mostInformative: TToken;
begin
    if pscanner=nil then
    begin
        scanner:=TScannerFile.create(name);
        scanner.inputfile:=TInputBuffer.create(name, source);
        scanner.firstFile;
    end
    else
    begin
        scanner:=pscanner^;
    end;
    if pmodule=nil then
    begin
        module:=TPpuModule.create(nil,name,'',false);
    end
    else
    begin
        module:=pmodule^;
    end;
    set_current_module(module);

    { Do lexical analisys for syntax highlighter }
    try
        repeat
              pattern:='';
              scanner.readtoken(false);
              if token=_EOF then break;
              write('token ');
              write(current_filepos.line-1);
              write(' ');
              write(current_filepos.column-1);
              write(' ');
              if length(pattern)>0 then
              begin
                  write(length(pattern));
              end
              else
              begin
                  write('1');
              end;
              write(' ');
              mostInformative:=idtoken;
              if ( (idtoken=_ID) or (idtoken=_NOID) ) and ( (token<>_ID) and (token<>_NOID) ) then
                 mostInformative:=token;
              case mostInformative of
                        _ID:
                          writeln('name');
                        _PLUS, _MINUS, _STAR, _SLASH, _EQUAL, _GT, _LT,
                        _GTE, _LTE, _SYMDIF, _STARSTAR, _ASSIGNMENT,
                        _CARET, _UNEQUAL, _POINT,
                        _COMMA, _LKLAMMER, _RKLAMMER, _COLON, _SEMICOLON,
                        _KLAMMERAFFE, _POINTPOINT, _POINTPOINTPOINT, _PIPE,
                        _AMPERSAND,
                        _PLUSASN, _MINUSASN, _ANDASN, _ORASN, _STARASN,
                        _SLASHASN, _MODASN, _DIVASN, _NOTASN, _XORASN:
                          writeln('operator');
                        _OP_AS, _OP_IN, _OP_IS, _OP_AND, _OP_DIV, _OP_MOD,
                        _OP_NOT, _OP_SHL, _OP_SHR, _OP_XOR, _AS, _DO,
                        _IN, _OF, _ON, _OR, _TO, _AND, _DIV, _FAR, _MOD,
                        _NOT, _OUT, _SET, _SHL, _SHR, _XOR:
                          writeln('seckeyword');
                        _PROGRAM, _UNIT, _USES, _LIBRARY, _PROCEDURE, _FUNCTION,
                        _VAR, _TYPE, _CONST,_INTERFACE, _IMPLEMENTATION:
                          writeln('prikeyword -1 +1');
                        _BEGIN, _CLASS, _OBJECT, _RECORD:
                          writeln('prikeyword 0 +1');
                        _END:
                          writeln('prikeyword -1 0');
                        _FOR, _IF:
                            writeln('prikeyword 0 0');
                        else
                          writeln('unknown');
              end;
              Flush(Output);
        until false;
    except
        on ECompilerAbort do
        begin
            inc(status.errorcount);
        end;
        on Exception do
        begin
            inc(status.errorcount);
        end;
    end;

    {
    if pscanner=nil then
    begin
        scanner.inputfile.destroy;
        scanner.destroy;
    end;
    if pmodule=nil then
    begin
        set_current_module(nil);
        module.destroy;
    end;
    }
end;

end.

