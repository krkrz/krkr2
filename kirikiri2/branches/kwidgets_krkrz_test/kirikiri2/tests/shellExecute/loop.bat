@echo off
set N=1
:LOOP
echo loop %N%
ping -n 3 localhost > nul
set /a N=N+1
goto LOOP
