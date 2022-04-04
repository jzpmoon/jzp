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
type libv.win.mk >> makefile

cd ..\u\
call .\configure.bat --prefix=%prefix% --envc=%envc% --thw=%thw%
cd %currdir%

echo /*auto create*/ > vmacro.h
for /f "delims=" %%i in (..\u\uexport.h) do (
    set tmp=%%i
    set tmp=!tmp:_EXPORT_XXX_=_EXPORT_LIBV_!
	set tmp=!tmp:_NOTEXPORT_XXX_=_NOTEXPORT_LIBV_!
    set tmp=!tmp:_UEXPORT_H_=_UEXPORT_LIBV_H_!
    set tmp=!tmp:xapi=vapi!
    set tmp=!tmp:xcall=vcall!
    echo !tmp! >> vmacro.h
)
endlocal