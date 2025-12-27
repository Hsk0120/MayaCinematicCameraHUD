@ECHO OFF
REM Maya 2024 Qt archives extraction
REM This batch file must be run as Administrator

ECHO Extracting Maya 2024 Qt archives...
ECHO This requires Administrator privileges!
ECHO.

SET "MAYA_DIR=C:\Program Files\Autodesk\Maya2024"

REM Check if running as Administrator
NET SESSION >nul 2>&1
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: This script must be run as Administrator!
    ECHO Right-click this file and select "Run as administrator"
    PAUSE
    EXIT /B 1
)

REM Extract Qt cmake files to lib/cmake
ECHO Extracting qt cmake files to lib/cmake...
powershell -Command "Expand-Archive -Path '%MAYA_DIR%\cmake\qt*.zip' -DestinationPath '%MAYA_DIR%\lib\cmake' -Force"

REM Extract Qt include files to include
ECHO Extracting qt include files to include...
powershell -Command "Expand-Archive -Path '%MAYA_DIR%\include\qt*.zip' -DestinationPath '%MAYA_DIR%\include' -Force"

REM Extract Qt mkspecs files to mkspecs
ECHO Extracting qt mkspecs files to mkspecs...
powershell -Command "Expand-Archive -Path '%MAYA_DIR%\mkspecs\qt*.zip' -DestinationPath '%MAYA_DIR%\mkspecs' -Force"

ECHO.
ECHO Qt extraction completed!
ECHO.

PAUSE
