CC = g++
CCFLAGS = -O3 -Wall -Wextra -pedantic -std=c++0x
LIBS = -lSDL2 -lSDL2_mixer

SRC = ./src/
OBJ = ./obj/

all: raycaster

debug: CCFLAGS += -g
debug: raycaster

raycaster: $(OBJ)smallcg.o $(OBJ)raycaster.o
	$(CC) $(CCFLAGS) -o raycaster $(OBJ)smallcg.o $(OBJ)raycaster.o $(LIBS)

$(OBJ)smallcg.o: $(SRC)smallcg.cpp $(SRC)smallcg.h
	$(CC) $(CCFLAGS) -c -o $(OBJ)smallcg.o $(SRC)smallcg.cpp

$(OBJ)raycaster.o: $(SRC)raycaster.cpp
	$(CC) $(CCFLAGS) -c -o $(OBJ)raycaster.o $(SRC)raycaster.cpp

debug: raycaster

clean:
	rm -rf $(OBJ)*.o raycaster
