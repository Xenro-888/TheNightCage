TheNightCage: main.o tile.o player.o board.o game.o
	g++ main.o tile.o player.o board.o game.o -o TheNightCage

main.o: main.cpp
	g++ -c main.cpp

tile.o: tile.cpp
	g++ -c tile.cpp

player.o: player.cpp
	g++ -c player.cpp

board.o: board.cpp
	g++ -c board.cpp

game.o: game.cpp
	g++ -c game.cpp

clean:
	rm *.o TheNightCage

run:
	./TheNightCage