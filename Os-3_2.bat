@echo off
set /p content=<%TEMP%\os_1_2.tmp
echo %DATE%>%TEMP%\os_1_2.tmp

rem %TEMP%\os_1_2.tmp

if %DATE%==%content% goto end

set /a answer=%content:~0,2%+1

if %DATE:~6,2%==%content:~6,2% (
	if %DATE:~3,2%==%content:~3,2% if %answer%==%DATE:~0,2% goto end
)
notepad.exe


:end
echo EEENd
pause
@echo on
exit /b




