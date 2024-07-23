@echo off
cd %~dp0

:: Set your path to your ucrt64/bin folder

set path=D:\devel\msys64\ucrt64\bin;'%path%


g++ convert.cpp -o ..\build\convert.dll
echo conv...
pause >nul
g++ split.cpp -o ..\build\split.dll
echo split...
pause >nul
g++ main.cpp -o ..\build\tagger.exe
echo app...
pause >nul