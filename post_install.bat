@echo off
set ARGS=/A /SE /SW /SA
if "%PROCESSOR_ARCHITECTURE%" == "AMD64" (
  drivers\dpinst-amd64.exe %ARGS%
  drivers\x64\mbed_64.exe
) ELSE IF "%PROCESSOR_ARCHITEW6432%" == "AMD64" (
  drivers\dpinst-amd64.exe %ARGS%
  drivers\x64\mbed_64.exe
) ELSE (
  drivers\dpinst-x86.exe %ARGS%
  drivers\x64\mbed_32.exe
)
exit /b 0