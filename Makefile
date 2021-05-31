all: main
main: main.cpp
	g++ -I/usr/local/include main.cpp -L/usr/local/lib -lcpr -lpthread -Ofast -o sniper
