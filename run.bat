@echo off
echo Welcome!
echo Compiling source code...

g++ -o build\main src\main.cpp src\Nurse.cpp src\Hospital.cpp src\Optimize_Schedule.cpp src\Schedule.cpp

cd "%cd%\build"

echo Press any key to run the program
pause >nul
cls
"%cd%\main"

