@echo off

CD /d "%~dp0\..\.."

REM ★ CMake ジェネレータを環境に合わせて設定してください
set "CMAKE_GENERATOR=Visual Studio 18"
SET "CMAKE_EXE=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"

REM === Maya 2022 ===
call :build_maya 2022 "C:\Program Files\Autodesk\Maya2022\devkit"

REM === Maya 2024 ===
call :build_maya 2024 "C:\Program Files\Autodesk\Maya2024\devkit"

REM === Maya 2026 ===
call :build_maya 2026 "C:\Program Files\Autodesk\Maya2026\devkit"

echo.
echo ==== All builds completed. ====
pause
exit /b


:build_maya
setlocal

set "MAYA_VER=%1"
set "DEVKIT=%2"

echo.
echo ==== Building for Maya %MAYA_VER% ====
echo Devkit: %DEVKIT%

set "BUILD_DIR=build_maya_%MAYA_VER%"

REM CMake configure / generate
"%CMAKE_EXE%" -S . -B "%BUILD_DIR%" -G "%CMAKE_GENERATOR%" ^
    -DMAYA_VERSION=%MAYA_VER% ^
    -DDEVKIT_LOCATION="%DEVKIT%"

IF ERRORLEVEL 1 (
    echo [ERROR] CMake configure failed for Maya %MAYA_VER%
    endlocal & exit /b 1
)

REM Release でビルド
"%CMAKE_EXE%" --build "%BUILD_DIR%" --config Release

IF ERRORLEVEL 1 (
    echo [ERROR] Build failed for Maya %MAYA_VER%
    endlocal & exit /b 1
)

echo ==== Done: Maya %MAYA_VER% ====
endlocal & exit /b 0
