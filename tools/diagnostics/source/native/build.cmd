@echo off
setlocal
call "C:\Users\logsa\Documents\Codex\BuildTools2022\VC\Auxiliary\Build\vcvars32.bat"
if errorlevel 1 exit /b 1
pushd "%~dp0"
cl /nologo /std:c++17 /EHsc /O2 /MT /W4 crash_recorder.cpp /Fecrash-recorder.exe /link bcrypt.lib dbghelp.lib user32.lib
exit /b %errorlevel%
