@echo off
setlocal enabledelayedexpansion
set file=config.ini
set file_tmp=config.tmp
set file_bak=config.bak

set key=DEBUG
set value=3


(for /f "delims=" %%i in (%file%) do (
  for /f "tokens=1* delims==" %%o in ("%%~i") do (
   if /i "%%o"=="%key%" (echo.%%o=%value%) else (echo.%%i)
) 
)) > %file_tmp%


copy "%file%" "%file_bak%" >nul 2>nul
move "%file_tmp%" "%file%"

pause