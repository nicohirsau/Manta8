CC		:= g++
D_FLAGS := -g -Wall -Wextra
C_FLAGS := -std=c++11

BIN		:= bin
SRC		:= src
INCLUDE	:= -iquote include -I ./import/include -I ../Mantaray/include -I ../Mantaray/external/include

EXECUTABLE_NAME := build

ifeq ($(OS),Windows_NT)
C_FLAGS		+= -static -static-libgcc -static-libstdc++
EXECUTABLE	:= $(EXECUTABLE_NAME).exe
LIB		:= -L ../Mantaray/external/lib/windows -L ../Mantaray/lib
LIBRARIES	:= -lmantaray -lglfw3 -lgdi32 -lbox2d
else
C_FLAGS		+= -no-pie
EXECUTABLE	:= $(EXECUTABLE_NAME)
LIB		:= -L ../Mantaray/external/lib/linux -L ../Mantaray/lib
LIBRARIES	:= -lmantaray -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor 
endif

all: $(BIN)/$(EXECUTABLE)

clean:
	$(RM) $(BIN)/$(EXECUTABLE)

run: all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CC) $(D_FLAGS) $(C_FLAGS) $(INCLUDE)  $^ $(LIB) $(LIBRARIES) -o $@
