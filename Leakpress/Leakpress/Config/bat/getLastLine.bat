@echo off

set filePath=config.ini

for /f "delims=" %%i in (%filePath%) do (
    set lastLine=%%~i
)

echo %lastLine%

pause