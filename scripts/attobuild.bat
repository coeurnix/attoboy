@echo off
setlocal enabledelayedexpansion

REM ============================================================================
REM attobuild.bat - Build script for attoboy library programs
REM ============================================================================
REM This script compiles a single .cpp file using the attoboy library.
REM It automatically detects MSVC or MinGW GCC and uses appropriate flags.
REM ============================================================================

REM --- Configuration (modify these if your directory structure differs) ------
REM %~dp0 = directory where this batch file is located
set "SCRIPT_DIR=%~dp0"
set "INCLUDE_DIR=%SCRIPT_DIR%include"
set "LIB_DIR=%SCRIPT_DIR%lib"
set "HEADER_FILE=%SCRIPT_DIR%include\attoboy\attoboy.h"
set "MSVC_LIB=%SCRIPT_DIR%lib\attoboy.lib"
set "MINGW_LIB=%SCRIPT_DIR%lib\libattoboy.a"
REM ----------------------------------------------------------------------------

REM Check if a .cpp file was provided
if "%~1"=="" (
    echo Error: No input file specified.
    echo Usage: attobuild.bat ^<source.cpp^>
    echo.
    echo Example:
    echo   attobuild.bat my_program.cpp
    exit /b 1
)

set "INPUT_FILE=%~1"

REM Check if input file exists
if not exist "%INPUT_FILE%" (
    echo Error: Input file "%INPUT_FILE%" not found.
    exit /b 1
)

REM Check if input file has .cpp extension
if /i not "%INPUT_FILE:~-4%"==".cpp" (
    echo Error: Input file must have a .cpp extension.
    echo Provided: %INPUT_FILE%
    exit /b 1
)

REM Verify we're in the right directory (check for required files)
if not exist "%HEADER_FILE%" (
    echo Error: Cannot find %HEADER_FILE%
    echo This script must be run from the attoboy distribution directory
    echo containing include/attoboy/attoboy.h and lib/
    exit /b 1
)

if not exist "%LIB_DIR%" (
    echo Error: Cannot find %LIB_DIR% directory
    echo This script must be run from the attoboy distribution directory
    exit /b 1
)

REM Determine output executable name (replace .cpp with .exe)
set "OUTPUT_FILE=%~n1.exe"

REM Check which compiler to use (prefer MSVC if available and lib exists)
set "USE_MSVC=0"
set "USE_MINGW=0"

REM Check for MSVC (use 'where' to avoid cl.exe hanging on /?)
where cl.exe >nul 2>&1
if !errorlevel! equ 0 (
    if exist "%MSVC_LIB%" (
        set "USE_MSVC=1"
    )
)

REM Check for MinGW if MSVC not available
if !USE_MSVC! equ 0 (
    where gcc.exe >nul 2>&1
    if !errorlevel! equ 0 (
        if exist "%MINGW_LIB%" (
            set "USE_MINGW=1"
        )
    )
)

REM Error if no suitable compiler found
if !USE_MSVC! equ 0 if !USE_MINGW! equ 0 (
    echo Error: No suitable compiler found.
    echo.
    echo Please ensure either:
    echo   - MSVC is installed ^(cl.exe in PATH^) and %MSVC_LIB% exists, or
    echo   - MinGW GCC is installed ^(gcc.exe in PATH^) and %MINGW_LIB% exists
    exit /b 1
)

REM ============================================================================
REM Build with the appropriate compiler
REM ============================================================================

if !USE_MSVC! equ 1 (
    echo Building with MSVC: %INPUT_FILE% -^> %OUTPUT_FILE%
    echo.

    REM MSVC compilation command
    cl.exe ^
        /nologo ^
        /std:c++17 ^
        /utf-8 ^
        /O1 ^
        /Ob2 ^
        /Os ^
        /Gy ^
        /GS- ^
        /GR- ^
        /EHs-c- ^
        /wd4530 ^
        /Gs9999999 ^
        /DNOMINMAX ^
        /I"%INCLUDE_DIR%" ^
        /Fe"%OUTPUT_FILE%" ^
        "%INPUT_FILE%" ^
        /link ^
        /NOLOGO ^
        /MACHINE:X86 ^
        /STACK:1048576 ^
        /SUBSYSTEM:CONSOLE ^
        /NODEFAULTLIB ^
        /OPT:REF ^
        /OPT:ICF ^
        /ENTRY:atto_main ^
        /DEBUG:NONE ^
        "%MSVC_LIB%" ^
        kernel32.lib user32.lib ws2_32.lib advapi32.lib crypt32.lib secur32.lib shell32.lib winhttp.lib

    set "BUILD_RESULT=!errorlevel!"

    REM Clean up intermediate files
    if exist "%~n1.obj" del "%~n1.obj" >nul 2>&1

) else (
    echo Building with MinGW GCC: %INPUT_FILE% -^> %OUTPUT_FILE%
    echo.

    REM MinGW GCC compilation command
    gcc.exe ^
        -m32 ^
        -std=c++17 ^
        -finput-charset=UTF-8 ^
        -fexec-charset=UTF-8 ^
        -Os ^
        -ffunction-sections ^
        -fdata-sections ^
        -fno-exceptions ^
        -fno-rtti ^
        -fno-unwind-tables ^
        -fno-asynchronous-unwind-tables ^
        -DNOMINMAX ^
        -I"%INCLUDE_DIR%" ^
        -o "%OUTPUT_FILE%" ^
        "%INPUT_FILE%" ^
        "%MINGW_LIB%" ^
        -m32 ^
        -s ^
        -Wl,--gc-sections ^
        -nostdlib ^
        -Wl,--entry=_atto_main ^
        -Wl,--defsym,main=_atto_main ^
        -Wl,--stack,1048576 ^
        -lkernel32 -luser32 -lws2_32 -ladvapi32 -lcrypt32 -lsecur32 -lshell32 -lwinhttp

    set "BUILD_RESULT=!errorlevel!"
)

REM ============================================================================
REM Check build result
REM ============================================================================

if !BUILD_RESULT! neq 0 (
    echo.
    echo ========================================
    echo Build FAILED
    echo ========================================
    exit /b !BUILD_RESULT!
)

@echo off
setlocal

REM Success!
@echo off

echo.
echo ========================================
echo    BUILD SUCCESS
echo ========================================
echo.
echo Output: %OUTPUT_FILE%
echo.

@echo off

rem Get ESC into %ESC% for ANSI colors
for /F "delims=" %%E in ('echo prompt $E^| cmd') do set "ESC=%%E"

echo   %ESC%[93m,-.___,-.%ESC%[0m
echo   %ESC%[93m\_/_ _\_/%ESC%[0m

rem Yellow face outline, blue eyes, red nose
echo     %ESC%[93m)%ESC%[96mO%ESC%[91m_%ESC%[96mO%ESC%[93m(%ESC%[0m

rem Yellow muzzle, red nose detail
echo    %ESC%[93m{ %ESC%[91m(_)%ESC%[93m }%ESC%[0m

rem Mouth line (note ^^ to print a literal ^)
echo     %ESC%[93m`-^^-'%ESC%[0m

rem Magenta "A" collar
echo    / %ESC%[95m(A)%ESC%[0m \ 

echo %ESC%[94m"Thank you for using Attoboy."%ESC%[0m

exit /b 0
