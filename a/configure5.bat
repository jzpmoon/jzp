@echo off
setlocal enabledelayedexpansion
set prefix=%cd%
set envc=win
set thw=x86_64
set currdir=%cd%

:loop
	shift
	if "%0%" == "" (
		goto end
	) else if "%0%" == "--prefix" (
		set prefix=%1
	) else if "%0%" == "--envc" (
		set envc=%1
	) else if "%0%" == "--thw" (
		set thw=%1
	)
	goto loop
:end

set entry_exec=l5.bat
echo @echo off > %entry_exec%
echo %prefix%\l5 %%* --self-path %prefix%\ --conf l5.win.conf >> %entry_exec%

echo prefix=%prefix% > makefile
echo envc=%envc% >> makefile
echo thw=%thw% >> makefile
echo currdir=%currdir% >> makefile
echo entry_exec=%entry_exec% >> makefile
echo !include ..\u\env\env_%envc%.mk >> makefile
type l5.win.mk >> makefile

cd ..\l\
call .\configure5.bat --prefix=%prefix% --envc=%envc% --thw=%thw%
call .\configurem.bat --prefix=%prefix% --envc=%envc% --thw=%thw%
cd %currdir%

endlocal