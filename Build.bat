cls
@echo off
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make -j8
cd ..
rd /s /q build