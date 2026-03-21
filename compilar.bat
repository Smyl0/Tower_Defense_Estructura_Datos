@echo off
echo Compilando Tower Defense con Raylib...

C:\mingw64\bin\g++ -o TowerDefense.exe main.cpp ^
    -I C:\raylib\include ^
    -L C:\raylib\lib ^
    -lraylib -lopengl32 -lgdi32 -lwinmm ^
    -std=c++17 -O2

if %errorlevel% == 0 (
    echo.
    echo Compilacion exitosa! Ejecutando...
    TowerDefense.exe
) else (
    echo.
    echo ERROR de compilacion.
    echo intento de red
    echo intenta denuevo
    pause
)
