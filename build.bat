@echo off
mkdir release
cd release
qmake ../sources/pathfinding_r_star.pro -r -spec win32-g++ "CONFIG+=release" "CONFIG+=qml_release"
mingw32-make release
cd ..
ECHO Look release/release/pathfinding_r_star.exe
@echo on
