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
type liblmod.win.mk >> makefile

cd ..\v\
call .\configure.bat --prefix=%prefix% --envc=%envc% --thw=%thw%
cd %currdir%

echo /*auto create*/ > lmmacro.h
for /f "delims=" %%i in (..\u\uexport.h) do (
    set tmp=%%i
    set tmp=!tmp:_EXPORT_XXX_=_EXPORT_LIBM_!
	set tmp=!tmp:_NOTEXPORT_XXX_=_NOTEXPORT_LIBM_!
    set tmp=!tmp:_UEXPORT_H_=_UEXPORT_LIBM_H_!
    set tmp=!tmp:xapi=lmapi!
    set tmp=!tmp:xcall=lmcall!
    echo !tmp! >> lmmacro.h
)
endlocal