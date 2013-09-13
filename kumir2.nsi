!include MUI2.nsh
 
# This include is used only by build system
# For manual build change this line to something line:
#  OutFile "kumir2-...exe"
!include outfilename.nsh

Name "Кумир-2.x"
InstallDir "$PROGRAMFILES\Kumir2x"
RequestExecutionLevel admin


!insertmacro MUI_PAGE_LICENSE "LICENSE_RU.rtf"

!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_LANGUAGE "Russian"

Section "Kumir" Kumir

	SetOutPath "$INSTDIR"
	File LICENSE_RU.rtf
	SetOutPath "$INSTDIR\bin"
	File /r bin\*
	SetOutPath "$INSTDIR\lib"
	File /r lib\*
	SetOutPath "$INSTDIR\share"
	File /r share\*

	CreateDirectory "$SMPROGRAMS\Кумир2"
	CreateShortCut "$SMPROGRAMS\Кумир2\Кумир-Стандарт.lnk" "$INSTDIR\bin\kumir2-classic.exe"
	CreateShortCut "$SMPROGRAMS\Кумир2\Кумир для старших классов.lnk" "$INSTDIR\bin\kumir2-highgrade.exe"
	CreateShortCut "$SMPROGRAMS\Кумир2\Кумир-Про.lnk" "$INSTDIR\bin\kumir2-ide.exe"
	CreateShortCut "$SMPROGRAMS\Кумир2\Кумир для учителей.lnk" "$INSTDIR\bin\kumir2-teacher.exe"
	
SectionEnd