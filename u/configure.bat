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
echo currdir=%currdir% >> makefile
echo !include ..\u\env\env_%envc%.mk >> makefile

type libu.win.mk >> makefile

echo /*auto create*/ > umacro.h

type .\env\env_%envc%.h > umacro.h

type .\env\thw_%thw%.h >> umacro.h

for /f "delims=" %%i in (.\uexport.h) do (
    set tmp=%%i
    set tmp=!tmp:_EXPORT_XXX_=_EXPORT_LIBU_!
	set tmp=!tmp:_NOTEXPORT_XXX_=_NOTEXPORT_LIBU_!
    set tmp=!tmp:_UEXPORT_H_=_UEXPORT_LIBU_H_!
    set tmp=!tmp:xapi=uapi!
    set tmp=!tmp:xcall=ucall!
    echo !tmp! >> umacro.h
)
endlocal