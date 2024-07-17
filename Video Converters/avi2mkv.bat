@echo off
REM Loop through each AVI file in the current directory
for %%a in (*.avi) do (
    ffmpeg -i "%%~a" -c:v copy -c:a copy "%%~na.mkv"
)
