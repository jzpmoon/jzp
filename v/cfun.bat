@echo off
setlocal enabledelayedexpansion
set cfun=
set out=_temp.cfun
set callback=_cfun_init

:loop
	shift
	if "%0%" == "" (
		goto end
	) else if "%0%" == "--cfun" (
		set cfun=%1
	) else if "%0%" == "--out" (
		set out=%1
	) else if "%0%" == "--callback" (
		set callback=%1
	)
	goto loop
:end

if %cfun% == "" goto exit
echo %cfun%
echo #include "vcfun.h" > %out%
type %cfun:-=.cfun %.cfun >> %out%
echo void %callback%(vcontext* ctx,vmod* mod){_vcfun_file_init_%cfun:-=(ctx,mod);_vcfun_file_init_%(ctx,mod);} >> %out%

:exit

endlocal