@echo off
echo Windows Registry Editor Version 5.00 > ffdshow.reg
echo. >> ffdshow.reg
echo [HKEY_LOCAL_MACHINE\SOFTWARE\GNU\ffdshow] >> ffdshow.reg
set FFPATH=%~dp0%
echo "pthPriority"="%FFPATH:\=\\%" >> ffdshow.reg
regedit /s ffdshow.reg
start regsvr32 %FFPATH%ffdshow.ax /s
del ffdshow.reg