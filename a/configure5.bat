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

echo prefix=%prefix% > makefile
echo envc=%envc% >> makefile
echo thw=%thw% >> makefile
echo currdir=%currdir% >> makefile
echo !include ..\u\env\env_%envc%.mk >> makefile
type l5.win.mk >> makefile

cd ..\l\
call .\configure5.bat --prefix=%prefix% --envc=%envc% --thw=%thw%
call .\configurem.bat --prefix=%prefix% --envc=%envc% --thw=%thw%
cd %currdir%

endlocal