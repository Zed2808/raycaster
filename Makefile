CC = g++
CCFLAGS = -O3 -Wall -Wextra -pedantic -std=c++0x
LIBS = -lSDL

SRC = ./src/
OBJ = ./obj/

all: raycaster

debug: CCFLAGS += -g
debug: raycaster

raycaster: $(OBJ)quickcg.o $(OBJ)raycaster.o
	$(CC) $(CCFLAGS) -o raycaster $(OBJ)quickcg.o $(OBJ)raycaster.o $(LIBS)

$(OBJ)quickcg.o: $(SRC)quickcg.cpp $(SRC)quickcg.h
	$(CC) $(CCFLAGS) -c -o $(OBJ)quickcg.o $(SRC)quickcg.cpp

$(OBJ)raycaster.o: $(SRC)raycaster.cpp
	$(CC) $(CCFLAGS) -c -o $(OBJ)raycaster.o $(SRC)raycaster.cpp

debug: raycaster

clean:
	rm -rf $(OBJ)*.o raycaster
