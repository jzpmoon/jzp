@echo off
setlocal enabledelayedexpansion
set prefix=%cd%
set envc=win
set thw=x86_64
set currdir=%cd%
set mk=makefile_m

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

echo prefix=%prefix% > %mk%
echo envc=%envc% >> %mk%
echo thw=%thw% >> %mk%
echo currdir=%currdir% >> %mk%
echo !include ..\u\env\env_%envc%.mk >> %mk%
type liblm.win.mk >> %mk%

cd ..\v\
call .\configure.bat --prefix=%prefix% --envc=%envc% --thw=%thw%
cd %currdir%

echo /*auto create*/ > lmmacro.h
for /f "delims=" %%i in (..\u\uexport.h) do (
    set tmp=%%i
    set tmp=!tmp:_EXPORT_XXX_=_EXPORT_LIBLM_!
	set tmp=!tmp:_NOTEXPORT_XXX_=_NOTEXPORT_LIBLM_!
    set tmp=!tmp:_UEXPORT_H_=_UEXPORT_LIBLM_H_!
    set tmp=!tmp:xapi=lmapi!
    set tmp=!tmp:xcall=lmcall!
    echo !tmp! >> lmmacro.h
)
endlocal
