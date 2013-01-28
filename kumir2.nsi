; Use modules from:
; http://nsis.sourceforge.net/Setting_Environment_Variables
; http://nsis.sourceforge.net/File_Association
!include EnvVarUpdate.nsh 
!include FileAssociation.nsh

Name "Кумир 2.0.0"

OutFile "kumir2-2.0.0-win32-setup.exe"

InstallDir $PROGRAMFILES32\kumir2

RequestExecutionLevel admin

Page license
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

LicenseText "Лицензионное соглашение"
LicenseData "license.cp1251.txt"

Section "Компилятор (kumir2-bc)"
  SetOutPath $INSTDIR\bin  
  File bin\*.dll
  File bin\kumir2-bc.exe
  SetOutPath $INSTDIR
  File /r lib
  File /r share  
SectionEnd

Section "Выполнитель (kumir2-run)"
  SetOutPath $INSTDIR\bin
  File bin\kumir2-run.exe
  ${EnvVarUpdate} $0 "PATHEXT" "A" "HKLM" ".kod"
  ${registerExtension} "$INSTDIR\bin\kumir2-run.exe" ".kod" "KumirExecutable"
SectionEnd

; Установка зависимости: VC2010 C/C++ Runtime
Section # hidden
  SetOutPath $INSTDIR
  File vcredist_x86_2010.exe 
  ExecWait '"$INSTDIR\vcredist_x86_2010.exe" /q'  
  Delete $INSTDIR\vcredist_x86_2010.exe
  WriteUninstaller "$INSTDIR\kumi2-uninst.exe"
  ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR\bin"
SectionEnd

Section "Uninstall"
  RMDir /r /REBOOTOK $INSTDIR
  ${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "$INSTDIR\bin"
  ${un.EnvVarUpdate} $0 "PATHEXT" "R" "HKLM" ".KOD"
  ${unregisterExtension} ".kod" "KumirExecutable"
SectionEnd
