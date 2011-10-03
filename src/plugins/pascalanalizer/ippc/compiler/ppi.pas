program pp;

{$i fpcdefs.inc}

uses parser, strings, fppu, finput, cutils, sysutils, scanner, fmodule, tokens, globals, pmodules;

const
    maxbuffersize: integer = 65536;


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
    i: longint;
    cnt: longint;
    fragment: pchar;
begin
    cnt:=strlen(buffer)-bufferPosition;
    if cnt>maxsize then cnt:=maxsize;
    result:=cnt;
    fragment:=buffer+bufferPosition;
    strlcopy(buf, fragment, cnt);
    bufferPosition+=cnt;
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
            j+=1;
        end else
            if c='\' then
            begin
                prevslash:=true;
            end else
            begin
                descreen[j]:=c;
                j+=1;
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
    buffer: pchar;
begin
    buffer:=stralloc(maxbuffersize);
    buffersize:=0;
    repeat
        read(c);
        buffer[buffersize]:=c;
        buffersize+=1;
    until (ord(c)=10) or (ord(c)=13) or (buffersize>=maxbuffersize);
    buffer[buffersize-1]:=chr(0);
    readSourceFromStdIn:=descreen(buffer);
    strdispose(buffer);
end;



procedure doSyntaxCheck(const name: string; const source: pchar);
var
    scanner: TScannerFile;
    mostInformative: TToken;
begin
    InitScanner;
    InitGlobals;
    scanner:=TScannerFile.create(name);
    scanner.inputfile:=TInputBuffer.create(name, source);
    scanner.firstFile;
    set_current_module(TPpuModule.create(nil,name,'',false));
    { Do lexical analisys for syntax highlighter }
    repeat
          scanner.readtoken(false);
          if token=_EOF then break;
          write('token ');
          write(current_tokenpos.line);
          write(' ');
          write(current_tokenpos.column);
          write(' ');
          mostInformative:=idtoken;
          if (idtoken=_ID) and (token<>_ID) then
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
                    _PROGRAM, _UNIT, _LIBRARY:
                      writeln('prikeyword 0 0');
                    _PROCEDURE, _FUNCTION, _VAR, _TYPE, _CONST, _BEGIN,
                    _INTERFACE, _IMPLEMENTATION:
                      writeln('prikeyword -1 +1');
                    _END:
                      writeln('prikeyword -1 0');
                    else
                      writeln('unknown');
          end;
    until false;
    set_current_module(nil);
    scanner.inputfile.destroy;
    scanner.destroy;
    DoneScanner;
    DoneGlobals;
end;

var
    command: string;
    name: string;
    source: pchar;
begin
    inittokens;
    repeat
        readln(command);
        if command='check' then
        begin
            readln(name);
            source:=readSourceFromStdIn;
            doSyntaxCheck(name, source);
            strdispose(source);
            readln();
        end else
        if command='exit' then
        begin
        end else
        begin
            writeln('Wrong command');
        end;
    until command='halt';
end.
