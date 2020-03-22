.PHONY: game

game:
	g++ -std=c++17 -Wall Main.cpp Game.cpp `sdl2-config --cflags --libs` && ./a.out
