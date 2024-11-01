CC = g++
SRC = src/*.cpp 
INC = include/*.hpp

all: Chess

Chess: $(SRC) $(INC) Makefile
	@echo "Compiling source files.."
	$(CC) $(SRC) -o Chess
