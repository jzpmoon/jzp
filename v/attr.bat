@echo off
set attr=
set out=_temp.attr
set callback=_attr_init

:loop
	shift
	if "%0%" == "" (
		goto end
	) else if "%0%" == "--attr" (
		set attr=%1
	) else if "%0%" == "--out" (
		set out=%1
	) else if "%0%" == "--callback" (
		set callback=%1
	)
	goto loop
:end

if %attr% == "" goto exit

echo #include "vparser.h" > %out%
type %attr: =.attr%.attr >> %out%
echo void %callback%(vreader* reader){_vattr_file_init_%attr: =(reader);_vattr_file_init_%(reader);} >> %out%

:exit