@echo off
setlocal
call "C:\Users\logsa\Documents\Codex\BuildTools2022\VC\Auxiliary\Build\vcvars32.bat"
pushd "%~dp0"
cl /nologo /EHsc /O2 /MT /W4 fixture.cpp /Fediagnostics-fixture.exe
exit /b %errorlevel%
