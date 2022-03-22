@echo off
echo.
echo Starting procedure
echo.

set QT_VERSION_PATH=6.2.3
set OUTPUT_PATH=CPSMGestionPlongees_exe_dev

@REM Currently in root/DevTools/
@REM go up to the git repo root where the build folder is
@REM then to to release build folder
cd .\..\build-*release*

rmdir -S -Q %OUTPUT_PATH%
mkdir %OUTPUT_PATH%
copy .\*.exe .\%OUTPUT_PATH%\
cd %OUTPUT_PATH%
C:\Qt\%QT_VERSION_PATH%\mingw_64\bin\windeployqt.exe --compiler-runtime CPSMGestionPlongees.exe

