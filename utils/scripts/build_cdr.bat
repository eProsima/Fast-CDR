:: This script build CDR library for any platform.

@echo off

:: Initialize the returned value to 0 (all succesfully)
set errorstatus=0

:: Get the current vesion of CDR
call %EPROSIMADIR%\scripts\common_pack_functions.bat :getVersionFromCPP VERSIONCDR ..\..\include\cpp\Cdr_version.h
if not %errorstatus%==0 goto :exit

:: i86 Platform

:: Release DLL Configuration
:: Clean the visual solution
msbuild "..\..\win32\cpp\CDR\CDR.sln" /t:Clean /p:Configuration="ReleaseDLL" /p:Platform="Win32" /p:VERSION="-%VERSIONCDR%"
:: Build the visual solution
msbuild "..\..\win32\cpp\CDR\CDR.sln" /t:Build /p:Configuration="ReleaseDLL" /p:Platform="Win32" /p:VERSION="-%VERSIONCDR%"
set errorstatus=%ERRORLEVEL%
if not %errorstatus%==0 goto :exit

:: Debug DLL Configuration
:: Clean the visual solution
msbuild "..\..\win32\cpp\CDR\CDR.sln" /t:Clean /p:Configuration="DebugDLL" /p:Platform="Win32" /p:VERSION="-%VERSIONCDR%"
:: Build the visual solution
msbuild "..\..\win32\cpp\CDR\CDR.sln" /t:Build /p:Configuration="DebugDLL" /p:Platform="Win32" /p:VERSION="-%VERSIONCDR%"
set errorstatus=%ERRORLEVEL%
if not %errorstatus%==0 goto :exit

:: Release Configuration
:: Clean the visual solution
msbuild "..\..\win32\cpp\CDR\CDR.sln" /t:Clean /p:Configuration="Release" /p:Platform="Win32" /p:VERSION="-%VERSIONCDR%"
:: Build the visual solution
msbuild "..\..\win32\cpp\CDR\CDR.sln" /t:Build /p:Configuration="Release" /p:Platform="Win32" /p:VERSION="-%VERSIONCDR%"
set errorstatus=%ERRORLEVEL%
if not %errorstatus%==0 goto :exit

:: Debug Configuration
:: Clean the visual solution
msbuild "..\..\win32\cpp\CDR\CDR.sln" /t:Clean /p:Configuration="Debug" /p:Platform="Win32" /p:VERSION="-%VERSIONCDR%"
:: Build the visual solution
msbuild "..\..\win32\cpp\CDR\CDR.sln" /t:Build /p:Configuration="Debug" /p:Platform="Win32" /p:VERSION="-%VERSIONCDR%"
set errorstatus=%ERRORLEVEL%
if not %errorstatus%==0 goto :exit

:: x64 Platform

:: Release DLL Configuration
:: Clean the visual solution
msbuild "..\..\win32\cpp\CDR\CDR.sln" /t:Clean /p:Configuration="ReleaseDLL" /p:Platform="x64" /p:VERSION="-%VERSIONCDR%"
:: Build the visual solution
msbuild "..\..\win32\cpp\CDR\CDR.sln" /t:Build /p:Configuration="ReleaseDLL" /p:Platform="x64" /p:VERSION="-%VERSIONCDR%"
set errorstatus=%ERRORLEVEL%
if not %errorstatus%==0 goto :exit

:: Debug DLL Configuration
:: Clean the visual solution
msbuild "..\..\win32\cpp\CDR\CDR.sln" /t:Clean /p:Configuration="DebugDLL" /p:Platform="x64" /p:VERSION="-%VERSIONCDR%"
:: Build the visual solution
msbuild "..\..\win32\cpp\CDR\CDR.sln" /t:Build /p:Configuration="DebugDLL" /p:Platform="x64" /p:VERSION="-%VERSIONCDR%"
set errorstatus=%ERRORLEVEL%
if not %errorstatus%==0 goto :exit

:: Release Configuration
:: Clean the visual solution
msbuild "..\..\win32\cpp\CDR\CDR.sln" /t:Clean /p:Configuration="Release" /p:Platform="x64" /p:VERSION="-%VERSIONCDR%"
:: Build the visual solution
msbuild "..\..\win32\cpp\CDR\CDR.sln" /t:Build /p:Configuration="Release" /p:Platform="x64" /p:VERSION="-%VERSIONCDR%"
set errorstatus=%ERRORLEVEL%
if not %errorstatus%==0 goto :exit

:: Debug Configuration
:: Clean the visual solution
msbuild "..\..\win32\cpp\CDR\CDR.sln" /t:Clean /p:Configuration="Debug" /p:Platform="x64" /p:VERSION="-%VERSIONCDR%"
:: Build the visual solution
msbuild "..\..\win32\cpp\CDR\CDR.sln" /t:Build /p:Configuration="Debug" /p:Platform="x64" /p:VERSION="-%VERSIONCDR%"
set errorstatus=%ERRORLEVEL%
if not %errorstatus%==0 goto :exit

goto :exit

:: Function exit ::
:exit
if %errorstatus%==0 (echo "BUILD SUCCESSFULLY") else (echo "BUILD FAILED")
goto :EOF

