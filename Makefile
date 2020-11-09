CC		:= g++
D_FLAGS := -g -Wall -Wextra
C_FLAGS := -std=c++11 -static -static-libgcc -static-libstdc++ 

BIN		:= bin
SRC		:= src
INCLUDE	:= -I ./include -I ../Mantaray/include -I ../Mantaray/external/include
LIB		:= -L ../Mantaray/external/lib -L../Mantaray/lib

LIBRARIES	:= -lMantaray -lglfw3 -lgdi32 -lglad

EXECUTABLE_NAME := chip8
ifeq ($(OS),Windows_NT)
EXECUTABLE	:= $(EXECUTABLE_NAME).exe
else
EXECUTABLE	:= $(EXECUTABLE_NAME)
endif

all: $(BIN)/$(EXECUTABLE)

clean:
	$(RM) $(BIN)/$(EXECUTABLE)

run: all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CC) $(D_FLAGS) $(C_FLAGS) $(INCLUDE) $^ $(LIB) $(LIBRARIES) -o $@
