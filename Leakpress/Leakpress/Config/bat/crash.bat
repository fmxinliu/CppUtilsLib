@echo off
if not exist "log" (
    mkdir log
)

call debug.bat
echo �����ռ���...
Leakpress.exe >> log\1.TXT