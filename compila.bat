cls
gcc -c interface.c -o interface.o
@echo.
@echo ponha as structs, etc em algoritmo.h
pause
gcc -c algoritmo.c -o algoritmo.o
@echo.
@echo tire as structs, etc de algoritmo.h
pause
gcc -c main.c -o main.o
gcc main.o interface.o algoritmo.o -o "batalhas.exe"
start batalhas.exe
