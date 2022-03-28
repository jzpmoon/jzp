@echo off
set prefix=%cd%
set envc=win
set thw=x86_64
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
echo currdir=%cd% >> makefile
echo !include ..\u\env\env_%envc%.mk >> makefile

type libu.win.mk >> makefile

type .\env\env_%envc%.h > umacro.h

type .\env\thw_%thw%.h >> umacro.h
