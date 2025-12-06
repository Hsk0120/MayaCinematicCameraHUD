@ECHO OFF
@REM REM Generate Visual Studio project using CMake

CD /d "%~dp0\..\.."

SET "CMAKE_EXE=C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"

ECHO Running CMake...
"%CMAKE_EXE%" "." -B build -G "Visual Studio 17 2022"

PAUSE