@echo off
windres recursos.rc recursos.o
g++ main.cpp recursos.o -o Bloc.exe -mwindows -lcomdlg32 -lgdi32 -ldwmapi
pause