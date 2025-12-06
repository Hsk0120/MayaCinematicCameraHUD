@echo off

CD /d "%~dp0\..\.."

SET "CMAKE_GENERATOR=Visual Studio 18"
SET "CMAKE_EXE=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"

for %%V in (2022 2024 2026) do (
  "%CMAKE_EXE%" -S . -B build_%%V -G "%CMAKE_GENERATOR%" -DMAYA_VERSION=%%V -DDEVKIT_LOCATION="C:\Program Files\Autodesk\Maya%%V\devkit"
  "%CMAKE_EXE%" --build build_%%V --config Release
)
pause