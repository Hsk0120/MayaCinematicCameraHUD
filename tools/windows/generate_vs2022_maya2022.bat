@ECHO OFF
@REM Generate Visual Studio 2022 project for Maya 2022

CD /d "%~dp0\..\.."

SET "MAYA_VERSION=2022"

SET "DEVKIT_LOCATION=C:/Program Files/Autodesk/Maya%MAYA_VERSION%"

SET "CMAKE_EXE=C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"

ECHO Running CMake for Maya %MAYA_VERSION%...
"%CMAKE_EXE%" "." -B "build_%MAYA_VERSION%" -G "Visual Studio 17 2022" -DCMAKE_CONFIGURATION_TYPES="Release;RelWithDebInfo"

PAUSE
