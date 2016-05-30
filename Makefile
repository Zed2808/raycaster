CC = g++ -O3 -Wall -Wextra -pedantic -std=c++0x
LIBS = -lSDL

SRC = ./src/
OBJ = ./obj/

all: raycaster

raycaster: $(OBJ)quickcg.o $(OBJ)raycaster.o
	$(CC) -o raycaster $(OBJ)quickcg.o $(OBJ)raycaster.o $(LIBS)

$(OBJ)quickcg.o: $(SRC)quickcg.cpp $(SRC)quickcg.h
	$(CC) -c -o $(OBJ)quickcg.o $(SRC)quickcg.cpp

$(OBJ)raycaster.o: $(SRC)raycaster.cpp
	$(CC) -c -o $(OBJ)raycaster.o $(SRC)raycaster.cpp

clean:
	rm -rf $(OBJ)*.o raycaster
