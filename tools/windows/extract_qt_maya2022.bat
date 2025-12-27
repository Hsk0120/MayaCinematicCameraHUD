@ECHO OFF
REM Maya 2022 Qt archives extraction
REM This batch file must be run as Administrator

ECHO Extracting Maya 2022 Qt archives...
ECHO This requires Administrator privileges!
ECHO.

SET "MAYA_DIR=C:\Program Files\Autodesk\Maya2022"

REM Check if running as Administrator
NET SESSION >nul 2>&1
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: This script must be run as Administrator!
    ECHO Right-click this file and select "Run as administrator"
    PAUSE
    EXIT /B 1
)

REM Extract Qt cmake files to lib/cmake
ECHO Extracting qt_5.15.2_vc14-cmake.zip to lib/cmake...
powershell -Command "Expand-Archive -Path '%MAYA_DIR%\cmake\qt_5.15.2_vc14-cmake.zip' -DestinationPath '%MAYA_DIR%\lib\cmake' -Force"

REM Extract Qt include files to include
ECHO Extracting qt_5.15.2_vc14-include.zip to include...
powershell -Command "Expand-Archive -Path '%MAYA_DIR%\include\qt_5.15.2_vc14-include.zip' -DestinationPath '%MAYA_DIR%\include' -Force"

REM Extract Qt mkspecs files to mkspecs
ECHO Extracting qt_5.15.2_vc14-mkspecs.zip to mkspecs...
powershell -Command "Expand-Archive -Path '%MAYA_DIR%\mkspecs\qt_5.15.2_vc14-mkspecs.zip' -DestinationPath '%MAYA_DIR%\mkspecs' -Force"

ECHO.
ECHO Qt extraction completed!
ECHO.

PAUSE
