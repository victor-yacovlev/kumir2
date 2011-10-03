library freepascal;

uses strings, cutils,cclasses, parser,
    globtype,version,tokens,systems,globals,verbose,switches,
    symbase,symtable,symdef,symsym,
    finput,fmodule,fppu,
    aasmbase,aasmtai,aasmdata,
    cgbase,
    script,gendef,
    comphook,
    scanner,scandir,
    pbase,ptype,psystem,pmodules,psub,ncgrtti,htypechk,
    cresstr,cpuinfo,procinfo;


{$i fpcdefs.inc}
type
    PSyntaxCheckError = ^TSyntaxCheckError;
    TSyntaxCheckError = record
	line: Integer; { Starting from 0 }
	col:  Integer; { Starting from 0 }
	text: String;  
	next: PSyntaxCheckError;
    end;



procedure doInit; cdecl;
begin
    initParser;
end;

function doSyntaxCheck(const data: PChar): PSyntaxCheckError; cdecl;
var
    scanner: TScannerFile;
    current_module: TPpuModule;
begin
    doSyntaxCheck:=nil;
    scanner:=TScannerFile.create('');
    scanner.inputBuffer:=data;
    scanner.inputPointer:=data;
    scanner.inputStart:=0;
    scanner.line_no:=0;
    scanner.lastLinePos:=0;
    scanner.lastTokenPos:=0;
    scanner.nextTokenPos:=0;
    set_current_module(TPpuModule.create(nil,'','',false));
end;


exports
    doSyntaxCheck;
exports
    doInit name 'initFreePascal';

begin
end.