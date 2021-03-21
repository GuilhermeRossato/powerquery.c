@echo off
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
cl /nologo /DWIN32 /D_WINDOWS /MD /Ob0 /O2 main.c && echo Compiled Successfully || pause
pause