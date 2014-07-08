@echo off
REM You must have yasm.exe in a directory pointed by PATH.
REM Source code path must not include space.

if "%1" == "Win32" (
    yasm.exe -f win32 -DPREFIX -DWIN32 -DHAVE_SSE3 -DUSE32 -Dalloc -Dexec %2 -o %3 %4

) else (
    yasm.exe -f win64 -DWIN64 %2 -o %3 %4
)
