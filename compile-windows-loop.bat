@echo off
title Setting environment
SET "ENVSCRIPT=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
if not exist "%ENVSCRIPT%" (
	echo Error: Missing Microsoft Visual Studio 2019 enviroment 'vcvars64' script
	echo You may try to compile it but 'cl.exe' might fail
	pause
) else (
	call "%ENVSCRIPT%"
)
WHERE cl
IF %ERRORLEVEL% NEQ 0 (
	echo Error: Could not find 'cl.exe', the Microsoft C/C++ Optimizing Compiler used by this script to compile
	echo Ensure it is installed and available at path
	pause
	exit
)
:start
title Compiling and Executing
cls
cl /nologo /DWIN32 /D_WINDOWS /MD /Ob0 /O2 main.c && echo. && main.exe
title Waiting for Key to Recompile
echo.
pause
goto start