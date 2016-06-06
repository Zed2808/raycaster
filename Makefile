CC = g++
CCFLAGS = -O3 -Wall -Wextra -pedantic -std=c++0x
LIBS = -lSDL2_mixer

ifeq ($(OS),Windows_NT)
	LIBS += -lmingw32 -lSDL2main -lSDL2
	EXECUTABLE += raycaster.exe
else
	LIBS += -lSDL2
	EXECUTABLE += raycaster
endif

SRC = ./src/
OBJ = ./obj/

all: $(EXECUTABLE)

debug: CCFLAGS += -g
debug: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)smallcg.o $(OBJ)entity.o $(OBJ)raycaster.o
	$(CC) $(CCFLAGS) -o raycaster $(OBJ)smallcg.o $(OBJ)entity.o $(OBJ)raycaster.o $(LIBS)

$(OBJ)smallcg.o: $(SRC)smallcg.cpp $(SRC)smallcg.h
	$(CC) $(CCFLAGS) -c -o $(OBJ)smallcg.o $(SRC)smallcg.cpp

$(OBJ)entity.o: $(SRC)entity.cpp $(SRC)entity.hpp
	$(CC) $(CCFLAGS) -c -o $(OBJ)entity.o $(SRC)entity.cpp

$(OBJ)raycaster.o: $(SRC)raycaster.cpp
	$(CC) $(CCFLAGS) -c -o $(OBJ)raycaster.o $(SRC)raycaster.cpp

debug: $(EXECUTABLE)

clean:
	rm -rf $(OBJ)*.o $(EXECUTABLE)
