@echo off
echo.
echo Auswahl
echo.
echo 1. Debug Visual Studio
echo 2. Release Visual Studio
echo 3. Debug MinGW
echo 4. Release MinGW
echo 5. Ende
echo.
choice /n /c 12345
if errorlevel 5 goto ende
if errorlevel 4 goto rm
if errorlevel 3 goto dm
if errorlevel 2 goto rv
if errorlevel 1 goto dv

:dv
echo "Debug Visual Studio"
conan install conanfile_win.txt -s build_type=Debug
premake5 vs2019
goto ende

:rv
echo "Release Visual Studio"
conan install conanfile_win.txt -s build_type=Release
premake5 vs2019
goto ende

:dm
echo "Debug MinGW"
conan install conanfile.txt -s build_type=Debug --build missing --profile default_mingw
goto ende

:rm
echo "Release MinGW"
conan install conanfile.txt -s build_type=Release --build missing --profile default_mingw
goto ende

:ende
echo "Ende"
