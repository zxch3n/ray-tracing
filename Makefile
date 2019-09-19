build:
	g++ -g -std=c++11 main.cpp -o main.exe

run:
	./main.exe > output.ppm

start:
	g++ -std=c++11 main.cpp -o main_fast.exe -O3
	./main_fast.exe > output.ppm