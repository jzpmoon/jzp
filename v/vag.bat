@echo off
setlocal enabledelayedexpansion
set prefix=%cd%
set envc=win
set thw=x86_64
set smk=vag.win.mk
set dmk=makefile
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
	) else if "%0%" == "--smk" (
		set smk=%1
	) else if "%0%" == "--dmk" (
		set dmk=%1
	)
	goto loop
:end

echo prefix=%prefix% > %dmk%
echo envc=%envc% >> %dmk%
echo thw=%thw% >> %dmk%
echo currdir=%currdir% >> %dmk%
echo !include ..\u\env\env_%envc%.mk >> %dmk%

type %smk% >> %dmk%

endlocal