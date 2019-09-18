build:
	g++ -g -std=c++11 main.cpp -o main.exe

run:
	./main.exe > output.ppm

start:
	make build
	make run