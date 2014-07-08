; register.nsi
; After build registration helper for Windows Vista.
;
; Here's register.bat for Windows Vista
;
; @echo off
; REM compile bin/register.nsi to get register.exe
; start %~dp0%register.exe /S


!packhdr "delvcr80.dat" "upx --best delvcr80.dat"

!verbose 3

Name "register"

!include "Sections.nsh"
!include "LogicLib.nsh"
!include "StrFunc.nsh"
!include "Library.nsh"

;General
!define FULLNAME register
OutFile "${FULLNAME}.exe"
Caption "${FULLNAME}"

!ifndef COMPRESSION
  !define COMPRESSION lzma
!endif
SetCompressor /SOLID ${COMPRESSION}

;--------------------------------
;Installer Sections
Section register Sec_register
 WriteRegStr HKLM SOFTWARE\GNU\ffdshow "pthPriority" $EXEDIR
 RegDll $EXEDIR\ffdshow.ax
 IfErrors 0 SucceededReg
 MessageBox MB_OK|MB_ICONEXCLAMATION "Error registering ffdshow.ax"
SucceededReg:
SectionEnd
