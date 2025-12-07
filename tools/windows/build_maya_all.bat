@echo off
setlocal ENABLEDELAYEDEXPANSION

CD /d "%~dp0\..\.."

SET "CMAKE_GENERATOR=Visual Studio 18"
SET "CMAKE_EXE=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"

for %%V in (2022 2024 2026) do (
  if %%V == 2022 (
    SET "TOOLSET=v142"
  )

  if %%V == 2024 (
    SET "TOOLSET=v143"
  )

  if %%V == 2026 (
    SET "TOOLSET=v145"
  )

  ECHO ==== Building for Maya %%V with toolset "!TOOLSET!" ====

  SET "DEVKIT_LOCATION=C:/Program Files/Autodesk/Maya%%V"

  "%CMAKE_EXE%" -S . -B build_%%V -G "%CMAKE_GENERATOR%" -T "!TOOLSET!" -DMAYA_VERSION=%%V
  "%CMAKE_EXE%" --build build_%%V --config Release
  
)
pause