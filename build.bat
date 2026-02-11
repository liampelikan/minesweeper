@echo off
setlocal enabledelayedexpansion

echo Finding build tools...

:: Define paths
set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools"
set "VCVARS=!VS_PATH!\VC\Auxiliary\Build\vcvarsall.bat"
set "CMAKE_BIN=!VS_PATH!\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"

:: Check if tools exist
if not exist "!VCVARS!" (
    echo Error: Could not find vcvarsall.bat at !VCVARS!
    echo Please ensure Visual Studio 2019 BuildTools is installed correctly.
    pause
    exit /b 1
)

if not exist "!CMAKE_BIN!" (
    echo Error: Could not find cmake.exe at !CMAKE_BIN!
    echo Please ensure the CMake component is installed in Visual Studio.
    pause
    exit /b 1
)

echo Tools found! Setting up environment...

:: Set up MSVC environment
call "!VCVARS!" x64

:: Move to build directory
if not exist build mkdir build
cd build

echo Running CMake...
"!CMAKE_BIN!" .. -G "Visual Studio 16 2019" -A x64
if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed.
    pause
    exit /b %ERRORLEVEL%
)

echo Building...
"!CMAKE_BIN!" --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo Build failed.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo ==================================================
echo Build successful! 
echo Game is located at: build\Release\Minesweeper-Raylib.exe
echo ==================================================
cd ..
pause
