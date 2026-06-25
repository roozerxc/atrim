@echo off
setlocal
    set GENERATEDBID=%DATE:~6,4%%DATE:~0,2%%DATE:~3,2%%TIME:~0,2%%TIME:~3,2%%TIME:~6,2%
    set GENERATEDBID=%GENERATEDBID: =0%

(
echo #include "impl/PlatformBuildID.h"
echo.
echo ///////////////////////////////////////////
echo // atrim buildcounter generated file
echo // DO NOT HAND EDIT THIS!
echo.
echo static char gsBuildID[16] = "%GENERATEDBID%";
echo.
echo const char* GetBuildID(^)
echo {
echo     return gsBuildID;
echo }
)>core\sources\impl\PlatformBuildID.cpp

echo Generated Build ID %GENERATEDBID% to file \core\sources\impl\PlatformBuildID.cpp
exit
