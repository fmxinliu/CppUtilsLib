@echo off
call "%VS100COMNTOOLS%vsvars32.bat"

if exist deploy  rmdir deploy /q /s

cd Leakpress

:MSBuild /t:Clean;Rebuild /p:Configuration=Release /m
msbuild -t:clean -p:configuration="release" -m -t:rebuild
@if not "%errorlevel%"=="0" goto error

cd ..
mkdir deploy

xcopy Leakpress\Release\*.pdb deploy\*.pdb                                  /C /I /R /K /Y
xcopy Leakpress\Leakpress\Config\*.ini deploy\*.ini                         /C /I /R /K /Y
xcopy Leakpress\Leakpress\DllTool\dll\*.dll deploy\*.dll                    /C /I /R /K /Y
xcopy Leakpress\Leakpress\DllTool\dll\VC100.CRT\release\*.dll deploy\*.dll  /C /I /R /K /Y
 copy Leakpress\Release\Leakpress.exe deploy\Leakpress.exe                  /Y
@if "%errorlevel%"=="0" goto end

:error
@echo === build fail ===

:end
pause