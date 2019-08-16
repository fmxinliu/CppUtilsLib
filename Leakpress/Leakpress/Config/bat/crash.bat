@echo off
if not exist "log" (
    mkdir log
)

call debug.bat
echo 数据收集中...
Leakpress.exe >> log\1.TXT